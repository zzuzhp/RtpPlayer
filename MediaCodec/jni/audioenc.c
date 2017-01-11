#include "audioenc.h"
#include "opusenc.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

void
audio_enc_log(audio_enc_context * ctx, const char * fmt, ...)
{
    if (ctx->cb_log)
    {
        static char msg[MAX_LOG_MESSAGE_BUF_SIZE];

        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msg, MAX_LOG_MESSAGE_BUF_SIZE - 1, fmt, ap);
        va_end(ap);

        ctx->cb_log(ctx, msg, ctx->cb_log_arg);
    }
}

MEDIACODEC_API
audio_enc_context *
create_audio_encoder(AUDIO_CODEC_TYPE codec)
{
    audio_enc_context * ctx = 0;

    if (codec != CODEC_TYPE_OPUS)
    {
        return 0;
    }

    ctx = (audio_enc_context *)malloc(sizeof(audio_enc_context));
    memset(ctx, 0, sizeof(audio_enc_context));

    if (codec == CODEC_TYPE_OPUS)
    {
        opus_enc_context * opus_ctx = 0;
        if (0 != opus_enc_create(&opus_ctx))
        {
            return 0;
        }

        opus_ctx->ctx = ctx;
        ctx->priv_ctx = opus_ctx;

        ctx->funcs.ef_init           = opus_enc_init;
        ctx->funcs.ef_update_bitrate = opus_enc_update_bitrate;
        ctx->funcs.ef_encode         = opus_enc_encode;
        ctx->funcs.ef_destroy        = opus_enc_destroy;
    }

    return ctx;
}

MEDIACODEC_API
int
audio_enc_init(audio_enc_context * ctx, audio_enc_config * cfg)
{
    return ctx ? ctx->funcs.ef_init(ctx, cfg) : -1;
}

MEDIACODEC_API
int 
audio_enc_set_cb_log(audio_enc_context * ctx, audio_enc_log_cb cb_log, void * arg)
{
    if (ctx)
    {
        ctx->cb_log     = cb_log;
        ctx->cb_log_arg = arg;
    }
}

MEDIACODEC_API
int 
audio_enc_set_cb_frame(audio_enc_context * ctx, audio_enc_frame_cb cb_frame, void * arg)
{
    if (ctx)
    {
        ctx->cb_out = cb_frame;
        ctx->cb_arg = arg;
    }
}

MEDIACODEC_API
int 
audio_enc_set_bitrate(audio_enc_context * ctx, int bitrate_in_kbps)
{
    return ctx ? ctx->funcs.ef_update_bitrate(ctx, bitrate_in_kbps) : -1;
}

MEDIACODEC_API
int 
audio_enc_encode(audio_enc_context * ctx, const void * data, int data_len_in_bytes, long long ntp_in_ms, long long opaque)
{
    return ctx ? ctx->funcs.ef_encode(ctx, data, data_len_in_bytes, ntp_in_ms, opaque) : -1;
}

MEDIACODEC_API
void 
audio_enc_destroy(audio_enc_context * ctx)
{
    if (ctx)
    {
        ctx->funcs.ef_destroy(ctx);
        free(ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
