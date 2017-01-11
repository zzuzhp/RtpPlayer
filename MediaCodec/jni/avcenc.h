#ifndef ___AVCENC_H___
#define ___AVCENC_H___

#include "common.h"
#include "videoenc.h"
#include "x264.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* defines an AVC encoder context */
typedef struct avc_enc_context
{
    video_enc_context   * ctx;

    x264_t              * x264;
    x264_param_t          param;
    x264_picture_t        picture;

    /* statistics */
    uint64_t              cx_time;
    uint32_t              frames;
} avc_enc_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create a avc encoder
 * returns 0 for success, negative for failure
 */
int avc_enc_create(avc_enc_context ** ctx);

/*!\brief initialize a avc encoder
 * returns 0 for success, negative for failure
 */
int avc_enc_init(video_enc_context * ctx, video_enc_config * cfg);

/*!\brief change the framerate
 * returns 0 for success, negative for failure
 */
int avc_enc_update_framerate(video_enc_context * ctx, int frame_rate);

/*!\brief change the bitrate
 * returns 0 for success, negative for failure
 */
int avc_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps);

/*!\brief release the frame
 */
void avc_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame);

/*!\brief encode an frame
 * returns 0 for success, negative for failure
 */
int avc_enc_encode(video_enc_context * ctx, video_surface * surface, int flags);

/*!\brief destroy the encoder
 */
void avc_enc_destroy(video_enc_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif ///< ___AVCENC_H___
