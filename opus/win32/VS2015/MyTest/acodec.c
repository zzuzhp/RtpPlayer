#include "acodec.h"
#include "opusenc.h"

#include <stdlib.h>

typedef int(*pfn_audio_enc_init)(void * handle, audio_enc_config * cfg);
typedef int(*pfn_audio_enc_set_cb_log)(void * handle, cb_enc_log cb_log, void * arg);
typedef int(*pfn_audio_enc_set_cb_frame)(void * handle, cb_enc_output cb_frame, void * arg);
typedef int(*pfn_audio_enc_set_bitrate)(void * handle, int new_bitrate_kbit);
typedef int(*pfn_audio_enc_encode)(void * handle, const void * data, int data_len_in_bytes, long long ntp_in_ms, long long opaque);
typedef void(*pfn_audio_enc_destroy)(void * handle);

typedef struct audio_enc_ctx
{
    AUDIO_CODEC_TYPE type;
    void * enc_ctx;

    pfn_audio_enc_init          init;
    pfn_audio_enc_set_cb_log    set_cb_log;
    pfn_audio_enc_set_cb_frame  set_cb_frame;
    pfn_audio_enc_set_bitrate   set_bitrate;
    pfn_audio_enc_encode        encode;
    pfn_audio_enc_destroy       destroy;
} audio_enc_ctx;

void * 
create_audio_encoder(AUDIO_CODEC_TYPE codec)
{
    audio_enc_ctx * ctx = 0;

    if (codec == CODEC_TYPE_OPUS)
    {
        opus_enc_context * opus_ctx = 0;
        if (0 != opus_enc_create(&opus_ctx))
        {
            return 0;
        }

        ctx = (audio_enc_ctx *)malloc(sizeof(audio_enc_ctx));
        ctx->type       = CODEC_TYPE_OPUS;
        ctx->enc_ctx    = opus_ctx;

        ctx->init           = (pfn_audio_enc_init)opus_enc_init;
        ctx->set_cb_log     = (pfn_audio_enc_set_cb_log)opus_enc_set_cb_log;
        ctx->set_cb_frame   = (pfn_audio_enc_set_cb_frame)opus_enc_set_cb_frame;
        ctx->set_bitrate    = (pfn_audio_enc_set_bitrate)opus_enc_set_bitrate;
        ctx->encode         = (pfn_audio_enc_encode)opus_enc_encode;
        ctx->destroy        = (pfn_audio_enc_destroy)opus_enc_destroy;
    }

    return ctx;
}

int 
audio_enc_init(void * handle, audio_enc_config * cfg)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->init(ctx->enc_ctx, cfg);
    }

    return -1;
}

int 
audio_enc_get_caps(AUDIO_CODEC_TYPE codec)
{
    if (codec == CODEC_TYPE_OPUS)
    {
        return opus_enc_get_caps();
    }

    return -1;
}

int 
audio_enc_set_cb_log(void * handle, cb_enc_log cb_log, void * arg)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_cb_log(ctx->enc_ctx, cb_log, arg);
    }

    return -1;
}

int 
audio_enc_set_cb_frame(void * handle, cb_enc_output cb_frame, void * arg)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_cb_frame(ctx->enc_ctx, cb_frame, arg);
    }

    return -1;
}

int 
audio_enc_set_bitrate(void * handle, int bitrate_in_kbps)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_bitrate(ctx->enc_ctx, bitrate_in_kbps);
    }

    return -1;
}

int 
audio_enc_encode(void * handle, const void * data, int data_len_in_bytes, long long ntp_in_ms)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->encode(ctx->enc_ctx, data, data_len_in_bytes, ntp_in_ms, 0);
    }

    return -1;
}

void 
audio_enc_destroy(void * handle)
{
    audio_enc_ctx * ctx = (audio_enc_ctx *)handle;
    if (ctx)
    {
        ctx->destroy(ctx->enc_ctx);
        free(ctx);
    }
}
