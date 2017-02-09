#ifndef ___AVOUTBASE_H___
#define ___AVOUTBASE_H___

#include "Renders/AVOut.h"
#include "pro_thread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVOutBase : public AVOut,
                  public CProThreadBase
{
public:

    AVOutBase(bool video, const char * name, int evt_mask = (int)EVENT_ALL);

    virtual ~AVOutBase(){}

    int set_source(AVSource * source);

    bool start();

    void stop();

    const int render_time() const
    {
        return m_render_time;
    }

private:

    virtual void on_frame(AVFrame * frame) = 0;

    virtual void on_start_render() = 0;

    virtual void on_end_render() = 0;

    void Svc();

protected:

    int               m_render_time;

private:

    volatile bool     m_exit;

    bool              m_video;

    AVSource        * m_source;

    int (AVSource::*m_get_frame)(AVFrame ** frame);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVOUTBASE_H___
