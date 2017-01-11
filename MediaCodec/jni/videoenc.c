#include "videoenc.h"
#include "vp8enc.h"
#include "avcenc.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

void
video_enc_log(video_enc_context * ctx, const char * fmt, ...)
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

video_enc_frame_bs *
video_enc_get_free_frame(video_enc_context * ctx, int num_units)
{
    /* mem space for all the data units */
    int units_size = sizeof(video_enc_data_unit_bs) * num_units;

    /* for simplicity, reserve a pretty large space for the bitstream */
    int frame_size = sizeof(video_enc_frame_bs) + /* video_enc_frame_bs  */
                     units_size + /* video_enc_data_unit_bs */
                     ctx->config.width * ctx->config.height * 3/2; /* bitstream */

    char * frame_buf = (char *)malloc(frame_size);
    if (!frame_buf)
    {
        return 0;
    }

    video_enc_frame_bs * frame = (video_enc_frame_bs *)frame_buf;
    memset(frame, 0, sizeof(video_enc_frame_bs));

    frame->units = (video_enc_data_unit_bs *) (frame_buf + sizeof(video_enc_frame_bs));
    frame->data  = (unsigned char *) ((char *)frame->units + units_size);
    frame->codec = CODEC_TYPE_NUL;

    return frame;
}

void
video_enc_free_frame(video_enc_frame_bs * frame)
{
    free((char *)frame);
}

MEDIACODEC_API
video_enc_context *
create_video_encoder(VIDEO_CODEC_TYPE codec)
{
    video_enc_context * ctx = 0;

    if (codec != CODEC_TYPE_VP8 && codec != CODEC_TYPE_AVC)
    {
        return 0;
    }

    ctx = (video_enc_context *)malloc(sizeof(video_enc_context));
    memset(ctx, 0, sizeof(video_enc_context));

    if (codec == CODEC_TYPE_VP8)
    {
        vp8_enc_context * vp8_ctx = 0;
        if (0 != vp8_enc_create(&vp8_ctx))
        {
            free(ctx);
            return 0;
        }

        vp8_ctx->ctx  = ctx;
        ctx->priv_ctx = vp8_ctx;
         
        ctx->funcs.ef_init               = vp8_enc_init;
        ctx->funcs.ef_update_framerate   = vp8_enc_update_framerate;
        ctx->funcs.ef_update_bitrate     = vp8_enc_update_bitrate;
        ctx->funcs.ef_encode             = vp8_enc_encode;
        ctx->funcs.ef_release_frame      = vp8_enc_release_frame;
        ctx->funcs.ef_destroy            = vp8_enc_destroy;
    }
    else if (codec == CODEC_TYPE_AVC)
    {
        avc_enc_context * avc_ctx = 0;
        if (0 != avc_enc_create(&avc_ctx))
        {
            free(ctx);
            return 0;
        }

        avc_ctx->ctx  = ctx;
        ctx->priv_ctx = avc_ctx;

        ctx->funcs.ef_init               = avc_enc_init;
        ctx->funcs.ef_update_framerate   = avc_enc_update_framerate;
        ctx->funcs.ef_update_bitrate     = avc_enc_update_bitrate;
        ctx->funcs.ef_encode             = avc_enc_encode;
        ctx->funcs.ef_release_frame      = avc_enc_release_frame;
        ctx->funcs.ef_destroy            = avc_enc_destroy;
    }

    return ctx;
}

MEDIACODEC_API
int
video_enc_init(video_enc_context * ctx, video_enc_config * config)
{
    return ctx ? ctx->funcs.ef_init(ctx, config) : -1;
}

MEDIACODEC_API
void
video_enc_set_cb_log(video_enc_context * ctx, video_enc_log_cb cb_log, void * arg)
{
    if (ctx)
    {
        ctx->cb_log     = cb_log;
        ctx->cb_log_arg = arg;
    }
}

MEDIACODEC_API
void
video_enc_set_cb_output(video_enc_context * ctx, video_enc_frame_cb cb_frame, void * arg)
{
    if (ctx)
    {
        ctx->cb_out = cb_frame;
        ctx->cb_arg = arg;
    }
}

MEDIACODEC_API
int
video_enc_update_framerate(video_enc_context * ctx, int frame_rate)
{
    return ctx ? ctx->funcs.ef_update_framerate(ctx, frame_rate) : -1;
}

MEDIACODEC_API
int
video_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps)
{
    return ctx ? ctx->funcs.ef_update_bitrate(ctx, bitrate_in_kbps) : -1;
}

MEDIACODEC_API
void
video_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame)
{
    if (ctx)
    {
        ctx->funcs.ef_release_frame(ctx, frame);
    }
}

MEDIACODEC_API
int
video_enc_encode(video_enc_context * ctx, video_surface * surface, int flags)
{
    return ctx ? ctx->funcs.ef_encode(ctx, surface, flags) : -1;
}

MEDIACODEC_API
void
video_enc_destroy(video_enc_context * ctx)
{
    if (ctx)
    {
        ctx->funcs.ef_destroy(ctx);
        free(ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
