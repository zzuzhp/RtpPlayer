#ifndef ___AVOUT_H___
#define ___AVOUT_H___

#include "Common/AVModule.h"
#include "Common/AVFrame.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVSource
{
public:

    virtual int video_frame(AVFrame ** frame) = 0;

    virtual int audio_frame(AVFrame ** frame) = 0;

    virtual void release_frame(AVFrame * frame) = 0;
};

class AVOut : public AVModule
{
public:

    AVOut(const char * name, int evt_mask = (int)EVENT_ALL) : AVModule(AV_MODULE_RENDERER, name, evt_mask){}

    virtual int set_source(AVSource * source) = 0;

    virtual bool start() = 0;

    virtual void stop() = 0;

    virtual const double render_time() const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVOUT_H___
