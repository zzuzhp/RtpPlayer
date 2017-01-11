#include "./tools_common.h"

#include "vp8enc.h"
#include "vpx/vpx_encoder.h"
#include "vpx_ports/vpx_timer.h"
#include "vpx_mem/vpx_mem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int
vp8_enc_create(vp8_enc_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1)

    *ctx = (vp8_enc_context *)vpx_malloc(sizeof(vp8_enc_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(vp8_enc_context));
    return 0;
}

int 
vp8_enc_init(video_enc_context * ctx, video_enc_config * config)
{
    int flags = 0;
    struct vpx_codec_enc_cfg cfg;

    FUNC_CHECK_NONZERO_PARAM(ctx && config, -1)

    vp8_enc_context * vp8_ctx = (vp8_enc_context *)ctx->priv_ctx;

    /* populate encoder configuration with default values */
    vpx_codec_err_t res = vpx_codec_enc_config_default(vpx_codec_vp8_cx(), &cfg, 0);
    if (res)
    {
        video_enc_log(ctx, "vp8 encoder failed to get default config: %s", vpx_codec_err_to_string(res));
        return -1;
    }

    /* Change the default timebase to a high enough value so that the
     * encoder will always create strictly increasing timestamps.
     */
    cfg.g_timebase.num  = config->timebase_num;
    cfg.g_timebase.den  = config->timebase_den;
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
    vp8_ctx->key_frame_request = 1;

    /* svc stuff */
    if (config->enable_temporal_layers > 0)
    {
        vp8_ctx->flag_idc = 255;

        vp8_ctx->temporal_ids[0] = 0;
        vp8_ctx->temporal_ids[1] = 2;
        vp8_ctx->temporal_ids[2] = 1;
        vp8_ctx->temporal_ids[3] = 2;

        cfg.ts_number_layers = 3;
        cfg.ts_periodicity   = 4;

        cfg.ts_target_bitrate[0] = (unsigned int)(config->bitrate * 0.4f);
        cfg.ts_target_bitrate[1] = (unsigned int)(config->bitrate * 0.6f);
        cfg.ts_target_bitrate[2] = config->bitrate;
        cfg.ts_rate_decimator[0] = 4;
        cfg.ts_rate_decimator[1] = 2;
        cfg.ts_rate_decimator[2] = 1;

        vp8_ctx->encode_flags[0] = VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_REF_GF;
        vp8_ctx->encode_flags[1] = VP8_EFLAG_NO_REF_GF  | VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF  | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        vp8_ctx->encode_flags[2] = VP8_EFLAG_NO_REF_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST;
        vp8_ctx->encode_flags[3] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        vp8_ctx->encode_flags[4] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_REF_GF;
        vp8_ctx->encode_flags[5] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
        vp8_ctx->encode_flags[6] = VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST;
        vp8_ctx->encode_flags[7] = VP8_EFLAG_NO_UPD_GF  | VP8_EFLAG_NO_UPD_ARF | VP8_EFLAG_NO_UPD_LAST | VP8_EFLAG_NO_UPD_ENTROPY;
    }

    /* parallel encoding */
    cfg.g_threads = config->max_threads == 0 ? detect_number_cores() : config->max_threads;
    cfg.g_profile = config->mode == ENC_MODE_FAST_DEC ? 2 : 0; ///< larger is faster both for encoder and decoder but at a cost of image quality.
                                                               ///< FYI: soccer_4CIF_500kbps[0(psnr:31.4) / 1(psnr:31.3) /2(psnr:31.0) /3(psnr:30.5)]

#if VPX_ENC_INIT_COLLECT_PSNR
    flags |= VPX_CODEC_USE_PSNR;
#endif
    flags |= VPX_CODEC_USE_OUTPUT_PARTITION;

    /* Construct Encoder Context */
    vpx_codec_enc_init(&vp8_ctx->encoder, vpx_codec_vp8_cx(), &cfg, flags);
    if (vp8_ctx->encoder.err)
    {
        video_enc_log(ctx, "failed to initialize vp8 encoder: %s", vpx_codec_error_detail(&vp8_ctx->encoder));
        return -1;
    }

    vpx_codec_control(&vp8_ctx->encoder, VP8E_SET_TOKEN_PARTITIONS, (vp8e_token_partitions)(VPX_ENC_TOKEN_PARTITIONS));

    vp8_ctx->image = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, config->width, config->height, 16);
    if (!vp8_ctx->image)
    {
        vp8_enc_destroy(ctx);
        return -1;
    }

    vp8_ctx->last_pts = 0;
    ctx->config   = *config;

