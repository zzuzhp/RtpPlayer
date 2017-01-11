#include "AVOutBase.h"
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AVOutBase::AVOutBase(bool video, const char * name, int evt_mask) : AVOut(name, evt_mask),
                                                                    m_exit(false),
                                                                    m_render_time(0),
                                                                    m_source(NULL)
{
    m_get_frame = video ? &AVSource::video_frame : &AVSource::audio_frame;
}

int
AVOutBase::set_source(AVSource * source)
{
    m_source = source;
}

bool
AVOutBase::start()
{
    if (!m_source)
    {
        RP_LOG_E("renders need a source to start.");
        return false;
    }

    Spawn(false);
    return true;
}

void
AVOutBase::stop()
{
    m_exit = true;
    Wait();
}

void
AVOutBase::Svc()
{
    on_start_render();

    while (!m_exit)
    {
        AVFrame * frame = NULL;

        int idle_time = (m_source->*m_get_frame)(&frame);
        if (!frame)
        {
            usleep(idle_time);
            continue;
        }

        on_frame(frame);

        m_source->release_frame(frame);
    }

    on_end_render();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
