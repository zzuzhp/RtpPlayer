#ifndef ___AVCDEC_H___
#define ___AVCDEC_H___

#include "videodec.h"
#include "libavcodec/avcodec.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* defines an avc decoder context */
typedef struct avc_dec_context
{
    video_dec_context   * ctx;

    AVCodec             * avcodec;
    AVCodecContext      * avcodec_ctx;
    AVFrame             * avframe;
} avc_dec_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create an avc decoder
 * returns 0 for success, negative for failure
 */
int avc_dec_create(avc_dec_context ** ctx);

/*!\brief initialize an avc decoder
 * returns 0 for success, negative for failure
 */
int avc_dec_init(video_dec_context * ctx, int max_threads);

/*!\brief decode an frame
 * returns 0 for success, negative for failure
 */
int avc_dec_decode(video_dec_context    * ctx,
                   const unsigned char  * frame,
                   int                    len,
                   long long              pts,
                   long long              opaque);

/*!\brief release the image
 */
void avc_dec_release_image(video_dec_context * ctx, video_surface * image);

/*!\brief destroy the encoder
 */
void avc_dec_destroy(video_dec_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif ///< ___AVCDEC_H___