#if VPX_ENC_DUMP_LAYERS
    create_fileWriter(&vp8_ctx->bs_dumper, CODEC_TYPE_VP8);
#endif

    return 0;
}

int 
vp8_enc_update_framerate(video_enc_context * ctx, int frame_rate)
{
    return 0;
}

int
vp8_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps)
{
    return 0;
}

void
vp8_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame)
{
    video_enc_free_frame(frame);
}

void
populate_codec_priv(video_enc_context * ctx, const vpx_codec_cx_pkt_t * pkt, video_enc_priv_info_VP8 * vp8_priv)
{
    vp8_enc_context * vp8_ctx = (vp8_enc_context *)ctx->priv_ctx;

    int base_layer_sync = pkt->data.frame.flags & VPX_FRAME_IS_KEY; ///< todo: frames that depend only on key frames also apply.

    vp8_priv->key_idx       = -1;
    vp8_priv->non_reference = (pkt->data.frame.flags & VPX_FRAME_IS_DROPPABLE) ? 1 : 0;

    if (base_layer_sync)
    {
        vp8_priv->layer_sync   = 1;
        vp8_priv->temporal_idx = 0;
    }
    else
    {
        int pattern_idc = vp8_ctx->flag_idc % VPX_ENC_3TL_PATTERN;

        vp8_priv->temporal_idx = vp8_ctx->temporal_ids[vp8_ctx->flag_idc % VPX_ENC_3TL_IDS];

        if (pattern_idc == 1 || pattern_idc == 2)
        {
            vp8_priv->layer_sync = 1;
        }
        else
        {
            vp8_priv->layer_sync = 0;
        }
    }

    if (vp8_ctx->last_base_layer_sync && vp8_priv->temporal_idx != 0)
    {
        /* regardless of pattern the frame after a base layer sync will always be a layer sync. */
        vp8_priv->layer_sync = 1;
    }

    if (vp8_priv->temporal_idx == 0)
    {
        vp8_ctx->tl0_pic_idx++;
    }

    vp8_ctx->last_base_layer_sync = base_layer_sync;
    vp8_priv->tl0_pic_idx         = vp8_ctx->tl0_pic_idx;

    vp8_priv->picture_id = vp8_ctx->picture_id;
    vp8_ctx->picture_id  = (vp8_ctx->picture_id + 1) & 0x7FFF;
}

int 
get_partitions(video_enc_context * ctx, long long pts, video_surface_prop * prop, long long opaque)
{
    vp8_enc_context * vp8_ctx = (vp8_enc_context *)ctx->priv_ctx;

    const vpx_codec_cx_pkt_t * pkt;
    vpx_codec_iter_t iter = NULL;

    int got_pic = 0;

    video_enc_frame_bs * frame = video_enc_get_free_frame(ctx, (1 << VPX_ENC_TOKEN_PARTITIONS) + 1);
    if (!frame)
    {
        return -1;
    }

    frame->len       = 0;
    frame->num_units = 0;
    frame->codec     = CODEC_TYPE_VP8;

    while ((pkt = vpx_codec_get_cx_data(&vp8_ctx->encoder, &iter)))
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
                    video_enc_log(ctx, "already got a picture!!!");
                }

                if (frame->num_units >= VPX_ENC_PARTITIONS ||
                    pkt->data.frame.partition_id != frame->num_units)
                {
                    /* partition should */
                    video_enc_log(ctx, "vp8 partition check failed.");
                    return -1;
                }
#if 0
                /* for debug: mark some frames as not displayable */
                if (vp8_ctx->frames % 3 != 0 && pkt->data.frame.partition_id == 0)
                {
                    partition[0] &= 0xef; ///< do not show this frame
                }
