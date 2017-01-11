#include "Depacketizers/RtpDepacketizerH264.h"
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpDepacketizerH264::RtpDepacketizerH264() : RtpDepacketizer("h264 depacketizer"),
                                             m_nalu_len(0),
                                             m_frame_len(0),
                                             m_packets(0),
                                             m_sps_sentry(true),
                                             m_pps_sentry(true),
                                             m_idr_sentry(true)
{
    ///< TODO: add an init method to do these initializations
    m_nalu  = new (std::nothrow)unsigned char[MAX_ES_FRAME_SIZE];
    m_frame = new (std::nothrow)unsigned char[MAX_ES_FRAME_SIZE];

    if (!m_nalu || !m_frame)
    {
        RP_LOG_E("RtpDepacketizerH264 failed to alloc memory.");
    }
}

RtpDepacketizerH264::~RtpDepacketizerH264()
{
    delete[] m_nalu;
    delete[] m_frame;
}

int
RtpDepacketizerH264::parse(const unsigned char * packet,
                           int                   packet_len,
                           bool                  marker,
                           unsigned int          pts,
                           int                   payload_type,
                           MediaFrame          * frame)
{
    /* decode RFC3984 packets */
    CProThreadMutexGuard mon(m_lock);

    int status = DEPACKETIZER_NEED_MORE;
    if (packet == 0 || packet_len <= 0)
    {
        /* invalid param */
        return DEPACKETIZER_ERROR;
    }

    ++m_packets;

#define PACKET_TYPE(x) ((unsigned char)(x) & 0x1f)
#define NALU_TYPE(x)   PACKET_TYPE(x)

#define CHECK_SIZE(x) \
        if ((x) > MAX_ES_FRAME_SIZE - m_nalu_len) \
        { \
            RP_LOG_W("h264 nalu too large, increase 'MAX_ES_FAME_SIZE'(%d) and recompile!", MAX_ES_FRAME_SIZE); \
            m_nalu_len  = 0; \
            m_frame_len = 0; \
            m_packets   = 0; \
            \
            goto fail; \
        }

#define ADD_STARTCODE \
        CHECK_SIZE(STARTCODE_LENGTH) \
        m_nalu[m_nalu_len++] = 0; \
        m_nalu[m_nalu_len++] = 0; \
        m_nalu[m_nalu_len++] = 0; \
        m_nalu[m_nalu_len++] = 1;

#define START_NALU(header) \
        ADD_STARTCODE \
        CHECK_SIZE(1) \
        m_nalu[m_nalu_len++] = header;

#define ADD_BUFFER(s, l) \
        CHECK_SIZE(l) \
        memcpy(m_nalu + m_nalu_len, s, l); \
        m_nalu_len += l;

#define DEC_NALU(last) \
        status = decode_nalu(m_nalu, m_nalu_len, NALU_TYPE(m_nalu[STARTCODE_LENGTH]), last, pts, payload_type, frame); \
        m_nalu_len = 0;

    unsigned char packet_type = PACKET_TYPE(packet[0]);

    switch (packet_type)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        {
            /* single nal unit packet */
            ADD_STARTCODE
            ADD_BUFFER(packet, packet_len)
            DEC_NALU(marker)

            break;
        }
    case 24:
        {
            /* STAPA */
            --packet_len;
            ++packet;

            do
            {
                unsigned short len_hi = *(unsigned char *)packet++;
                unsigned char  len_lo = *(unsigned char *)packet++;

                packet_len -= 2;

                len_hi <<= 8;
                len_hi |= len_lo;

                if (len_hi > packet_len)
                {
                    RP_LOG_E("H264 depacketizer: available data less than required!");
                    goto fail;
                }

                ADD_STARTCODE

                ADD_BUFFER(packet, len_hi);

                packet     += len_hi;
                packet_len -= len_hi;

                if (packet_len < 0)
                {
                    RP_LOG_E("H264 depacketizer: STAPA wrong packet len!");
                    goto fail;
                }

                DEC_NALU(!packet_len ? marker : false)
            } while (packet_len > 0);

            break;
        }
    case 28:
        {
            /* FUA */
            unsigned char temp        = *packet++ & 0xe0;
            unsigned char nalu_header = temp | (*packet & 0x1f);

            bool start = (*packet & 0x80) > 0;
            bool end   = (*packet++ & 0x40) > 0;

            packet_len -= 2;
            if (packet_len < 0)
            {
                RP_LOG_E("H264 depacketizer: wrong packet size for one FUA packet!");
                goto fail;
            }

            if (start && end)
            {
                RP_LOG_E("H264 depacketizer: sanity check failed for one FUA packet!");
                goto fail;
            }

            if (start)
            {
                START_NALU(nalu_header)
            }

            ADD_BUFFER(packet, packet_len);

            if (end)
            {
                DEC_NALU(marker)
            }

            break;
        }
    default:
        {
            /* unsupported packet type */
            {
                const char ascii2char[]       = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
                char       leading_bytes[128] = "0x";

                for (int i = 0; i < RTPPLAYER_MIN(packet_len, 8); ++i)
                {
                    sprintf(&leading_bytes[strlen(leading_bytes)], "%c%c", ascii2char[(packet[i] & 0xf0) >> 4], ascii2char[packet[i] & 0x0f]);
                }

                /* FYI: dump the leading bytes */
                RP_LOG_W("H264 depacketizer unknown packet type: %d[%s].", packet_type, leading_bytes);
            }

            goto fail;
        }
    }

    return status;

