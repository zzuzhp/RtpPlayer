#ifndef ___AVSYNC_H___
#define ___AVSYNC_H___

#include "Renders/AVOut.h"
#include "Common/AVFrame.h"
#include "Common/AVTimer.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVSync : public AVSource
{
public:

    AVSync();

    ~AVSync();

    void set_clock(AVClock * clock);

    void push_video(AVFrame * frame);

    void push_audio(AVFrame * frame);

    int video_frame(AVFrame ** frame);

    int audio_frame(AVFrame ** frame);

    void release_frame(AVFrame * frame);

    int video_latency_ms();

    int audio_latency_ms();

private:

    std::list<AVFrame *>    m_video_frames;

    std::list<AVFrame *>    m_audio_frames;

    uint32_t                m_last_video_ts;

    int                     m_video_duration;

    AVClock               * m_clock;    ///< external timer

    AVTimer               * m_timer;    ///< internal timer

    bool                    m_rendering_started;

    CProThreadMutex         m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif  ///< ___LIPSYNC_H___
