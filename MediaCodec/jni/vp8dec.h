#ifndef ___VP8DEC_H___
#define ___VP8DEC_H___

#include "videodec.h"
#include "vpx/vpx_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* defines a vp8 decoder context */
typedef struct vp8_dec_context
{
    video_dec_context   * ctx;
    vpx_codec_ctx_t       decoder;
} vp8_dec_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create a vp8 decoder
 * returns 0 for success, negative for failure
 */
int vp8_dec_create(vp8_dec_context ** ctx);

/*!\brief initialize a vp8 decoder
 * returns 0 for success, negative for failure
 */
int vp8_dec_init(video_dec_context * ctx, int max_threads);

/*!\brief decode an frame
 * returns 0 for success, negative for failure
 */
int vp8_dec_decode(video_dec_context    * ctx,
                   const unsigned char  * frame,
                   int                    len,
                   long long              pts,
                   long long              opaque);

/*!\brief release the image
 */
void vp8_dec_release_image(video_dec_context * ctx, video_surface * image);

/*!\brief destroy the encoder
 */
void vp8_dec_destroy(video_dec_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif ///< ___VP8DEC_H___
