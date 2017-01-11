#include "videodec.h"
#include "vp8dec.h"
#include "avcdec.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

void
video_dec_log(video_dec_context * ctx, const char * fmt, ...)
{
    if (ctx && ctx->cb_log)
    {
        static char msg[MAX_LOG_MESSAGE_BUF_SIZE];

        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msg, MAX_LOG_MESSAGE_BUF_SIZE - 1, fmt, ap);
        va_end(ap);

        ctx->cb_log(ctx, msg, ctx->cb_log_arg);
    }
}

void
video_dec_send_image(video_dec_context   * ctx,
                     const unsigned char * data[3],
                     const int             width,
                     const int             height,
                     const int             linesize[3],
                     long long             pts,
                     long long             opaque)
{
    int plane, line;
    if (!ctx->cb_out)
    {
        return;
    }

    video_surface * surface = (video_surface *)malloc(sizeof(video_surface));
    memset(surface, 0, sizeof(video_surface));

    surface->color  = IMG_FMT_I420;
    surface->pts    = pts;
    surface->opaque = opaque;

    for (plane = 0; plane < 3; ++plane)
    {
        const int plane_stride = linesize[plane];
        const int plane_width  = plane ? ((width + 1) >> 1) : width;
        const int plane_height = plane ? ((height + 1) >> 1) : height;

        surface->width[plane]  = plane_width;
        surface->height[plane] = plane_height;
        surface->pitch[plane]  = plane_width;

        surface->planes[plane] = (uint8_t *)malloc(plane_width * plane_height);
        if (!surface->planes[plane])
        {
            video_dec_log(ctx, "video_dec_send_image alloc plane %d[%d - %d] failed.", plane, plane_width, plane_height);
            video_dec_free_image(surface);
            return;
        }

        for (line = 0; line < plane_height; ++line)
        {
            memcpy(surface->planes[plane] + line * plane_width, data[plane] + line * plane_stride, plane_width);
        }
    }

    ctx->cb_out(ctx, surface, ctx->cb_arg);
}

void
video_dec_free_image(video_surface * image)
{
    if (image)
    {
        free(image->planes[0]);
        free(image->planes[1]);
        free(image->planes[2]);

        free(image);
    }
}

MEDIACODEC_API
video_dec_context *
create_video_decoder(VIDEO_CODEC_TYPE codec)
{
    video_dec_context * ctx = 0;

    if (codec != CODEC_TYPE_VP8 && codec != CODEC_TYPE_AVC)
    {
        return 0;
    }

    ctx = (video_dec_context *)malloc(sizeof(video_dec_context));
    memset(ctx, 0, sizeof(video_dec_context));

    if (codec == CODEC_TYPE_VP8)
    {
        vp8_dec_context * vp8_ctx = 0;
        if (0 != vp8_dec_create(&vp8_ctx))
        {
            free(ctx);
            return 0;
        }

        vp8_ctx->ctx  = ctx;
        ctx->priv_ctx = vp8_ctx;

        ctx->funcs.df_init           = vp8_dec_init;
        ctx->funcs.df_decode         = vp8_dec_decode;
        ctx->funcs.df_release_image  = vp8_dec_release_image;
        ctx->funcs.df_destroy        = vp8_dec_destroy;
    }
    else if (codec == CODEC_TYPE_AVC)
    {
        avc_dec_context * avc_ctx = 0;
        if (0 != avc_dec_create(&avc_ctx))
        {
            free(ctx);
            return 0;
        }

        avc_ctx->ctx  = ctx;
        ctx->priv_ctx = avc_ctx;

        ctx->funcs.df_init           = avc_dec_init;
        ctx->funcs.df_decode         = avc_dec_decode;
        ctx->funcs.df_release_image  = avc_dec_release_image;
        ctx->funcs.df_destroy        = avc_dec_destroy;
    }

    return ctx;
}

MEDIACODEC_API
int
video_dec_init(video_dec_context * ctx, int max_threads)
{
    return ctx ? ctx->funcs.df_init(ctx, max_threads) : -1;
}

MEDIACODEC_API
void
video_dec_set_cb_log(video_dec_context * ctx, video_dec_log_cb cb_log, void * arg)
{
    if (ctx)
    {
        ctx->cb_log     = cb_log;
        ctx->cb_log_arg = arg;
    }
}

MEDIACODEC_API
void
video_dec_set_cb_output(video_dec_context * ctx, video_dec_image_cb cb_image, void * arg)
{
    if (ctx)
    {
        ctx->cb_out = cb_image;
        ctx->cb_arg = arg;
    }
}

MEDIACODEC_API
int
video_dec_decode(video_dec_context   * ctx,
                 const unsigned char * frame,
                 int                   len,
                 int                   flags,
                 long long             pts,
                 long long             opaque)
{
    return ctx ? ctx->funcs.df_decode(ctx, frame, len, pts, opaque) : -1;
}

MEDIACODEC_API
void
video_dec_release_image(video_dec_context * ctx, video_surface * image)
{
    if (ctx)
    {
        ctx->funcs.df_release_image(ctx, image);
    }
}

MEDIACODEC_API
void
video_dec_destroy(video_dec_context * ctx)
{
    if (ctx)
    {
        ctx->funcs.df_destroy(ctx);
        free(ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
