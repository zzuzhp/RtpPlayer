#include "Streams/VideoStream.h"
#include "Depacketizers/RtpDepacketizerVp8.h"
#include "Depacketizers/RtpDepacketizerH264.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

VideoStream *
VideoStream::create_instance(RtpStreamObserver * observer)
{
    if (!observer)
    {
        return NULL;
    }

    return new(std::nothrow) VideoStream(observer);
}

VideoStream::VideoStream(RtpStreamObserver * observer) : RtpStream(observer, "video stream"),
                                                         m_depacketizer(NULL),
                                                         m_frame_jitter(RTP_VIDEO_CLOCK)
{

}

VideoStream::~VideoStream()
{
    destroy_depacketizer(m_depacketizer);
}

bool
VideoStream::set_video_media(RTP_MEDIA_TYPE media, int payload_type, int port)
{
    m_depacketizer = create_depacketizer(media);
    if (!m_depacketizer)
    {
        RP_LOG_E("unable to create depacketizer!");
        return false;
    }

    if (!set_media(media, payload_type, port))
    {
        RP_LOG_E("unable to set_media for media: %d!", media);

        delete m_depacketizer;
        m_depacketizer = NULL;
    }

    return !!m_depacketizer;
}

void
VideoStream::on_rtp_packet(IRtpPacket * packet, bool discontinuous)
{
    int status;
    MediaFrame frame;

    if (!m_depacketizer || !packet)
    {
        RP_LOG_E("error on_rtp_packet, invalid param or depacketizer.");
        return;
    }

#if 0
    if (discontinuous)
    {
        /* resync */
        m_depacketizer->set_sync_point();
        RP_LOG_I("stream discontinuity, video is resynchronized!");
    }
#endif
    /* parse the packet */
    status = m_depacketizer->parse((unsigned char *)packet->payload(),
                                   packet->payload_size(),
                                   packet->marker(),
                                   packet->timestamp(),
                                   packet->payload_type(),
                                   &frame);

    if (DEPACKETIZER_FRAME == status)
    {
        ///< RP_LOG_D("video frame jitter(new frame): %dms", m_frame_jitter.jitter_ms(packet->GetTimeStamp()));
        send_frame(&frame);
    }
}

void
VideoStream::video_info(float & framerate, int & bitrate_kbps, int & losses, float & lossrate)
{
    framerate    = m_framerate.GetBitRate();
    bitrate_kbps = m_bitrate.GetBitRate() / 1000;
    losses       = m_lossrate.GetLossCount();
    lossrate     = m_lossrate.GetLossRate(); ///< [0, 1]
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
