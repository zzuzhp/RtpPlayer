#include "./tools_common.h"
#include "./file_dump.h"

#include "vp8enc.h"
#include "vpx/vpx_encoder.h"
#include "vpx_ports/vpx_timer.h"
#include "vpx_mem/vpx_mem.h"

#include <stdlib.h>

static int 
DetectNumberOfCores()
{
    /* We fall back on assuming a single core in case of errors. */
    return 1;
}

static void
vp8_enc_log(vp8_enc_context * ctx, const char * fmt, ...)
{
    if (ctx->cb_out_log)
    {
#define MSG_BUF_SIZE 1024
        static char msg[MSG_BUF_SIZE];

        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msg, MSG_BUF_SIZE - 1, fmt, ap);
        va_end(ap);

        ctx->cb_out_log(ctx, msg, ctx->cb_arg_log);
    }
}

int
vp8_enc_create(vp8_enc_context ** ctx)
{
    if (!ctx)
    {
        return -1;
    }

    *ctx = (vp8_enc_context *)vpx_malloc(sizeof(vp8_enc_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(vp8_enc_context));
    return 0;
}

int 
vp8_enc_init(vp8_enc_context * ctx, video_enc_config * config)
{
    int flags = 0;
    struct vpx_codec_enc_cfg cfg;
    /* check encoder config */
    if (!ctx || !config)
    {
        vp8_enc_log(ctx, "invalid param passed to vp8_enc_init.");
        return -1;
    }

    /* populate encoder configuration with default values */
    vpx_codec_err_t res = vpx_codec_enc_config_default(vpx_codec_vp8_cx(), &cfg, 0);
    if (res)
    {
        vp8_enc_log(ctx, "Failed to get config: %s\n", vpx_codec_err_to_string(res));
        return -1;
    }

    /* Change the default timebase to a high enough value so that the
     * encoder will always create strictly increasing timestamps.
     */
    cfg.g_timebase.num = config->timebase_num;
    cfg.g_timebase.den = config->timebase_den;
    cfg.g_lag_in_frames = 0;  // 0- no frame lagging

    /* Never use the library's default resolution, require it be parsed
     * from the file or set on the command line.
     */
    cfg.g_w = config->width;
    cfg.g_h = config->height;

    /* bitrate control */
    cfg.g_error_resilient   = 1;
    cfg.rc_dropframe_thresh = config->strict_bitrate ? 30 : 0;
    cfg.rc_end_usage        = VPX_CBR;
    cfg.g_pass              = VPX_RC_ONE_PASS;
    cfg.rc_resize_allowed   = 0;
    cfg.rc_target_bitrate   = config->bitrate;
    cfg.rc_buf_sz           = 1000;
    cfg.rc_buf_initial_sz   = 500;
    cfg.rc_buf_optimal_sz   = 600; 
    cfg.rc_undershoot_pct   = 100;
    cfg.rc_overshoot_pct    = 15;

    /* key frame settings */
    if (config->key_interval > 0)
    {
        cfg.kf_mode     = VPX_KF_AUTO;
        cfg.kf_max_dist = config->key_interval;
    }
    else
    {
        cfg.kf_mode = VPX_KF_DISABLED;
    }

    /* Need a key frame if we have not sent this stream before */
    ctx->key_frame_request = 1;

    /* svc stuff */
    if (config->enable_temporal_layers > 0)
    {
        ctx->flag_idc = 255;

        ctx->temporal_ids[0] = 0;
        ctx->temporal_ids[1] = 2;
        ctx->temporal_ids[2] = 1;
        ctx->temporal_ids[3] = 2;

        cfg.ts_number_layers = 3;
        cfg.ts_periodicity   = 4;

        cfg.ts_target_bitrate[0] = (unsigned int)(config->bitrate * 0.4f);
        cfg.ts_target_bitrate[1] = (unsigned int)(config->bitrate * 0.6f);
        cfg.ts_target_bitrate[2] = config->bitrate;
        cfg.ts_rate_decimator[0] = 4;
        cfg.ts_rate_decimator[1] = 2;
        cfg.ts_rate_decimator[2] = 1;

        ctx->encode_flags[0] = VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_REF_GF;
        ctx->encode_flags[1] = VP8_EFLAG_NO_REF_GF  | VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF  | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        ctx->encode_flags[2] = VP8_EFLAG_NO_REF_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST;
        ctx->encode_flags[3] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        ctx->encode_flags[4] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_REF_GF;
        ctx->encode_flags[5] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        ctx->encode_flags[6] = VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST;
        ctx->encode_flags[7] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
    }

    /* parallel encoding */
    cfg.g_threads = config->max_threads == 0 ? DetectNumberOfCores() : config->max_threads;
    cfg.g_profile = 0; ///< 0 as default. FYI: soccer_4CIF_500kbps[0(psnr:31.4) / 1(psnr:31.3) /2(psnr:31.0) /3(psnr:30.5)]

#if VPX_ENC_INIT_COLLECT_PSNR
    flags |= VPX_CODEC_USE_PSNR;
#endif
    flags |= VPX_CODEC_USE_OUTPUT_PARTITION;

    /* Construct Encoder Context */
    vpx_codec_enc_init(&ctx->encoder, vpx_codec_vp8_cx(), &cfg, flags);
    if (ctx->encoder.err)
    {
        vp8_enc_log(ctx, "Failed to initialize encoder: %s\n", vpx_codec_error_detail(&ctx->encoder));
        return -1;
    }

    vpx_codec_control(&ctx->encoder, VP8E_SET_TOKEN_PARTITIONS, (vp8e_token_partitions)(VPX_ENC_TOKEN_PARTITIONS));

    ctx->image = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, config->width, config->height, 16);
    if (!ctx->image)
    {
        vp8_enc_destroy(ctx);
        return -1;
    }

    ctx->out_frame = (video_enc_frame_bs *)vpx_malloc(sizeof(video_enc_frame_bs));
    memset(ctx->out_frame, 0, sizeof(video_enc_frame_bs));

    ctx->out_frame->num_units = 0;
    ctx->out_frame->units = (video_enc_data_unit_bs *)vpx_malloc(sizeof(video_enc_data_unit_bs) * ((1 << VPX_ENC_TOKEN_PARTITIONS) + 1));

    ctx->buffer_bs_size = (size_t)(config->width * config->height * 4/*ARGB*/);
    ctx->buffer_bs = (uint8_t *)vpx_malloc(ctx->buffer_bs_size);

    ctx->last_pts = 0;
    ctx->config   = *config;

#if VPX_ENC_DUMP_LAYERS
    vpx_create_fileDumper(&ctx->bs_dumper);
#endif

    return 0;
}