fail:

    /* abandon the current nalu */
    m_nalu_len = 0;

    return DEPACKETIZER_ERROR;
}

void
RtpDepacketizerH264::set_sync_point()
{
    CProThreadMutexGuard mon(m_lock);

    m_sps_sentry = true;
    m_pps_sentry = true;
    m_idr_sentry = true;

    m_nalu_len   = 0;
    m_frame_len  = 0;
}

bool
RtpDepacketizerH264::check_random_access_unit(unsigned char nalu_type)
{
    if (nalu_type == SPS_TYPE || nalu_type == PPS_TYPE)
    {
        if (nalu_type == SPS_TYPE)
        {
            m_sps_sentry = false;
        }
        else
        {
            m_pps_sentry = false;
        }

        /* always write sps/pps */
        return true;
    }

    if (m_sps_sentry || m_pps_sentry)
    {
        RP_LOG_I("H264 depacketizer: expecting SPS/PPS to start with(nalu: %d)!", nalu_type);
        return false;
    }

    /* we have sps/pps, check for the IDR frame */
    if (nalu_type != IDR_TYPE)
    {
        RP_LOG_E("H264 depacketizer: IDR frame is expected after SPS/PPS(nalu: %d)!", nalu_type);
        return false;
    }

    m_idr_sentry = false;

    return true;
}

int
RtpDepacketizerH264::decode_nalu(const unsigned char * nalu,
                                 int                   nalu_len,
                                 int                   nalu_type,
                                 bool                  frame_end,
                                 unsigned int          pts,
                                 int                   payload_type,
                                 MediaFrame          * frame)
{
    int flags = 0;
    if (!nalu || nalu_len <= 0)
    {
        return DEPACKETIZER_ERROR;
    }

    if (nalu_len > MAX_ES_FRAME_SIZE - m_frame_len)
    {
        RP_LOG_W("h264 frame too large, increase 'MAX_ES_FAME_SIZE'(%d) and recompile!", MAX_ES_FRAME_SIZE);

        /* we copy as many bytes as we can, and start decode the INCOMPLETE frame */
        flags    |= FRAME_IS_INCOMPLETE;
        nalu_len  = MAX_ES_FRAME_SIZE - m_frame_len;
        frame_end = true; ///< !!!
    }

    if (m_sps_sentry || m_pps_sentry || m_idr_sentry)
    {
        if (!check_random_access_unit(nalu_type))
        {
            return DEPACKETIZER_NEED_MORE;
        }
    }

    if (nalu_type == IDR_TYPE)
    {
        flags |= FRAME_IS_SYNC_POINT;
    }

    /* we callback only at frame boundaries */
    if (nalu_len > 0)
    {
        memcpy(m_frame + m_frame_len, nalu, nalu_len);
        m_frame_len += nalu_len;
    }

    if (frame_end)
    {
        /* this is the last nalu of a frame */
        /* we got a frame */
        if (frame)
        {
            frame->data             = m_frame;
            frame->len              = m_frame_len;
            frame->pts              = pts;
            frame->rtp_payload_type = payload_type;
            frame->flags            = flags;
            frame->packets          = m_packets;
        }

        m_packets   = 0;
        m_frame_len = 0;

        return DEPACKETIZER_FRAME;
    }

    return DEPACKETIZER_NEED_MORE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
