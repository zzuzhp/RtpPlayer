#include "vcodec.h"
#include "vp8enc.h"

#include <stdlib.h>

typedef int(*pfn_video_enc_init)(void * handle, video_enc_config * cfg);
typedef int(*pfn_video_enc_get_caps)(void * handle);
typedef int(*pfn_video_enc_set_cb_log)(void * handle, cb_enc_log cb_log, void * arg);
typedef int(*pfn_video_enc_set_cb_output)(void * handle, cb_enc_output cb_frame, void * arg);
typedef int(*pfn_video_enc_set_rates)(void * handle, int new_bitrate_kbit, int frame_rate);
typedef int(*pfn_video_enc_encode)(void * handle, video_surface * surface, int flags);
typedef void(*pfn_video_enc_destroy)(void * handle);

typedef struct video_enc_ctx
{
    VIDEO_CODEC_TYPE type;
    void * enc_ctx;

    pfn_video_enc_init init;
    pfn_video_enc_get_caps get_caps;
    pfn_video_enc_set_cb_log set_cb_log;
    pfn_video_enc_set_cb_output set_cb_output;
    pfn_video_enc_set_rates set_rates;
    pfn_video_enc_encode encode;
    pfn_video_enc_destroy destroy;
} video_enc_ctx;

void * 
create_video_encoder(VIDEO_CODEC_TYPE type)
{
    video_enc_ctx * ctx = 0;

    if (type == CODEC_TYPE_VP8)
    {
        vp8_enc_context * vp8_ctx = 0;
        if (0 != vp8_enc_create(&vp8_ctx))
        {
            return 0;
        }

        ctx = (video_enc_ctx *)malloc(sizeof(video_enc_ctx));
        ctx->type = CODEC_TYPE_VP8;
        ctx->enc_ctx = vp8_ctx;
         
        ctx->init           = (pfn_video_enc_init)vp8_enc_init;
        ctx->get_caps       = (pfn_video_enc_get_caps)vp8_enc_get_caps;
        ctx->set_cb_log     = (pfn_video_enc_set_cb_log)vp8_enc_set_cb_log;
        ctx->set_cb_output  = (pfn_video_enc_set_cb_output)vp8_enc_set_cb_output;
        ctx->set_rates      = (pfn_video_enc_set_rates)vp8_enc_set_rates;
        ctx->encode         = (pfn_video_enc_encode)vp8_enc_encode;
        ctx->destroy        = (pfn_video_enc_destroy)vp8_enc_destroy;
    }

    return ctx;
}

int 
video_enc_init(void * handle, video_enc_config * cfg)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->init(ctx->enc_ctx, cfg);
    }

    return -1;
}

int 
video_enc_get_caps(void * handle)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->get_caps(ctx->enc_ctx);
    }

    return 0;
}

int 
video_enc_set_cb_log(void * handle, cb_enc_log cb_log, void * arg)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_cb_log(ctx->enc_ctx, cb_log, arg);
    }

    return -1;
}

int 
video_enc_set_cb_output(void * handle, cb_enc_output cb_frame, void * arg)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_cb_output(ctx->enc_ctx, cb_frame, arg);
    }

    return -1;
}

int 
video_enc_set_rates(void * handle, int new_bitrate_kbit, int frame_rate)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->set_rates(ctx->enc_ctx, new_bitrate_kbit, frame_rate);
    }

    return -1;
}

int 
video_enc_encode(void * handle, video_surface * surface, int flags)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        return ctx->encode(ctx->enc_ctx, surface, flags);
    }

    return -1;
}

void 
video_enc_destroy(void * handle)
{
    video_enc_ctx * ctx = (video_enc_ctx *)handle;
    if (ctx)
    {
        ctx->destroy(ctx->enc_ctx);
        free(ctx);
    }
}
