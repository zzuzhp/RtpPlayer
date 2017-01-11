#ifndef ___COMMON_H___
#define ___COMMON_H___

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* control flags */
#define VID_ENC_CTRLFLAG_KEY                    0x0001  ///< force a key frame
#define VID_ENC_CTRLFLAG_DEINTERLACE            0x0002  ///< perform deinterlace before encode

/* data unit flag */
#define VID_ENC_UNIT_DROPPABLE                  0x0001  ///< this unit is droppable

#define COLLECT_STATISTICS                      0       ///< log bitstream statistics as fps & bitrate for every frame

#define MAX_LOG_MESSAGE_BUF_SIZE                4096    ///< log message max length

#define MEDIACODEC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define MEDIACODEC_MIN(a, b) ((a) < (b) ? (a) : (b))

#define FUNC_CHECK_NONZERO_PARAM_VOID(x) \
    if (!(x)) \
    { \
        return; \
    }

#define FUNC_CHECK_NONZERO_PARAM(x, r) \
    if (!(x)) \
    { \
        return (r); \
    }

static int
detect_number_cores()
{
    /* We fall back on assuming a single core in case of errors. */
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___COMMON_H___
