#ifndef ___RTPDEPACKETIZER_H___
#define ___RTPDEPACKETIZER_H___

#include "Common/common.h"
#include "Common/AVModule.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* return values */
#define DEPACKETIZER_NEED_MORE 0    ///< need more packets to generate one frame
#define DEPACKETIZER_ERROR     1    ///< an error occured
#define DEPACKETIZER_FRAME     2    ///< a new frame is available

class RtpDepacketizer : public AVModule
{
public:

    RtpDepacketizer(const char * name, int evt_masks = (int)EVENT_ALL) : AVModule(AV_MODULE_TRANSFORMER, name, evt_masks){}

    virtual ~RtpDepacketizer(){}

    /* \! brief: parse an rtp packet, returns true if a frame is available
     *   return: DEPACKETIZER_xxx
     * note: if a new frame is available, you should copy it immediately.
     */
    virtual int  parse(const unsigned char * packet,
                       int                   packet_len,
                       bool                  marker,
                       unsigned int          pts,
                       int                   payload_type,
                       MediaFrame          * frame) = 0;

    /* \! brief: a key frame is requested for the next frame, all others before it should be dropped.
     */
    virtual void set_sync_point() = 0;
};

RtpDepacketizer *
create_depacketizer(RTP_MEDIA_TYPE media);

void
destroy_depacketizer(RtpDepacketizer * depacketizer);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPDEPACKETIZER_H___
