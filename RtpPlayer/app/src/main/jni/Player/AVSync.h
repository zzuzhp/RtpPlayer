#ifndef ___AVSYNC_H___
#define ___AVSYNC_H___

#include "Renders/AVOut.h"
#include "Common/AVModule.h"
#include "Common/AVFrame.h"
#include "Common/AVTimer.h"
#include "Common/AVJitter.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////////////////////////
////    balance between latency and smoothness

/*                            x
 *         buffering ---------- moderate
 *                x \          /
 *                   \        /
 *                    \      /
 *                    playing
 *                       |
 *                       |
 *                       |
 *                  accelerating
 *
 */
enum AVSYNC_STATE
{
    AVSYNC_BUFFERING    = 0,   ///< buffering
    AVSYNC_PLAYING      = 1,   ///< 1x playing
    AVSYNC_ACCELERATING = 2,   ///< fast forward
    AVSYNC_MODERATE     = 3    ///< pacing down
};

/*
 *  buffer size should incorporate jitter
 */
class AVSync : public AVSource,
               public AVModule
{
public:

    static AVSync * create_instance();

    void set_clock(AVClock * clock);

    void push_video(AVFrame * frame);

    void push_audio(AVFrame * frame);

    int video_frame(AVFrame ** frame);

    int audio_frame(AVFrame ** frame);

    void release_frame(AVFrame * frame);

    int video_latency_ms();

    int audio_latency_ms();

    AVSYNC_STATE state() const { return m_state; }

private:

    AVSync();

    ~AVSync();

private:

    std::list<AVFrame *>    m_video_frames;

    std::list<AVFrame *>    m_audio_frames;

    const uint32_t          m_max_latency;

    uint32_t                m_last_video_ts;

    AVClock               * m_clock;      ///< external timer

    AVTimer               * m_avtimer;    ///< internal timer

    int64_t                 m_last_time;

    AVSYNC_STATE            m_state;

    AVJitter                m_jitter;

    CProThreadMutex         m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif  ///< ___LIPSYNC_H___
