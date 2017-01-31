#include "AVSync.h"
#include <new>

#define MAX_LATENCY_MS      2000
#define DISABLE_LIPSYNC     1

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AVSync *
AVSync::create_instance()
{
    return new(std::nothrow) AVSync();
}

AVSync::AVSync() : AVModule(AV_MODULE_TRANSFORMER, "avsync", (int)EVENT_ALL),
                   m_last_video_ts(0),
                   m_video_duration(0)
{
    m_timer = new AVTimer();
    m_clock = m_timer;
}

AVSync::~AVSync()
{
    while (!m_video_frames.empty())
    {
        AVFrame * frame = m_video_frames.front();
        m_video_frames.pop_front();

        frame->release();
    }

    while (!m_audio_frames.empty())
    {
        AVFrame * frame = m_audio_frames.front();
        m_audio_frames.pop_front();

        frame->release();
    }

    delete m_timer;
}

void
AVSync::set_clock(AVClock * clock)
{
    CProThreadMutexGuard mon(m_lock);

    /* use external timer */
    if (m_timer)
    {
        delete m_timer;
        m_timer = 0;
    }

    if (!clock)
    {
        RP_LOG_I("setting clock to 0, disabling av sync ?");
    }

    m_clock = clock;
}

void
AVSync::push_video(AVFrame * frame)
{
    CProThreadMutexGuard mon(m_lock);

    frame->add_ref();

    m_video_frames.push_back(frame);
}

void
AVSync::push_audio(AVFrame * frame)
{
    {
        CProThreadMutexGuard mon(m_lock);

        frame->add_ref();

        m_audio_frames.push_back(frame);
    }

#if 0
    while (audio_latency_ms() > MAX_LATENCY_MS)
    {
        RP_LOG_W("av sync is buffering too much audio(%dms) and one will be erased.", m_audio_latency);

        AVFrame * pcm = NULL;

        {
            CProThreadMutexGuard mon(m_lock);

            pcm = m_audio_frames.front();
            m_audio_frames.pop_front();
        }

        release_frame(pcm);
    }
#endif
}

int
AVSync::video_frame(AVFrame ** frame)
{
    if (!frame)
    {
        return 1;
    }

    *frame = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_video_frames.empty())
        {
            return 1;
        }

        AVFrame  * video_frame  = m_video_frames.front();
        bool       first_frame  = m_video_duration == 0 && m_last_video_ts == 0;
        uint32_t   duration     = first_frame ? 0 : (video_frame->get_pts() - m_last_video_ts) / RTP_VIDEO_CLOCK;
        bool       render_frame = true;

#if !DISABLE_LIPSYNC
        if (m_clock)
        {
            /* sync video to audio */
            int time_now = m_clock->time_ms();

            if (!first_frame && (time_now + 50) < (m_video_duration + duration))
            {
                render_frame = false;
            }
        }
#endif

        if (render_frame)
        {
            m_video_frames.pop_front();
            *frame = video_frame;

            m_last_video_ts   = video_frame->get_pts();
            m_video_duration += duration;

            return 0;
        }
    }

    return 1;
}

int
AVSync::audio_frame(AVFrame ** frame)
{
    if (!frame)
    {
        return 1;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_audio_frames.empty())
        {
            *frame = NULL;
            return 1;
        }

        *frame = m_audio_frames.front();
        m_audio_frames.pop_front();

        return 0;
    }

    return 1;
}

int
AVSync::video_latency_ms()
{
    CProThreadMutexGuard mon(m_lock);

    if (m_video_frames.empty())
    {
        return 0;
    }

    return (m_video_frames.back()->get_pts() - m_video_frames.front()->get_pts()) / RTP_VIDEO_CLOCK * 1000;
}

int
AVSync::audio_latency_ms()
{
    CProThreadMutexGuard mon(m_lock);

    if (m_audio_frames.empty())
    {
        return 0;
    }

    return (m_audio_frames.back()->get_pts() - m_audio_frames.front()->get_pts()) * 1000 / m_audio_frames.front()->get_sample_rate();
}

void
AVSync::release_frame(AVFrame * frame)
{
    if (frame)
    {
        frame->release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
