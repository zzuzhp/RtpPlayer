#include "Depacketizers/RtpDepacketizerVp8.h"

#include <stdint.h>
#include <assert.h>
#include <memory.h>
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpDepacketizerVp8::RtpDepacketizerVp8() : RtpDepacketizer("vp8 depacketizer"),
                                           m_frame_len(0),
                                           m_packets(0),
                                           m_frame_type(kEmptyFrame),
                                           m_key_required(true)
{
    ///< TODO: add an init method to do these initializations
    m_frame = new (std::nothrow)uint8_t[MAX_ES_FRAME_SIZE];

    if (!m_frame)
    {
        RP_LOG_E("RtpDepacketizerVp8 failed to alloc memory.");
    }
}

RtpDepacketizerVp8::~RtpDepacketizerVp8()
{
    delete[] m_frame;
}

//
// VP8 format:
//
// Payload descriptor
//       0 1 2 3 4 5 6 7
//      +-+-+-+-+-+-+-+-+
//      |X|R|N|S|PartID | (REQUIRED)
//      +-+-+-+-+-+-+-+-+
// X:   |I|L|T|K|  RSV  | (OPTIONAL)
//      +-+-+-+-+-+-+-+-+
// I:   |   PictureID   | (OPTIONAL)
//      +-+-+-+-+-+-+-+-+
// L:   |   TL0PICIDX   | (OPTIONAL)
//      +-+-+-+-+-+-+-+-+
// T/K: |TID:Y| KEYIDX  | (OPTIONAL)
//      +-+-+-+-+-+-+-+-+
//
// Payload header (considered part of the actual payload, sent to decoder)
//       0 1 2 3 4 5 6 7
//      +-+-+-+-+-+-+-+-+
//      |Size0|H| VER |P|
//      +-+-+-+-+-+-+-+-+
//      |      ...      |
//      +               +

/* Parses the RTP payload, parsed result will be saved in | parsed_payload | */
int
RtpDepacketizerVp8::parse(const unsigned char * payload_data,
                          int                   payload_data_length,
                          bool                  marker,
                          unsigned int          pts,
                          int                   payload_type,
                          MediaFrame          * frame)
{
    CProThreadMutexGuard mon(m_lock);

    ParsedPayload   payload_temp;
    ParsedPayload * parsed_payload = &payload_temp;

    assert(parsed_payload != NULL);
    if (payload_data_length == 0)
    {
        return DEPACKETIZER_ERROR;
    }

    ++m_packets;

    // Parse mandatory first byte of payload descriptor.
    bool    extension = (*payload_data & 0x80) ? true : false;                  // X bit
    bool    beginning_of_partition = (*payload_data & 0x10) ? true : false;     // S bit
    int     partition_id = (*payload_data & 0x0F);                              // PartID field

    parsed_payload->type.Video.width                                = 0;
    parsed_payload->type.Video.height                               = 0;
    parsed_payload->type.Video.isFirstPacket                        = beginning_of_partition && (partition_id == 0);
    parsed_payload->type.Video.simulcastIdx                         = 0;
    parsed_payload->type.Video.codec                                = kRtpVideoVp8;
    parsed_payload->type.Video.codecHeader.VP8.nonReference         = (*payload_data & 0x20) ? true : false;  // N bit
    parsed_payload->type.Video.codecHeader.VP8.partitionId          = partition_id;
    parsed_payload->type.Video.codecHeader.VP8.beginningOfPartition = beginning_of_partition;
    parsed_payload->type.Video.codecHeader.VP8.pictureId            = kNoPictureId;
    parsed_payload->type.Video.codecHeader.VP8.tl0PicIdx            = kNoTl0PicIdx;
    parsed_payload->type.Video.codecHeader.VP8.temporalIdx          = kNoTemporalIdx;
    parsed_payload->type.Video.codecHeader.VP8.layerSync            = false;
    parsed_payload->type.Video.codecHeader.VP8.keyIdx               = kNoKeyIdx;

    if (partition_id > 8)
    {
        // Weak check for corrupt payload_data: PartID MUST NOT be larger than 8.
        return DEPACKETIZER_ERROR;
    }

    // Advance payload_data and decrease remaining payload size.
    payload_data++;
    if (payload_data_length <= 1)
    {
        return DEPACKETIZER_ERROR;
    }

    payload_data_length--;

    if (extension) 
    {
        const int parsed_bytes = ParseVP8Extension(&parsed_payload->type.Video.codecHeader.VP8, payload_data, payload_data_length);

        if (parsed_bytes < 0)
        {
            return DEPACKETIZER_ERROR;
        }

        payload_data += parsed_bytes;
        payload_data_length -= parsed_bytes;

        if (payload_data_length == 0)
        {
            return DEPACKETIZER_ERROR;
        }
    }

    // Read P bit from payload header (only at beginning of first partition).
    if (beginning_of_partition && partition_id == 0)
    {
        parsed_payload->frame_type = (*payload_data & 0x01) ? kVideoFrameDelta : kVideoFrameKey;
        m_frame_type = parsed_payload->frame_type;
    }
    else
    {
        parsed_payload->frame_type = kVideoFrameDelta;
    }

    if (ParseVP8FrameSize(parsed_payload, payload_data, payload_data_length) != 0)
    {
        return DEPACKETIZER_ERROR;
    }

    parsed_payload->payload = payload_data;
    parsed_payload->payload_length = payload_data_length;

    if (payload_data_length > MAX_ES_FRAME_SIZE - m_frame_len)
    {
        RP_LOG_W("es frame too large, increase 'MAX_ES_FAME_SIZE'(%d) and recompile!", MAX_ES_FRAME_SIZE);

        m_frame_type = kEmptyFrame;
        m_frame_len  = 0;
        m_packets    = 0;

        return DEPACKETIZER_ERROR;
    }

    memcpy(m_frame + m_frame_len, payload_data, payload_data_length);
    m_frame_len += payload_data_length;

    if (marker)
    {
        if (m_key_required && m_frame_type != kVideoFrameKey)
        {
            RP_LOG_W("vp8 depacketizer: a key frame is expected, drop this one(%d).", parsed_payload->frame_type);
            m_packets   = 0;
            m_frame_len = 0;
        }
        else
        {
            if (frame)
            {
                frame->data             = m_frame;
                frame->len              = m_frame_len;
                frame->pts              = pts;
                frame->rtp_payload_type = payload_type;
                frame->flags            = (m_frame_type == kVideoFrameKey) ? FRAME_IS_SYNC_POINT : 0;
                frame->packets          = m_packets;
            }

            m_key_required = false;
            m_packets      = 0;
            m_frame_len    = 0;

            return DEPACKETIZER_FRAME;
        }
    }

    return DEPACKETIZER_NEED_MORE;
}

