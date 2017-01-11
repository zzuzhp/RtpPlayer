#ifndef ___AVDECODER_H___
#define ___AVDECODER_H___

#include "Common/AVModule.h"
#include "Common/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVDecoder : public AVModule
{
public:

    AVDecoder(const char * name, int evt_mask = (int)EVENT_ALL) : AVModule(AV_MODULE_TRANSFORMER, name, evt_mask){}

    virtual void decode(MediaFrame * frame) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVDECODER_H___
