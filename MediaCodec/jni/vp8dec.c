#include "vp8dec.h"
#include "vpx/vp8dx.h"
#include "vpx_mem/vpx_mem.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int
vp8_dec_create(vp8_dec_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    *ctx = (vp8_dec_context *)vpx_malloc(sizeof(vp8_dec_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(vp8_dec_context));
    return 0;
}

int
vp8_dec_init(video_dec_context * ctx, int max_threads)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    vp8_dec_context * vp8_ctx = (vp8_dec_context *)ctx->priv_ctx;

    vpx_codec_dec_cfg_t cfg = { max_threads, 0, 0 };

    int dec_flags = VPX_CODEC_USE_FRAME_THREADING;

    if (vpx_codec_dec_init(&vp8_ctx->decoder, vpx_codec_vp8_dx(), &cfg, dec_flags))
    {
        video_dec_log(ctx, "failed to initialize decoder: %s", vpx_codec_error(&vp8_ctx->decoder));
        return -1;
    }

    return 0;
}

int
vp8_dec_decode(video_dec_context    * ctx,
               const unsigned char  * frame,
               int                    len,
               long long              pts,
               long long              opaque)
{
    vpx_image_t      * image = NULL;
    vpx_codec_iter_t   iter  = NULL;

    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    vp8_dec_context * vp8_ctx = (vp8_dec_context *)ctx->priv_ctx;

    if (frame == 0 && len == 0)
    {
        /* flush the decoder in frame parallel decode */
        if (vpx_codec_decode(&vp8_ctx->decoder, NULL, 0, NULL, 0))
        {
            video_dec_log(ctx, "failed to flush decoder: %s", vpx_codec_error(&vp8_ctx->decoder));
            return -1;
        }
    }
    else
    {
        if (vpx_codec_decode(&vp8_ctx->decoder, frame, len, NULL, 0))
        {
            video_dec_log(ctx, "error decode one frame: %s", vpx_codec_error_detail(&vp8_ctx->decoder));
            return -1;
        }
    }

    if ((image = vpx_codec_get_frame(&vp8_ctx->decoder, &iter)))
    {
        if (image->fmt != VPX_IMG_FMT_I420)
        {
            /* error */
            video_dec_log(ctx, "we do not handle images that is not i420 format: %d.", image->fmt);
            return -1;
        }
        else
        {
            video_dec_send_image(ctx,
                                 (const unsigned char **)image->planes,
                                 image->d_w,
                                 image->d_h,
                                 image->stride,
                                 pts,
                                 opaque);
        }
    }

    return 0;
}

void
vp8_dec_release_image(video_dec_context * ctx, video_surface * image)
{
    video_dec_free_image(image);
}

void
vp8_dec_destroy(video_dec_context * ctx)
{
    if (ctx && ctx->priv_ctx)
    {
        vp8_dec_context * vp8_ctx = (vp8_dec_context *)ctx->priv_ctx;

        if (vpx_codec_destroy(&vp8_ctx->decoder))
        {
            video_dec_log(ctx, "failed to destroy decoder: %s", vpx_codec_error_detail(&vp8_ctx->decoder));
        }

        free(vp8_ctx);
    }
}
