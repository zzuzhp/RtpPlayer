#ifndef ___COMMON_H___
#define ___COMMON_H___

#include "Common/Log.h"

#include <cstdint>
#include <cstdlib>
#include <cstdio>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* defines the rtp media type */
typedef enum RTP_MEDIA_TYPE
{
    RTP_MEDIA_INVALID       = 0x0000,
    RTP_MEDIA_AUDIO_AAC     = 0x0001,
    RTP_MEDIA_AUDIO_OPUS    = 0x0002,
    RTP_MEDIA_VIDEO_AVC     = 0x0100,
    RTP_MEDIA_VIDEO_VP8     = 0x0200
} RTP_MEDIA_TYPE;

/* defines a media frame */
typedef struct MediaFrame
{
    RTP_MEDIA_TYPE   type;              ///< media type
    uint8_t        * data;              ///< frame buffer
    int              len;               ///< frame length
    int64_t          pts;               ///< pts
    int              flags;             ///< FRAME_IS_xxx
    int              rtp_payload_type;  ///< rtp payload type
    int              packets;           ///< num of rtp packets used to generate this frame
} MediaFrame;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define RTP_VIDEO_CLOCK        90000    ///< RTP video use 90khz clock for its timestamp

#define RTPPLAYER_MAX(a, b) ((a) > (b) ? (a) : (b))
#define RTPPLAYER_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ISVIDEO(x) ((x) == RTP_MEDIA_VIDEO_AVC || (x) == RTP_MEDIA_VIDEO_VP8)
#define ISAUDIO(x) ((x) == RTP_MEDIA_AUDIO_AAC || (x) == RTP_MEDIA_AUDIO_OPUS)

#define FRAME_IS_INCOMPLETE    0x0001   ///< frame is incomplete
#define FRAME_IS_SYNC_POINT    0x0002   ///< key frame
#define FRAME_IS_DISCONTINUITY 0x0004   ///< frame(s) lost before this frame

#define MAX_ES_FRAME_SIZE      1000000  ///< assumed max frame length in bytes: 1M bytes

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___COMMON_H___
