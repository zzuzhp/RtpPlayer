#include "Streams/AudioStream.h"
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AudioStream *
AudioStream::create_instance(RtpStreamObserver * observer)
{
    if (!observer)
    {
        return NULL;
    }

    return new(std::nothrow) AudioStream(observer);
}

AudioStream::AudioStream(RtpStreamObserver * observer) : RtpStream(observer, "audio stream")
{

}

AudioStream::~AudioStream()
{

}

bool
AudioStream::set_audio_media(RTP_MEDIA_TYPE media, int payload_type, int port)
{
    if (!set_media(media, payload_type, port))
    {
        RP_LOG_E("unable to set_media for media: %d!", media);
        return false;
    }

    return true;
}

void
AudioStream::audio_info(int & bitrate_kbps, int & losses, float & lossrate)
{
    bitrate_kbps = m_bitrate.GetBitRate() / 1000;
    losses       = m_lossrate.GetLossCount();
    lossrate     = m_lossrate.GetLossRate(); ///< [0, 1]
}

void
AudioStream::on_rtp_packet(IRtpPacket * packet, bool discontinuous)
{
    MediaFrame frame;

    if (!packet)
    {
        RP_LOG_E("error on_rtp_packet, invalid param.");
        return;
    }

    frame.type              = media_type();
    frame.data              = (unsigned char *)packet->payload();
    frame.len               = packet->payload_size();
    frame.pts               = packet->timestamp();
    frame.flags             = 0;
    frame.rtp_payload_type  = rtp_payload_type();
    frame.packets           = 1;

    send_frame(&frame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