void
RtpDepacketizerVp8::set_sync_point()
{
    CProThreadMutexGuard mon(m_lock);

    m_key_required = true;
    m_frame_len    = 0;
}

int 
RtpDepacketizerVp8::ParseVP8PictureID(RTPVideoHeaderVP8* vp8,
                                      const uint8_t** data,
                                      size_t* data_length,
                                      size_t* parsed_bytes)
{
    assert(vp8 != NULL);
    if (*data_length == 0)
        return -1;

    vp8->pictureId = (**data & 0x7F);
    if (**data & 0x80) 
    {
        (*data)++;
        (*parsed_bytes)++;
        if (--(*data_length) == 0)
            return -1;
        // PictureId is 15 bits
        vp8->pictureId = (vp8->pictureId << 8) + **data;
    }

    (*data)++;
    (*parsed_bytes)++;
    (*data_length)--;

    return 0;
}

int 
RtpDepacketizerVp8::ParseVP8Tl0PicIdx(RTPVideoHeaderVP8* vp8,
                                      const uint8_t** data,
                                      size_t* data_length,
                                      size_t* parsed_bytes) 
{
    assert(vp8 != NULL);
    if (*data_length == 0)
        return -1;

    vp8->tl0PicIdx = **data;
    (*data)++;
    (*parsed_bytes)++;
    (*data_length)--;
    return 0;
}

int 
RtpDepacketizerVp8::ParseVP8TIDAndKeyIdx(RTPVideoHeaderVP8* vp8,
                                         const uint8_t** data,
                                         size_t* data_length,
                                         size_t* parsed_bytes,
                                         bool has_tid,
                                         bool has_key_idx) 
{
    assert(vp8 != NULL);
    if (*data_length == 0)
        return -1;

    if (has_tid) 
    {
        vp8->temporalIdx = ((**data >> 6) & 0x03);
        vp8->layerSync = (**data & 0x20) ? true : false;  // Y bit
    }

    if (has_key_idx) 
    {
        vp8->keyIdx = (**data & 0x1F);
    }

    (*data)++;
    (*parsed_bytes)++;
    (*data_length)--;

    return 0;
}

int 
RtpDepacketizerVp8::ParseVP8Extension(RTPVideoHeaderVP8* vp8, const uint8_t* data, size_t data_length) 
{
    assert(vp8 != NULL);
    assert(data_length > 0);
    size_t parsed_bytes = 0;
    // Optional X field is present.
    bool has_picture_id = (*data & 0x80) ? true : false;   // I bit
    bool has_tl0_pic_idx = (*data & 0x40) ? true : false;  // L bit
    bool has_tid = (*data & 0x20) ? true : false;          // T bit
    bool has_key_idx = (*data & 0x10) ? true : false;      // K bit

    // Advance data and decrease remaining payload size.
    data++;
    parsed_bytes++;
    data_length--;

    if (has_picture_id) 
    {
        if (ParseVP8PictureID(vp8, &data, &data_length, &parsed_bytes) != 0) 
        {
            return -1;
        }
    }

    if (has_tl0_pic_idx) 
    {
        if (ParseVP8Tl0PicIdx(vp8, &data, &data_length, &parsed_bytes) != 0) 
        {
            return -1;
        }
    }

    if (has_tid || has_key_idx) 
    {
        if (ParseVP8TIDAndKeyIdx(vp8, &data, &data_length, &parsed_bytes, has_tid, has_key_idx) != 0) 
        {
            return -1;
        }
    }

    return static_cast<int>(parsed_bytes);
}

int
RtpDepacketizerVp8::ParseVP8FrameSize(ParsedPayload * parsed_payload, const uint8_t* data, size_t data_length) 
{
    assert(parsed_payload != NULL);
    if (parsed_payload->frame_type != kVideoFrameKey)
    {
        // Included in payload header for I-frames.
        return 0;
    }

    if (data_length < 10) 
    {
        // For an I-frame we should always have the uncompressed VP8 header
        // in the beginning of the partition.
        return -1;
    }

    parsed_payload->type.Video.width = ((data[7] << 8) + data[6]) & 0x3FFF;
    parsed_payload->type.Video.height = ((data[9] << 8) + data[8]) & 0x3FFF;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
