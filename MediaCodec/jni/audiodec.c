#include "audiodec.h"
#include "opusdec.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

void
audio_dec_log(audio_dec_context * ctx, const char * fmt, ...)
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
audio_dec_context *
create_audio_decoder(AUDIO_CODEC_TYPE codec)
{
    audio_dec_context * ctx = 0;

    if (codec != CODEC_TYPE_OPUS)
    {
        return 0;
    }

    ctx = (audio_dec_context *)malloc(sizeof(audio_dec_context));
    memset(ctx, 0, sizeof(audio_dec_context));

    if (codec == CODEC_TYPE_OPUS)
    {
        opus_dec_context * opus_ctx = 0;
        if (0 != opus_dec_create(&opus_ctx))
        {
            return 0;
        }

        opus_ctx->ctx = ctx;
        ctx->priv_ctx = opus_ctx;

        ctx->funcs.df_init      = opus_dec_init;
        ctx->funcs.df_decode    = opus_dec_decode;
        ctx->funcs.df_destroy   = opus_dec_destroy;
    }

    return ctx;
}

MEDIACODEC_API
int
audio_dec_init(audio_dec_context * ctx)
{
    return ctx ? ctx->funcs.df_init(ctx) : -1;
}

MEDIACODEC_API
void
audio_dec_set_cb_log(audio_dec_context * ctx, audio_dec_log_cb cb_log, void * arg)
{
    if (ctx)
    {
        ctx->cb_log = cb_log;
        ctx->cb_log_arg = arg;
    }
}

MEDIACODEC_API
void
audio_dec_set_cb_output(audio_dec_context * ctx, audio_dec_pcm_cb cb_out, void * arg)
{
    if (ctx)
    {
        ctx->cb_out = cb_out;
        ctx->cb_arg = arg;
    }
}

MEDIACODEC_API
int
audio_dec_decode(audio_dec_context  * ctx,
                 const char         * packet,
                 int                  packet_len,
                 int                  flags,
                 long long            pts,
                 long long            opaque)
{
    return ctx ? ctx->funcs.df_decode(ctx, packet, packet_len, flags, pts, opaque) : -1;
}

MEDIACODEC_API
void
audio_dec_destroy(audio_dec_context * ctx)
{
    if (ctx)
    {
        ctx->funcs.df_destroy(ctx);
        free(ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
