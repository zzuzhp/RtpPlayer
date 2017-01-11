#ifndef ___RTPDEPACKETIZERVP8_H___
#define ___RTPDEPACKETIZERVP8_H___

#include "pro_thread_mutex.h"
#include "Depacketizers/RtpDepacketizer.h"
#include "Common/common.h"

const int16_t kNoPictureId                  = -1;
const int16_t kMaxOneBytePictureId          = 0x7F;    ///< 7 bits
const int16_t kMaxTwoBytePictureId          = 0x7FFF;  ///< 15 bits
const int16_t kNoTl0PicIdx                  = -1;
const uint8_t kNoTemporalIdx                = 0xFF;
const uint8_t kNoSpatialIdx                 = 0xFF;
const uint8_t kNoGofIdx                     = 0xFF;
const uint8_t kNumVp9Buffers                = 8;
const size_t kMaxVp9RefPics                 = 3;
const size_t kMaxVp9FramesInGof             = 0xFF;    ///< 8 bits
const size_t kMaxVp9NumberOfSpatialLayers   = 8;
const int kNoKeyIdx                         = -1;

enum FrameType
{
    kEmptyFrame         = 0,
    kAudioFrameSpeech   = 1,
    kAudioFrameCN       = 2,
    kVideoFrameKey      = 3,
    kVideoFrameDelta    = 4
};

struct RTPVideoHeaderVP8 
{
    void InitRTPVideoHeaderVP8() 
    {
        nonReference            = false;
        pictureId               = kNoPictureId;
        tl0PicIdx               = kNoTl0PicIdx;
        temporalIdx             = kNoTemporalIdx;
        layerSync               = false;
        keyIdx                  = kNoKeyIdx;
        partitionId             = 0;
        beginningOfPartition    = false;
    }

    bool    nonReference;           ///< Frame is discardable.
    int16_t pictureId;              ///< Picture ID index, 15 bits;
                                    ///< kNoPictureId if PictureID does not exist.
    int16_t tl0PicIdx;              ///< TL0PIC_IDX, 8 bits;
                                    ///< kNoTl0PicIdx means no value provided.
    uint8_t temporalIdx;            ///< Temporal layer index, or kNoTemporalIdx.
    bool    layerSync;              ///< This frame is a layer sync frame.
                                    ///< Disabled if temporalIdx == kNoTemporalIdx.
    int     keyIdx;                 ///< 5 bits; kNoKeyIdx means not used.
    int     partitionId;            ///< VP8 partition ID
    bool    beginningOfPartition;   ///< True if this packet is the first
                                    ///< in a VP8 partition. Otherwise false
};

union RTPVideoTypeHeader 
{
    RTPVideoHeaderVP8 VP8;
};

enum RtpVideoCodecTypes
{
    kRtpVideoNone,
    kRtpVideoGeneric,
    kRtpVideoVp8,
    kRtpVideoVp9,
    kRtpVideoH264
};

struct PlayoutDelay 
{
    int min_ms;
    int max_ms;
};

enum VideoRotation
{
    kVideoRotation_0    = 0,
    kVideoRotation_90   = 90,
    kVideoRotation_180  = 180,
    kVideoRotation_270  = 270
};

/* Since RTPVideoHeader is used as a member of a union, it can't have a non-trivial default constructor. */
struct RTPVideoHeader 
{
    uint16_t            width;
    uint16_t            height;
    VideoRotation       rotation;

    PlayoutDelay        playout_delay;

    bool                isFirstPacket;  ///< first packet in frame
    uint8_t             simulcastIdx;   ///< Index if the simulcast encoder creating this frame, 0 if not using simulcast.

    RtpVideoCodecTypes  codec;
    RTPVideoTypeHeader  codecHeader;
};

union RTPTypeHeader
{
    RTPVideoHeader Video;
};

struct ParsedPayload
{
    const uint8_t * payload;
    size_t          payload_length;
    FrameType       frame_type;
    RTPTypeHeader   type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//// VP8 depacketizer

class RtpDepacketizerVp8 : public RtpDepacketizer
{
public:

    RtpDepacketizerVp8();

    ~RtpDepacketizerVp8();

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
    int  parse(const unsigned char * packet,
               int                   packet_len,
               bool                  marker,
               unsigned int          pts,
               int                   payload_type,
               MediaFrame          * frame);

    void set_sync_point();

private:

    int ParseVP8PictureID(RTPVideoHeaderVP8* vp8,
                          const uint8_t** data,
                          size_t* data_length,
                          size_t* parsed_bytes);

    int ParseVP8Tl0PicIdx(RTPVideoHeaderVP8* vp8,
                          const uint8_t** data,
                          size_t* data_length,
                          size_t* parsed_bytes);

    int ParseVP8TIDAndKeyIdx(RTPVideoHeaderVP8* vp8,
                             const uint8_t** data,
                             size_t* data_length,
                             size_t* parsed_bytes,
                             bool has_tid,
                             bool has_key_idx);

    int ParseVP8Extension(RTPVideoHeaderVP8* vp8, const uint8_t* data, size_t data_length);

    int ParseVP8FrameSize(ParsedPayload * parsed_payload, const uint8_t* data, size_t data_length);

private:

    bool                      m_key_required;
    FrameType                 m_frame_type;

    int                       m_frame_len;
    uint8_t                 * m_frame;

    int                       m_packets;

    CProThreadMutex           m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPDEPACKETIZERVP8_H___
