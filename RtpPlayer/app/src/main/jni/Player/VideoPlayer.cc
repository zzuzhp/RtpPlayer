#include "VideoPlayer.h"
#include "Streams/VideoStream.h"
#include "Decoders/VideoDecoder.h"
#include "Renders/VideoOut.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

VideoPlayer::VideoPlayer() : BasePlayer("VideoPlayer")
{

}

VideoPlayer::~VideoPlayer()
{
    tear();
}

bool
VideoPlayer::build (AVSync * sync, ANativeWindow * window, RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    {
        CProThreadMutexGuard mon(m_lock);

        VIDEO_CODEC_TYPE codec;
        if (media == RTP_MEDIA_VIDEO_AVC)
        {
            codec = CODEC_TYPE_AVC;
        }
        else if (media == RTP_MEDIA_VIDEO_VP8)
        {
            codec = CODEC_TYPE_VP8;
        }
        else
        {
            RP_LOG_E("play_video failed for invalid media: %d!", media);
            return false;
        }

        m_renderer = VideoOut::create_instance(window);
        if (!m_renderer)
        {
            RP_LOG_E("video out create failed!");
            goto fail;
        }

        m_renderer->set_source(sync);
        if (!m_renderer->start())
        {
            RP_LOG_E("video out start failed!");
            goto fail;
        }

        m_decoder = VideoDecoder::create_instance();
        if (!m_decoder || !((VideoDecoder *)m_decoder)->build(codec, this))
        {
            RP_LOG_E("failed to create decoder for codec: %d.", codec);
            goto fail;
        }

        m_stream = VideoStream::create_instance(this);
        if (!m_stream)
        {
            RP_LOG_E("stream %d receiver create failed.", rtp_payload_type);
            goto fail;
        }

        if (!((VideoStream *)m_stream)->set_video_media(media, rtp_payload_type, port))
        {
            RP_LOG_E("stream %d set_video_media failed.", rtp_payload_type);
            goto fail;
        }

        m_width  = 0;
        m_height = 0;

        m_av_sync = sync;
    }

    return true;

fail:

    tear();

    return false;
}

void
VideoPlayer::tear()
{
    clear();

    m_width  = 0;
    m_height = 0;
}

bool
VideoPlayer::video_info(int   & width,
                        int   & height,
                        float & framerate,
                        int   & bitrate_kbps,
                        int   & losses,
                        float & lossrate)
{
    CProThreadMutexGuard mon(m_lock);

    VideoStream * stream = (VideoStream *)m_stream;
    if (!stream)
    {
        RP_LOG_E("can not get video info, stream not exist.");
        return false;
    }

    stream->video_info(framerate, bitrate_kbps, losses, lossrate);

    width  = m_width;
    height = m_height;

    return true;
}

void
VideoPlayer::on_decoded_image(AVFrame * frame)
{
    if (frame->get_width() <= 0 || frame->get_height() <= 0)
    {
        RP_LOG_E("invalid frame from video decoder callback, width: %d, height: %d.", frame->get_width(), frame->get_height());
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_width == 0 || m_height == 0)
        {
            m_start_ts = frame->get_pts();
            RP_LOG_I("received first video pts: %u, audio buffering: %dms", frame->get_pts(), m_av_sync->audio_latency_ms());
        }

        m_width  = frame->get_width();
        m_height = frame->get_height();

        m_av_sync->push_video(frame);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
