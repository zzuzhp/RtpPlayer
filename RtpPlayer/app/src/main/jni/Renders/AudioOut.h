#ifndef ___AUDIOOUT_H___
#define ___AUDIOOUT_H___

#include "Common/AVClock.h"
#include "Renders/AVOutBase.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define SAMPLE_BLOCK_SIZE_MS    20  ///< 20ms

class AudioOut : public AVClock,
                 public AVOutBase
{
public:

    static AudioOut * create_instance()
    {
        return new (std::nothrow)AudioOut();
    }

    bool start() { return AVOutBase::start(); }

    void start(int offset_ms){}

    void stop(){ AVOutBase::stop(); }

    void resume(){}

    int time_ms()
    {
        /* this is not accurate if there are lost packets: consider using timestamp */
        return (int)(render_time() * 1000);
    }

private:

    AudioOut();

    ~AudioOut();

    bool init(int sample_rate, int channels);

    void clear();

    bool create_engine();

    bool open_device(int sample_rate, int channels, int bits_per_sample);

    void buffer_available();

    void on_frame(AVFrame * frame);

    void on_start_render(){}

    void on_end_render(){}

    static void on_buffer_finished (SLAndroidSimpleBufferQueueItf bq, void * context);

private:

    /* engine interfaces */
    SLObjectItf                     m_engineObject;
    SLEngineItf                     m_engineEngine;

    /* output mix interfaces */
    SLObjectItf                     m_outputMixObject;

    /* buffer queue player interfaces */
    SLObjectItf                     m_bqPlayerObject;
    SLPlayItf                       m_bqPlayerPlay;
    SLAndroidSimpleBufferQueueItf   m_bqPlayerBufferQueue;

    /* buffer indexes */
    int                             m_currentOutputIndex;

    /* current buffer half (0/1) */
    int                             m_currentOutputBuffer;

    /* buffers */
    short                         * m_outputBuffer[2];
    int                             m_outBufSamples;

    int                             m_channels;
    int                             m_sample_rate;

    CProThreadMutexCondition        m_cond;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AUDIOOUT_H___
