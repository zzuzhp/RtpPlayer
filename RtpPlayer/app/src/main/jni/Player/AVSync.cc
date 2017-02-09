#include "AVSync.h"
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AVSync *
AVSync::create_instance()
{
    return new(std::nothrow) AVSync();
}

AVSync::AVSync() : AVModule(AV_MODULE_TRANSFORMER, "avsync", (int)EVENT_ALL),
                   m_max_latency(1000),
                   m_last_video_ts(0),
                   m_video_duration(0),
                   m_state(AVSYNC_BUFFERING),
                   m_jitter(RTP_VIDEO_CLOCK),
                   m_buffering(false)
{
    m_avtimer = new AVTimer();
    m_avtimer->start_clock(0);

    m_clock = m_avtimer;
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

    delete m_avtimer;
}

void
AVSync::set_clock(AVClock * clock)
{
    CProThreadMutexGuard mon(m_lock);

    /* use external timer */
    if (m_avtimer)
    {
        delete m_avtimer;
        m_avtimer = 0;
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
    {
        CProThreadMutexGuard mon(m_lock);

        frame->add_ref();

        if (m_video_frames.size() == 0)
        {
            m_buffering = true;
            RP_LOG_E("start buffering ...");
        }

        m_video_frames.push_back(frame);

        m_jitter.push(frame->get_pts());
    }

    int latency = video_latency_ms();

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_buffering && latency >= 300)
        {
            m_buffering = false;
            RP_LOG_E("stop buffering.");
        }
    }
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
    while (audio_latency_ms() > m_max_latency)
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
    CProThreadMutexGuard mon(m_lock);

    if (m_video_frames.empty() || m_buffering)
    {
        *frame = NULL;
        return 1;
    }

    AVFrame * video_frame = m_video_frames.front();

    if (m_video_duration == 0)
    {
        m_last_video_ts  = video_frame->get_pts();
        m_video_duration = 40;

        m_clock->stop_clock();
        m_clock->start_clock(0);
    }

    int      time_now = m_clock->clock_ms();
    uint32_t duration = (video_frame->get_pts() - m_last_video_ts) * 1000 / RTP_VIDEO_CLOCK;

    if (time_now >= m_video_duration + duration)
    {
        m_video_frames.pop_front();
        *frame = video_frame;

        m_last_video_ts   = video_frame->get_pts();
        m_video_duration += duration; ///< always one frame late
        return 0;
    }

    *frame = NULL;
    return 1;
}

int
AVSync::audio_frame(AVFrame ** frame)
{
    CProThreadMutexGuard mon(m_lock);

    if (m_audio_frames.empty() || m_buffering)
    {
        *frame = NULL;
        return 1;
    }

    *frame = m_audio_frames.front();
    m_audio_frames.pop_front();

    return 0;
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