int 
vp8_enc_get_caps(vp8_enc_context * ctx)
{
    return 0;
}

int 
vp8_enc_set_cb_log(vp8_enc_context * ctx, cb_enc_log cb_log, void * arg)
{
    ctx->cb_arg_log = arg;
    ctx->cb_out_log = cb_log;

    return 0;
}

int 
vp8_enc_set_cb_output(vp8_enc_context * ctx, cb_enc_output cb_frame, void * arg)
{
    ctx->cb_arg = arg;
    ctx->cb_out = cb_frame;

    return 0;
}

int 
vp8_enc_set_rates(vp8_enc_context * ctx, int new_bitrate_kbit, int frame_rate)
{
    return 0;
}

int 
populate_codec_priv(vp8_enc_context * ctx, const vpx_codec_cx_pkt_t * pkt)
{
    video_enc_priv_data_VP8 * vp8_priv = &ctx->out_frame->codec_priv.vp8;
    int base_layer_sync = pkt->data.frame.flags & VPX_FRAME_IS_KEY; ///< todo: frames that depend only on key frames also apply.

    vp8_priv->key_idx = -1;
    vp8_priv->non_reference = (pkt->data.frame.flags & VPX_FRAME_IS_DROPPABLE) ? 1 : 0;

    if (base_layer_sync)
    {
        vp8_priv->layer_sync = 1;
        vp8_priv->temporal_idx = 0;
    }
    else
    {
        int pattern_idc = ctx->flag_idc % VPX_ENC_3TL_PATTERN;

        vp8_priv->temporal_idx = ctx->temporal_ids[ctx->flag_idc % VPX_ENC_3TL_IDS];

        if (pattern_idc == 1 || pattern_idc == 2)
        {
            vp8_priv->layer_sync = 1;
        }
        else
        {
            vp8_priv->layer_sync = 0;
        }
    }

    if (ctx->last_base_layer_sync && vp8_priv->temporal_idx != 0)
    {
        /* Regardless of pattern the frame after a base layer sync will always be a layer sync. */
        vp8_priv->layer_sync = 1;
    }

    if (vp8_priv->temporal_idx == 0)
    {
        ctx->tl0_pic_idx++;
    }

    ctx->last_base_layer_sync = base_layer_sync;
    vp8_priv->tl0_pic_idx = ctx->tl0_pic_idx;

    vp8_priv->picture_id = ctx->picture_id;
    ctx->picture_id = (ctx->picture_id + 1) & 0x7FFF;

    return 0;
}