#endif
                frame->units[frame->num_units].data             = frame->data + frame->len;
                frame->units[frame->num_units].len              = (int)partition_len;
                frame->units[frame->num_units].start_code_len   = 0;
                frame->units[frame->num_units].flags           |= (pkt->data.frame.flags & VPX_FRAME_IS_DROPPABLE) ? VID_ENC_UNIT_DROPPABLE : 0; ///< same as non-reference
                frame->units[frame->num_units].reserved         = 0;

                ++frame->num_units;

                memcpy(frame->data + frame->len, partition, partition_len);
                frame->len += (int)partition_len;

                /* end of frame */
                if ((pkt->data.frame.flags & VPX_FRAME_IS_FRAGMENT) == 0)
                {
                    if (pkt->data.frame.flags & VPX_FRAME_IS_KEY)
                    {
                        frame->flags |= VIDEO_FRAME_TYPE_KEY;
                    }

                    frame->pts    = pts;
                    frame->prop   = *prop;
                    frame->opaque = opaque;

                    populate_codec_priv(ctx, pkt, &frame->codec_priv.vp8);

#if VPX_ENC_DUMP_LAYERS
                    if (frame->codec_priv.vp8.temporal_idx <= VPX_ENC_DUMP_LAYERS - 1)
                    {
                        write_frame(vp8_ctx->bs_dumper, &ctx->config, frame->data, frame->len, frame->pts);
                    }
#endif
                    got_pic = 1;
                }

                break;
            }
            case VPX_CODEC_PSNR_PKT:
            {
#if VPX_ENC_INIT_COLLECT_PSNR
                vp8_ctx->psnr[0] = pkt->data.psnr.psnr[0];
                frame->psnr      = vp8_ctx->psnr[0];
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
            ctx->cb_out(ctx, frame, ctx->cb_arg);
        }
    }

    return 0;
}

int 
vp8_enc_encode(video_enc_context * ctx, video_surface * surface, int flags)
{
    int enc_flag = 0;
    struct vpx_usec_timer timer;

    FUNC_CHECK_NONZERO_PARAM(ctx, -1)

    vp8_enc_context * vp8_ctx = (vp8_enc_context *)ctx->priv_ctx;

    if (surface->width[0] != ctx->config.width || surface->height[0] != ctx->config.height)
    {
        /* we do no scale */
        video_enc_log(ctx, "no scale is support within the vp8 encoder.");
        return -1;
    }

    if (surface->color != IMG_FMT_I420)
    {
        video_enc_log(ctx, "vp8 encoder doesn't support color convert, frame is not encoded.");
        return -1;
    }
    else
    {
        /* Image in vpx_image_t format */
        /* Input image is const. VP8's raw image is not defined as const */
        vp8_ctx->image->planes[VPX_PLANE_Y] = surface->planes[VPX_PLANE_Y];
        vp8_ctx->image->planes[VPX_PLANE_U] = surface->planes[VPX_PLANE_U];
        vp8_ctx->image->planes[VPX_PLANE_V] = surface->planes[VPX_PLANE_V];

        vp8_ctx->image->stride[VPX_PLANE_Y] = surface->pitch[VPX_PLANE_Y];
        vp8_ctx->image->stride[VPX_PLANE_U] = surface->pitch[VPX_PLANE_U];
        vp8_ctx->image->stride[VPX_PLANE_V] = surface->pitch[VPX_PLANE_V];
    }

    ///< uint32_t duration = ctx->frames == 0 ? (40 * ctx->config.timebase_den / ctx->config.timebase_num / 1000) : (surface->pts - ctx->last_pts);
    uint32_t duration = ctx->config.timebase_den * ctx->config.framerate_den / (ctx->config.framerate_num * ctx->config.timebase_num); ///< it seems that duration has no effect on bitrate

    enc_flag = vp8_ctx->encode_flags[++vp8_ctx->flag_idc % VPX_ENC_3TL_PATTERN];

    if (vp8_ctx->key_frame_request || (flags & VID_ENC_CTRLFLAG_KEY))
    {
        enc_flag |= VPX_EFLAG_FORCE_KF;
        vp8_ctx->key_frame_request = 0;
    }

    vpx_codec_control(&vp8_ctx->encoder, VP8E_SET_FRAME_FLAGS, enc_flag);
    vpx_codec_control(&vp8_ctx->encoder, VP8E_SET_TEMPORAL_LAYER_ID, vp8_ctx->temporal_ids[vp8_ctx->flag_idc % VPX_ENC_3TL_IDS]);

    vpx_usec_timer_start(&timer);
    vpx_codec_encode(&vp8_ctx->encoder, vp8_ctx->image, surface->pts, duration, 0, VPX_DL_REALTIME);
    vpx_usec_timer_mark(&timer);

    get_partitions(ctx, surface->pts, &surface->prop, surface->opaque);

    vp8_ctx->cx_time += vpx_usec_timer_elapsed(&timer);
    ++vp8_ctx->frames;

    vp8_ctx->last_pts = surface->pts;

    return 0;
}

void 
vp8_enc_destroy(video_enc_context * ctx)
{
    if (ctx && ctx->priv_ctx)
    {
        vp8_enc_context * vp8_ctx = (vp8_enc_context *)ctx->priv_ctx;

        vpx_img_free(vp8_ctx->image);

#if VPX_ENC_DUMP_LAYERS
        destroy_fileWriter(vp8_ctx->bs_dumper);
#endif

        vpx_free(vp8_ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
