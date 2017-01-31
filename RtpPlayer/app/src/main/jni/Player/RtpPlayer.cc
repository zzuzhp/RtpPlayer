#include "Player/RtpPlayer.h"
#include "Renders/AudioOut.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpPlayer::RtpPlayer() : m_video_player(NULL),
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
    CProThreadMutexGuard mon(m_lock);

    m_av_sync = AVSync::create_instance();
    if (!m_av_sync)
    {
        RP_LOG_E("AVSync create failed!");
        return false;
    }

    return true;
}

void
RtpPlayer::tear()
{
    AVSync * sync = NULL;

    RP_FOOTPRINT
    remove_video();
    RP_FOOTPRINT
    remove_audio();
    RP_FOOTPRINT

    {
        CProThreadMutexGuard mon(m_lock);

        sync      = m_av_sync;
        m_av_sync = NULL;
    }

    if (sync)
    {
        sync->release();
    }
}

bool
RtpPlayer::play_video(ANativeWindow * window, RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    CProThreadMutexGuard mon(m_lock);

    if (m_video_player)
    {
        RP_LOG_E("only one video is allowed for one player!");
        return false;
    }

    m_video_player = new (std::nothrow)VideoPlayer;
    if (!m_video_player)
    {
        RP_LOG_E("VideoPlayer create failed!");
        return false;
    }

    if (!m_video_player->build(m_av_sync, window, media, rtp_payload_type, port))
    {
        delete m_video_player;
        m_video_player = NULL;

        return false;
    }

    return true;
}

bool
RtpPlayer::play_audio(RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    CProThreadMutexGuard mon(m_lock);

    if (m_audio_player)
    {
        RP_LOG_E("only one audio is allowed for one player!");
        return false;
    }

    m_audio_player = new (std::nothrow)AudioPlayer;
    if (!m_audio_player)
    {
        RP_LOG_E("AudioPlayer alloc failed!");
        return false;
    }

    if (!m_audio_player->build(m_av_sync, media, rtp_payload_type, port))
    {
        delete m_audio_player;
        m_audio_player = NULL;

        return false;
    }

    m_av_sync->set_clock((AudioOut *)m_audio_player->renderer());

    return true;
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

    if (!m_video_player->video_info(width,
                                    height,
                                    framerate,
                                    bitrate_kbps,
                                    losses,
                                    lossrate))
    {
        return false;
    }

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

    if (!m_audio_player->audio_info(sample_rate,
                                    channels,
                                    bits_per_sample,
                                    bitrate_kbps,
                                    losses,
                                    lossrate))
    {
        return false;
    }

    return true;
}

void
RtpPlayer::remove_video()
{
    VideoPlayer * video_player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        video_player    = m_video_player;
        m_video_player  = NULL;
    }

    if (video_player)
    {
        video_player->tear();
        video_player->release();
    }
}

void
RtpPlayer::remove_audio()
{
    AudioPlayer * audio_player = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        audio_player    = m_audio_player;
        m_audio_player  = NULL;
    }

    if (audio_player)
    {
        audio_player->tear();
        audio_player->release();
    }
}

int
RtpPlayer::play_time_ms()
{
    CProThreadMutexGuard mon(m_lock);

    /* we prefer audio */
    if (m_audio_player)
    {
        return m_audio_player->play_time_ms();
    }

    if (m_video_player)
    {
        return m_video_player->play_time_ms();
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
