#ifndef ___VIDEOENC_H___
#define ___VIDEOENC_H___

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

struct video_enc_context;

/* encoder functions */
typedef struct video_enc_funcs
{
    int  (* ef_init)             (struct video_enc_context * ctx, video_enc_config * cfg);
    int  (* ef_update_framerate) (struct video_enc_context * ctx, int frame_rate);
    int  (* ef_update_bitrate)   (struct video_enc_context * ctx, int bitrate_in_kbps);
    int  (* ef_encode)           (struct video_enc_context * ctx, video_surface * surface, int flags);
    void (* ef_release_frame)    (struct video_enc_context * ctx, video_enc_frame_bs * frame);
    void (* ef_destroy)          (struct video_enc_context * ctx);
} video_enc_funcs;

/* defines an encoder context */
typedef struct video_enc_context
{
    void                * priv_ctx;

    video_enc_config      config;

    void                * cb_arg;
    video_enc_frame_cb    cb_out;

    void                * cb_log_arg;
    video_enc_log_cb      cb_log;

    video_enc_funcs       funcs;
} video_enc_context;

void video_enc_log(video_enc_context * ctx, const char * fmt, ...);

video_enc_frame_bs * video_enc_get_free_frame(video_enc_context * ctx, int num_units);

void video_enc_free_frame(video_enc_frame_bs * frame);

////////////////////////////////////////////////////////////////////////////////////////////////////
////    APIs

/*!\brief: create an encoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
video_enc_context *
create_video_encoder(VIDEO_CODEC_TYPE codec);

/*!\brief: initialize an encoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
video_enc_init(video_enc_context * ctx, video_enc_config * cfg);

/*!\brief: set a callback for the encoder logs
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
void
video_enc_set_cb_log(video_enc_context * ctx, video_enc_log_cb cb_log, void * arg);

/*!\brief: set a callback for the encoded frames
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
void
video_enc_set_cb_output(video_enc_context * ctx, video_enc_frame_cb cb_frame, void * arg);

/*!\brief: change the framerate
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
video_enc_update_framerate(video_enc_context * ctx, int frame_rate);

/*!\brief: change the bitrates
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
video_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps);

/*!\brief: release the frame
 */
MEDIACODEC_API
void
video_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame);

/*!\brief: encode an frame
 * set @surface to 0 to indicate a flush of the encoder.
 *     @flags: control flags, see VID_ENC_CTRLFLAG_xxx
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
video_enc_encode(video_enc_context * ctx, video_surface * surface, int flags);

/*!\brief: destroy the encoder
 */
MEDIACODEC_API
void
video_enc_destroy(video_enc_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}
#endif

#endif ///< ___VIDEOENC_H___
