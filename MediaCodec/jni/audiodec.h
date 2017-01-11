#ifndef ___AUDIODEC_H___
#define ___AUDIODEC_H___

#include "audiodefs.h"

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

struct audio_dec_context;

/* encoder functions */
typedef struct audio_dec_funcs
{
    int  (* df_init)    (struct audio_dec_context * ctx);
    int  (* df_decode)  (struct audio_dec_context * ctx, const char * packet, int packet_len, int flags, long long pts, long long opaque);
    void (* df_destroy) (struct audio_dec_context * ctx);
} audio_dec_funcs;

/* defines an decoder context */
typedef struct audio_dec_context
{
    void                * priv_ctx;

    void                * cb_arg;
    audio_dec_pcm_cb      cb_out;

    void                * cb_log_arg;
    audio_dec_log_cb      cb_log;

    audio_dec_funcs       funcs;
} audio_dec_context;

void audio_dec_log(struct audio_dec_context * ctx, const char * fmt, ...);

////////////////////////////////////////////////////////////////////////////////////////////////////
////    APIs

/*!\brief: create an audio decoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
audio_dec_context *
create_audio_decoder(AUDIO_CODEC_TYPE codec);

/*!\brief: initialize a decoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_dec_init(audio_dec_context * ctx);

/*!\brief: set a callback for the decoder logs
 */
MEDIACODEC_API
void
audio_dec_set_cb_log(audio_dec_context * ctx, audio_dec_log_cb cb_log, void * arg);

/*!\brief: set a callback for the decoded pcm
 */
MEDIACODEC_API
void
audio_dec_set_cb_output(audio_dec_context * ctx, audio_dec_pcm_cb cb_out, void * arg);

/*!\brief: decode one frame
 * note: set @frame and @len to 0 to flush the decoder
 */
MEDIACODEC_API
int
audio_dec_decode(audio_dec_context   * ctx,
                 const char          * packet,
                 int                   packet_len,
                 int                   flags,
                 long long             pts,
                 long long             opaque);

/*!\brief: destroy the decoder
 */
MEDIACODEC_API
void
audio_dec_destroy(audio_dec_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif
