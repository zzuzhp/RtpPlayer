#ifndef ___AUDIOENC_H___
#define ___AUDIOENC_H___

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

struct audio_enc_context;

/* encoder functions */
typedef struct audio_enc_funcs
{
    int  (* ef_init)             (struct audio_enc_context * ctx, audio_enc_config * cfg);
    int  (* ef_update_bitrate)   (struct audio_enc_context * ctx, int bitrate_in_kbps);
    int  (* ef_encode)           (struct audio_enc_context * ctx, const void * data, int data_len_in_bytes, long long ntp_in_ms, long long opaque);
    void (* ef_destroy)          (struct audio_enc_context * ctx);
} audio_enc_funcs;

/* defines an encoder context */
typedef struct audio_enc_context
{
    void                * priv_ctx;

    audio_enc_config      config;

    void                * cb_arg;
    audio_enc_frame_cb    cb_out;

    void                * cb_log_arg;
    audio_enc_log_cb      cb_log;

    audio_enc_funcs       funcs;
} audio_enc_context;

void audio_enc_log(audio_enc_context * ctx, const char * fmt, ...);

////////////////////////////////////////////////////////////////////////////////////////////////////
////    APIs

/*!\brief create an audio encoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
audio_enc_context *
create_audio_encoder(AUDIO_CODEC_TYPE codec);

/*!\brief initialize an audio encoder
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_enc_init(audio_enc_context * ctx, audio_enc_config * cfg);

/*!\brief set a callback for the encoder logs
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_enc_set_cb_log(audio_enc_context * ctx, audio_enc_log_cb cb_log, void * arg);

/*!\brief set a callback for the encoded frames
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_enc_set_cb_frame(audio_enc_context * ctx, audio_enc_frame_cb cb_frame, void * arg);

/*!\brief change the bitrates or(and) framerate
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_enc_set_bitrate(audio_enc_context * ctx, int bitrate_in_kbps);

/*!\brief encode an frame
 * @data_len_in_bytes should be a multiply of channels*bytes_per_sample
 * @ntp_in_ms is ntp of the last sample
 * returns 0 for success, negative for failure
 */
MEDIACODEC_API
int
audio_enc_encode(audio_enc_context * ctx, const void * data, int data_len_in_bytes, long long ntp_in_ms, long long opaque);

/*!\brief destroy the encoder
 */
MEDIACODEC_API
void
audio_enc_destroy(audio_enc_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}
#endif

#endif ///< ___AUDIOENC_H___
