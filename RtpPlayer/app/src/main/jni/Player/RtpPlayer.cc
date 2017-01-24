#include "Player/RtpPlayer.h"
#include "Streams/VideoStream.h"

#include <new>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpPlayer::RtpPlayer() : m_session(NULL),
                         m_video_player(NULL),
                         m_audio_player(NULL),
                         m_av_sync(NULL)
{

}

RtpPlayer::~RtpPlayer()
{
    tear();
}

bool
RtpPlayer::build()
{
    {
        CProThreadMutexGuard mon(m_lock);

        m_session = new RtpSession(this);
        if (!m_session)
        {
            RP_LOG_E("RtpSession create failed!");
            return false;
        }

        m_av_sync = new AVSync();
        if (!m_av_sync)
        {
            RP_LOG_E("AVSync create failed!");
            return false;
        }
    }

    return true;

fail:

    tear();

    return false;
}

void
RtpPlayer::tear()
{
    RtpSession * session = NULL;
    AVSync     * sync    = NULL;

    RP_FOOTPRINT
    remove_video();
    RP_FOOTPRINT
    remove_audio();
    RP_FOOTPRINT

    {
        CProThreadMutexGuard mon(m_lock);

        session   = m_session;
        m_session = NULL;

        sync      = m_av_sync;
        m_av_sync = NULL;
    }

    if (sync)
    {
        delete sync;
    }

    if (session)
    {
        delete session;
    }
}

bool
RtpPlayer::play_video(ANativeWindow * window, RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    VideoPlayer * player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_video_player)
        {
            RP_LOG_E("only one video is allowed for one player, old video:%d(%d), new video:%d(%d)",
                      m_video_player->stream->media_type(), m_video_player->stream->rtp_payload_type(), media, rtp_payload_type);
            return false;
        }

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

        player = new (std::nothrow)VideoPlayer;
        if (!player)
        {
            RP_LOG_E("VideoPlayer alloc failed!");
            return false;
        }

        player->vout = VideoOut::create_instance(window);
        if (!player->vout)
        {
            RP_LOG_E("video out create failed!");
            goto fail;
        }

        player->vout->set_source(m_av_sync);
        if (!player->vout->start())
        {
            RP_LOG_E("video out start failed!");
            goto fail;
        }

        player->stream = m_session->create_stream(media, rtp_payload_type, port);
        if (!player->stream)
        {
            RP_LOG_E("create_stream failed for media: %d!", media);
            goto fail;
        }

        player->decoder = VideoDecoder::create_instance();
        if (!player->decoder || !player->decoder->build(codec, this))
        {
            RP_LOG_E("failed to create decoder for codec: %d.", codec);
            goto fail;
        }

        player->width  = 0;
        player->height = 0;

        m_video_player = player;

        return true;
    }

fail:

    remove_player(player);

    return false;
}

bool
RtpPlayer::play_audio(RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    AudioPlayer * player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_audio_player)
        {
            RP_LOG_E("only one audio is allowed for one player, old audio:%d(%d), new audio:%d(%d)",
                      m_audio_player->stream->media_type(), m_audio_player->stream->rtp_payload_type(), media, rtp_payload_type);
            return false;
        }

        AUDIO_CODEC_TYPE codec;
        if (media == RTP_MEDIA_AUDIO_AAC)
        {
            codec = CODEC_TYPE_AAC;
        }
        else if (media == RTP_MEDIA_AUDIO_OPUS)
        {
            codec = CODEC_TYPE_OPUS;
        }
        else
        {
            RP_LOG_E("play_audio failed for invalid media: %d!", media);
            return false;
        }

        player = new (std::nothrow)AudioPlayer;
        if (!player)
        {
            RP_LOG_E("AudioPlayer alloc failed!");
            return false;
        }

        player->aout = AudioOut::create_instance();
        if (!player->aout)
        {
            RP_LOG_E("audio out create failed!");
            goto fail;
        }

        player->aout->set_source(m_av_sync);
        if (!player->aout->start())
        {
            RP_LOG_E("audio out start failed!");
            goto fail;
        }

        player->stream = m_session->create_stream(media, rtp_payload_type, port);
        if (!player->stream)
        {
            RP_LOG_E("create_stream failed for media: %d!", media);
            goto fail;
        }

        player->decoder = AudioDecoder::create_instance();
        if (!player->decoder || !player->decoder->build(codec, this))
        {
            RP_LOG_E("failed to create decoder for codec: %d.", codec);
            goto fail;
        }

        m_av_sync->set_clock((AudioOut *)player->aout);

        player->sample_rate     = 0;
        player->channels        = 0;
        player->bits_per_sample = 0;

        m_audio_player = player;

        return true;
    }

fail:

    remove_player(player);

    return false;
}

void
RtpPlayer::remove_video()
{
    VideoPlayer * player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        player = m_video_player;
        m_video_player = NULL;
    }

    if (player)
    {
        remove_player(player);
    }
}

void
RtpPlayer::remove_audio()
{
    AudioPlayer * player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        player = m_audio_player;
        m_audio_player = NULL;
    }

    if (player)
    {
        remove_player(player);
    }
}

bool
RtpPlayer::video_info(int   & width,
                      int   & height,
                      float & framerate,
                      int   & bitrate_kbps,
                      int   & losses,
                      float & lossrate)
{
    CProThreadMutexGuard mon(m_lock);

    if (!m_video_player)
    {
        RP_LOG_E("cannot get video info, stream not found.");
        return false;
    }

    StreamInfo info;
    if (!m_session->stream_status(m_video_player->stream, info))
    {
        return false;
    }

    width          = m_video_player->width;
    height         = m_video_player->height;
    framerate      = info.framerate;
    bitrate_kbps   = info.bitrate_kbps;
    losses         = info.losses;
    lossrate       = info.loss_rate;

    return true;
}

bool
RtpPlayer::audio_info(int   & sample_rate,
                      int   & channels,
                      int   & bits_per_sample,
                      int   & bitrate_kbps,
                      int   & losses,
                      float & lossrate)
{
    CProThreadMutexGuard mon(m_lock);

    if (!m_audio_player)
    {
        RP_LOG_E("cannot get audio info, stream not found.");
        return false;
    }

    StreamInfo info;
    if (!m_session->stream_status(m_audio_player->stream, info))
    {
        return false;
    }

    sample_rate     = m_audio_player->sample_rate;
    channels        = m_audio_player->channels;
    bits_per_sample = m_audio_player->bits_per_sample;
    bitrate_kbps    = info.bitrate_kbps;
    losses          = info.losses;
    lossrate        = info.loss_rate;

    return true;
}

void
RtpPlayer::remove_player(VideoPlayer * player)
{
    if (player)
    {
        m_session->destroy_stream(player->stream);

        player->decoder->tear();
        player->decoder->release();

        player->vout->stop();
        player->vout->release();

        delete player;
    }
}

void
RtpPlayer::remove_player(AudioPlayer * player)
{
    if (player)
    {
        m_session->destroy_stream(player->stream);

        player->decoder->tear();
        player->decoder->release();

        player->aout->stop();
        player->aout->release();

        delete player;
    }
}

void
RtpPlayer::on_session_frame(RtpSession * session, RtpStream * stream, MediaFrame * frame)
{
    AVDecoder * decoder = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (ISVIDEO(stream->media_type()))
        {
            if (m_video_player && m_video_player->stream == stream)
            {
                m_video_player->decoder->add_ref();
                decoder = m_video_player->decoder;
            }
        }
        else if (ISAUDIO(stream->media_type()))
        {
            if (m_audio_player && m_audio_player->stream == stream)
            {
                m_audio_player->decoder->add_ref();
                decoder = m_audio_player->decoder;
            }
        }
    }

    if (decoder)
    {
        decoder->decode(frame);
        decoder->release();
    }
}

void
RtpPlayer::on_decoded_image (VideoDecoder * decoder, AVFrame * frame)
{
    if (!decoder || !frame)
    {
        RP_LOG_E("invalid param from video decoder callback.");
        return;
    }

    if (frame->get_width() <= 0 || frame->get_height() <= 0)
    {
        RP_LOG_E("invalid frame from video decoder callback, width: %d, height: %d.", frame->get_width(), frame->get_height());
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (!m_av_sync || !m_video_player || m_video_player->decoder != decoder)
        {
            return;
        }

        if (m_video_player->width == 0 || m_video_player->height == 0)
        {
            m_video_player->start_ts = frame->get_pts();
            RP_LOG_I("received first video pts: %u, audio buffering: %dms", frame->get_pts(), m_av_sync->audio_latency_ms());
        }

        m_video_player->width  = frame->get_width();
        m_video_player->height = frame->get_height();

        m_av_sync->push_video(frame);
    }
}

void
RtpPlayer::on_decoded_pcm (AudioDecoder * decoder, AVFrame * frame)
{
    if (!decoder || !frame)
    {
        RP_LOG_E("invalid param from audio decoder callback.");
        return;
    }

    if (frame->get_sample_rate() <= 0 || frame->get_channels() <= 0 || frame->get_bits_per_sample() <= 0)
    {
        RP_LOG_E("invalid frame from audio decoder callback, ar: %d, ac: %d, bits: %d.",
                  frame->get_sample_rate(), frame->get_channels(), frame->get_bits_per_sample());
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (!m_av_sync || !m_audio_player || m_audio_player->decoder != decoder)
        {
            return;
        }

        if (m_audio_player->sample_rate == 0 || m_audio_player->channels == 0 || m_audio_player->bits_per_sample == 0)
        {
            m_audio_player->start_ts = frame->get_pts();
            RP_LOG_I("received first audio pts: %u, (video buffering: %dms)", frame->get_pts(), m_av_sync->video_latency_ms());
        }

        m_audio_player->sample_rate     = frame->get_sample_rate();
        m_audio_player->channels        = frame->get_channels();
        m_audio_player->bits_per_sample = frame->get_bits_per_sample();

        m_av_sync->push_audio(frame);
    }
}

int
RtpPlayer::play_time_ms()
{
    CProThreadMutexGuard mon(m_lock);

    if (m_audio_player && m_audio_player->aout)
    {
        return (int)(m_audio_player->aout->render_time() * 1000);
    }

    if (m_video_player && m_video_player->vout)
    {
        return (int)(m_video_player->vout->render_time() * 1000);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
