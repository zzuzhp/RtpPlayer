#include "Depacketizers/RtpDepacketizer.h"
#include "Depacketizers/RtpDepacketizerH264.h"
#include "Depacketizers/RtpDepacketizerVp8.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpDepacketizer *
create_depacketizer(RTP_MEDIA_TYPE media)
{
    RtpDepacketizer * depacketizer = NULL;

    if (media == RTP_MEDIA_VIDEO_AVC)
    {
        depacketizer = new (std::nothrow)RtpDepacketizerH264();
    }
    else if (media == RTP_MEDIA_VIDEO_VP8)
    {
        depacketizer = new (std::nothrow)RtpDepacketizerVp8();
    }
    else
    {
        RP_LOG_E("unable to create depacketizer, unsupported media: %d!", media);
        return NULL;
    }

    return depacketizer;
}

void
destroy_depacketizer(RtpDepacketizer * depacketizer)
{
    if (depacketizer)
    {
        delete depacketizer;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
