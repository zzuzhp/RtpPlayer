#ifndef ___VIDEODEC_H___
#define ___VIDEODEC_H___

#include "videodefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEDIACODEC_EXPORT
#define MEDIACODEC_EXPORT  __attribute__((visibility("default")))
#endif

#ifndef MEDIACODEC_IMPORT
#define MEDIACODEC_IMPORT
#endif

#ifndef MEDIACODEC_API
#ifdef MEDIACODEC_EXPORTS
    #define MEDIACODEC_API MEDIACODEC_EXPORT
#else
    #define MEDIACODEC_API MEDIACODEC_IMPORT
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct video_dec_context;

/* decoder functions */
typedef struct video_dec_funcs
{
    int  (* df_init)          (struct video_dec_context * ctx, int max_threads);
    int  (* df_decode)        (struct video_dec_context * ctx, const unsigned char * frame,  int len, long long pts, long long opaque);
    void (* df_release_image) (struct video_dec_context * ctx, video_surface * image);
    void (* df_destroy)       (struct video_dec_context * ctx);
} video_dec_funcs;

/* defines a decoder context */
typedef struct video_dec_context
{
    void                * priv_ctx;

    void                * cb_arg;
    video_dec_image_cb    cb_out;

    void                * cb_log_arg;
    video_dec_log_cb      cb_log;

    video_dec_funcs       funcs;
} video_dec_context;

void video_dec_log(video_dec_context * ctx, const char * fmt, ...);

void video_dec_send_image(video_dec_context   * ctx,
                          const unsigned char * data[3],      ///< const unsigned char **
                          const int             width,
                          const int             height,
                          const int             linesize[3],  ///< const int **
                          long long             pts,
                          long long             opaque);

void video_dec_free_image(video_surface * image);

////////////////////////////////////////////////////////////////////////////////////////////////////
////    APIs

/*!\brief: create a video decoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
video_dec_context *
create_video_decoder(VIDEO_CODEC_TYPE codec);

/*!\brief: initialize a decoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
video_dec_init(video_dec_context * ctx, int max_threads);

/*!\brief: set a callback for the decoder logs
 */
MEDIACODEC_API
void
video_dec_set_cb_log(video_dec_context * ctx, video_dec_log_cb cb_log, void * arg);

/*!\brief: set a callback for the decoded frames
 */
MEDIACODEC_API
void
video_dec_set_cb_output(video_dec_context * ctx, video_dec_image_cb cb_image, void * arg);

/*!\brief: decode one frame
 * @flags: reserved
 * note: set @frame and @len to 0 to flush the decoder
 */
MEDIACODEC_API
int
video_dec_decode(video_dec_context   * ctx,
                 const unsigned char * frame,
                 int                   len,
                 int                   flags,
                 long long             pts,
                 long long             opaque);

/*!\brief: release the image
 */
MEDIACODEC_API
void
video_dec_release_image(video_dec_context * ctx, video_surface * image);

/*!\brief: destroy the decoder
 */
MEDIACODEC_API
void
video_dec_destroy(video_dec_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif ///< ___VIDEODEC_H___