int 
get_partitions(vp8_enc_context * ctx, long long pts, long long opaque)
{
    const vpx_codec_cx_pkt_t * pkt;
    vpx_codec_iter_t iter = NULL;

    int got_pic = 0;
    int buffer_len = 0;

    ctx->out_frame->num_units = 0;
    ctx->out_frame->flags = 0;
    ctx->out_frame->pts   = 0;
    ctx->out_frame->codec = CODEC_TYPE_VP8;

    while ((pkt = vpx_codec_get_cx_data(&ctx->encoder, &iter)))
    {
        switch (pkt->kind)
        {
            case VPX_CODEC_CX_FRAME_PKT:
            {
                size_t partition_len = pkt->data.frame.sz;
                uint8_t * partition = (uint8_t *)pkt->data.frame.buf;

                if (got_pic)
                {
                    /* warning already got a picture */
                    vp8_enc_log(ctx, "already got a picture!!!");
                }

                if (ctx->out_frame->num_units >= VPX_ENC_PARTITIONS ||
                    pkt->data.frame.partition_id != ctx->out_frame->num_units)
                {
                    /* partition should */
                    vp8_enc_log(ctx, "vp8 partition check failed.");
                    return -1;
                }
#if 0
                /* for debug: mark some frames as not displayable */
                if (ctx->frames % 3 != 0 && pkt->data.frame.partition_id == 0)
                {
                    partition[0] &= 0xef; ///< do not show this frame
                }
#endif
                if (partition_len + buffer_len > ctx->buffer_bs_size)
                {
                    size_t new_size = ctx->buffer_bs_size + partition_len + 1024;
                    uint8_t * buffer_bs = (uint8_t *)calloc(1, new_size);
                    if (!buffer_bs)
                    {
                        vp8_enc_log(ctx, "alloc memory failed for the output stream.");
                        return -1;
                    }

                    memcpy(buffer_bs, ctx->buffer_bs, buffer_len);

                    ctx->buffer_bs = buffer_bs;
                    ctx->buffer_bs_size = new_size;
                }

                memcpy(ctx->buffer_bs + buffer_len, partition, partition_len);

                ctx->out_frame->units[ctx->out_frame->num_units].data = ctx->buffer_bs + buffer_len;
                ctx->out_frame->units[ctx->out_frame->num_units].length = (int)partition_len;

                ++ctx->out_frame->num_units;

                buffer_len += (int)partition_len;

                /* end of frame */
                if ((pkt->data.frame.flags & VPX_FRAME_IS_FRAGMENT) == 0)
                {
                    if (pkt->data.frame.flags & VPX_FRAME_IS_KEY)
                    {
                        ctx->out_frame->flags |= VIDEO_FRAME_TYPE_KEY;
                    }

                    ctx->out_frame->pts    = pts;
                    ctx->out_frame->opaque = opaque;

                    populate_codec_priv(ctx, pkt);

#if VPX_ENC_DUMP_LAYERS
                    if (ctx->out_frame->codec_priv.vp8.temporal_idx <= VPX_ENC_DUMP_LAYERS - 1)
                    {
                        vpx_write_frame(ctx->bs_dumper, &ctx->config, ctx->buffer_bs, buffer_len, pts);
                    }
#endif
                    got_pic = 1;
                }

                break;
            }
            case VPX_CODEC_PSNR_PKT:
            {
#if VPX_ENC_INIT_COLLECT_PSNR
                ctx->psnr[0] = pkt->data.psnr.psnr[0];
                ctx->out_frame->psnr = ctx->psnr[0];
#endif
                break;
            }
            default:
            {
                break;
            }
        }

        /* deliver this frame */
        if (got_pic && ctx->cb_out)
        {
            got_pic = 0;
            ctx->cb_out(ctx, ctx->out_frame, ctx->cb_arg);
        }
    }

    return 0;
}

