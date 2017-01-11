#include "avcdec.h"
#include "libavutil/mem.h"
#include "libavutil/log.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

static
void
ff_log_report(void * ptr, int level, const char * fmt, va_list vl)
{
    static char line[MAX_LOG_MESSAGE_BUF_SIZE] = {0};
    AVClass * avclass = ptr ? *(AVClass **)ptr : NULL;
    if (avclass)
    {
        if (avclass->parent_log_context_offset)
        {
            AVClass ** parent = *(AVClass ***) (((uint8_t *)ptr) + avclass->parent_log_context_offset);
            if (parent && *parent)
            {
                snprintf(line, sizeof(line), "[%s @ %p] ", (*parent)->item_name(parent), parent);
            }
        }

        snprintf(line + strlen(line), sizeof(line) - strlen(line), "[%s @ %p] ", avclass->item_name(ptr), ptr);
    }

    vsnprintf(line + strlen(line), sizeof(line) - strlen(line), fmt, vl);

    if (level == AV_LOG_PANIC || level == AV_LOG_FATAL || level == AV_LOG_ERROR || level == AV_LOG_WARNING)
    {
        /* forward only serious messages */
        video_dec_log(0, "%s", line);
    }
}

int
avc_dec_create(avc_dec_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    *ctx = (avc_dec_context *)av_malloc(sizeof(avc_dec_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(avc_dec_context));
    return 0;
}

int
avc_dec_init(video_dec_context * ctx, int max_threads)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    avc_dec_context * avc_ctx = (avc_dec_context *)ctx->priv_ctx;

    avcodec_register_all();

    av_log_set_callback(ff_log_report);

    avc_ctx->avcodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!avc_ctx->avcodec)
    {
        video_dec_log(ctx, "AV_CODEC_ID_H264 not found.");
        return -1;
    }

    avc_ctx->avcodec_ctx = avcodec_alloc_context3(avc_ctx->avcodec);
    if (!avc_ctx->avcodec_ctx)
    {
        video_dec_log(ctx, "could not allocate video codec context.");
        return -1;
    }

    if (avcodec_open2(avc_ctx->avcodec_ctx, avc_ctx->avcodec, NULL) < 0)
    {
        video_dec_log(ctx, "could not open codec.");
        return -1;
    }

    avc_ctx->avframe = av_frame_alloc();
    if (!avc_ctx->avframe)
    {
        video_dec_log(ctx, "could not allocate video frame.");
        return -1;
    }

    return 0;
}

int
avc_dec_decode(video_dec_context    * ctx,
               const unsigned char  * frame,
               int                    len,
               long long              pts,
               long long              opaque)
{
    int res = 0;
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    AVPacket pkt;
    avc_dec_context * avc_ctx = (avc_dec_context *)ctx->priv_ctx;

    av_init_packet(&pkt);

    pkt.pts  = pts;
    pkt.data = (unsigned char *)frame;
    pkt.size = len;

    /* decode */
    if (0 != (res = avcodec_send_packet(avc_ctx->avcodec_ctx, &pkt)))
    {
        video_dec_log(ctx, "avcodec_send_packet return :%d", res);
    }

    /* get decoded image */
    if (0 != (res = avcodec_receive_frame(avc_ctx->avcodec_ctx, avc_ctx->avframe)))
    {
        video_dec_log(ctx, "no frame: %d", res);
    }
    else
    {
        /* success, a frame was returned */
        video_dec_send_image(ctx,
                             (const unsigned char **)avc_ctx->avframe->data,
                             avc_ctx->avframe->width,
                             avc_ctx->avframe->height,
                             avc_ctx->avframe->linesize,
                             avc_ctx->avframe->pts,
                             opaque); ///!!! which opaque?
    }

    return 0;
}

void
avc_dec_release_image(video_dec_context * ctx, video_surface * image)
{
    video_dec_free_image(image);
}

void
avc_dec_destroy(video_dec_context * ctx)
{
    if (ctx && ctx->priv_ctx)
    {
        avc_dec_context * avc_ctx = (avc_dec_context *)ctx->priv_ctx;

        if (avc_ctx->avcodec_ctx)
        {
            avcodec_close(avc_ctx->avcodec_ctx);
            av_free(avc_ctx->avcodec_ctx);
        }

        if (avc_ctx->avframe)
        {
            av_frame_free(&avc_ctx->avframe);
        }

        free(avc_ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