int 
vp8_enc_encode(vp8_enc_context * ctx, video_surface * surface, int flags)
{
    int enc_flag = 0;
    struct vpx_usec_timer timer;

    if (surface->width[0] != ctx->config.width || surface->height[0] != ctx->config.height)
    {
        /* we do no scale */
        vp8_enc_log(ctx, "no scale is support within the vp8 encoder.");
        return -1;
    }

    if (surface->color != IMG_FMT_I420)
    {
#if 0 ///< need libyuv
        ctx->image->stride[VPX_PLANE_Y] = surface->width[0];
        ctx->image->stride[VPX_PLANE_U] = surface->width[0] >> 1;
        ctx->image->stride[VPX_PLANE_V] = surface->width[0] >> 1;

        if (0 > ConvertToI420(surface->planes[0],
                              0,
                              ctx->image->planes[VPX_PLANE_Y],
                              ctx->image->stride[VPX_PLANE_Y],
                              ctx->image->planes[VPX_PLANE_U],
                              ctx->image->stride[VPX_PLANE_U],
                              ctx->image->planes[VPX_PLANE_V],
                              ctx->image->stride[VPX_PLANE_V],
                              0, 0, ///< crop_x, crop_y
                              surface->width[0],
                              surface->height[0],
                              surface->width[0],
                              surface->height[0],
                              kRotate0,
                              supportedFourCCs[surface->color].fourcc))
#endif
        {
            vp8_enc_log(ctx, "convert color failed for one frame, frame is not encoded.");
            return -1;
        }
    }
    else
    {
        // Image in vpx_image_t format.
        // Input image is const. VP8's raw image is not defined as const.
        ctx->image->planes[VPX_PLANE_Y] = surface->planes[VPX_PLANE_Y];
        ctx->image->planes[VPX_PLANE_U] = surface->planes[VPX_PLANE_U];
        ctx->image->planes[VPX_PLANE_V] = surface->planes[VPX_PLANE_V];

        ctx->image->stride[VPX_PLANE_Y] = surface->pitch[VPX_PLANE_Y];
        ctx->image->stride[VPX_PLANE_U] = surface->pitch[VPX_PLANE_U];
        ctx->image->stride[VPX_PLANE_V] = surface->pitch[VPX_PLANE_V];
    }

    ///< uint32_t duration = ctx->frames == 0 ? (40 * ctx->config.timebase_den / ctx->config.timebase_num / 1000) : (surface->pts - ctx->last_pts);
    uint32_t duration = ctx->config.timebase_den * ctx->config.framerate_den / (ctx->config.framerate_num * ctx->config.timebase_num);

    enc_flag = ctx->encode_flags[++ctx->flag_idc % VPX_ENC_3TL_PATTERN];

    if (ctx->key_frame_request || (flags & VPX_ENC_CONTROL_KEY))
    {
        enc_flag |= VPX_EFLAG_FORCE_KF;
        ctx->key_frame_request = 0;
    }

    vpx_codec_control(&ctx->encoder, VP8E_SET_FRAME_FLAGS, enc_flag);
    vpx_codec_control(&ctx->encoder, VP8E_SET_TEMPORAL_LAYER_ID, ctx->temporal_ids[ctx->flag_idc % VPX_ENC_3TL_IDS]);

    vpx_usec_timer_start(&timer);
    vpx_codec_encode(&ctx->encoder, ctx->image, surface->pts, duration, 0, VPX_DL_REALTIME);
    vpx_usec_timer_mark(&timer);

    get_partitions(ctx, surface->pts, surface->opaque);

    ctx->cx_time += vpx_usec_timer_elapsed(&timer);
    ++ctx->frames;

    ctx->last_pts = surface->pts;

    return 0;
}

void 
vp8_enc_destroy(vp8_enc_context * ctx)
{
    if (ctx)
    {
        vpx_img_free(ctx->image);
        vpx_free(ctx->buffer_bs);

        if (ctx->out_frame)
        {
            vpx_free(ctx->out_frame->units);
            vpx_free(ctx->out_frame);
        }

#if VPX_ENC_DUMP_LAYERS
        vpx_destroy_fileDumper(ctx->bs_dumper);
#endif

        vpx_free(ctx);
    }
}
