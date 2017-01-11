#include "avcenc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int
avc_enc_create(avc_enc_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    *ctx = (avc_enc_context *)malloc(sizeof(avc_enc_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(avc_enc_context));

    return 0;
}

int
avc_enc_init(video_enc_context * ctx, video_enc_config * cfg)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    avc_enc_context * avc_ctx = (avc_enc_context *)ctx->priv_ctx;

    /* our baseline encoder */
    x264_param_default(&avc_ctx->param);

    /* our favor */
    avc_ctx->param.i_width                    = cfg->width;
    avc_ctx->param.i_height                   = cfg->height;
    avc_ctx->param.i_fps_num                  = cfg->framerate_num;
    avc_ctx->param.i_fps_den                  = cfg->framerate_den;
    avc_ctx->param.b_vfr_input                = 0;
    avc_ctx->param.b_intra_refresh            = 0;  ///< force key frame conflicts with this
    avc_ctx->param.i_keyint_max               = cfg->key_interval;
    avc_ctx->param.i_level_idc                = -1; ///< auto

    /* baseline */
    avc_ctx->param.analyse.b_transform_8x8    = 0;
    avc_ctx->param.i_bframe                   = 0;
    avc_ctx->param.b_cabac                    = 0;
    avc_ctx->param.b_interlaced               = 0;
    avc_ctx->param.b_fake_interlaced          = 0;
    avc_ctx->param.i_cqm_preset               = X264_CQM_FLAT;
    avc_ctx->param.analyse.i_weighted_pred    = X264_WEIGHTP_NONE;

    /* fast */
    avc_ctx->param.i_scenecut_threshold       = 0;
    avc_ctx->param.i_frame_reference          = 1;
    avc_ctx->param.i_bframe_adaptive          = X264_B_ADAPT_NONE;
    avc_ctx->param.b_sliced_threads           = 0;

    //avc_ctx->param.analyse.intra              = 0;
    //avc_ctx->param.analyse.inter              = 0;
    avc_ctx->param.analyse.i_me_method        = X264_ME_DIA;
    avc_ctx->param.analyse.i_subpel_refine    = 2;
    avc_ctx->param.analyse.b_mixed_references = 0;
    avc_ctx->param.analyse.i_trellis          = 0;
    avc_ctx->param.analyse.b_weighted_bipred  = 0;
    //avc_ctx->param.analyse.b_fast_pskip       = 1;

    avc_ctx->param.b_repeat_headers           = 1;
    avc_ctx->param.b_annexb                   = 1;

    avc_ctx->param.rc.i_rc_method             = X264_RC_ABR;
    avc_ctx->param.rc.i_bitrate               = cfg->bitrate;
    avc_ctx->param.rc.i_aq_mode               = 0;
    avc_ctx->param.rc.i_lookahead             = 0;
    avc_ctx->param.rc.b_mb_tree               = 0;
    avc_ctx->param.rc.f_rate_tolerance        = 0.1f;

    avc_ctx->param.rc.i_vbv_buffer_size       = cfg->bitrate;
    avc_ctx->param.rc.i_vbv_max_bitrate       = cfg->bitrate;
    avc_ctx->param.rc.f_vbv_buffer_init       = 0.9f;
    avc_ctx->param.i_slice_max_size           = 1200;

    if (cfg->mode & ENC_MODE_LOW_LATENCY)
    {
        /* frame level parallel introduce latency, we use slice-based multi-thread instead */
        avc_ctx->param.rc.i_lookahead   = 0;
        avc_ctx->param.i_sync_lookahead = 0;
        avc_ctx->param.i_bframe         = 0;
        avc_ctx->param.b_sliced_threads = 1;
        avc_ctx->param.rc.b_mb_tree     = 0;
    }

    if (cfg->max_threads == 0)
    {
        if ((cfg->framerate_num / cfg->framerate_den) * ((cfg->width * cfg->height) >> 8) <= 36000/*VGA@30FPS*/)
        {
            avc_ctx->param.i_threads = 2;
        }
        else
        {
            avc_ctx->param.i_threads = 4;
        }
    }
    else
    {
        avc_ctx->param.i_threads = cfg->max_threads;
    }

    if ((avc_ctx->x264 = x264_encoder_open(&avc_ctx->param)) == 0)
    {
        video_enc_log(ctx, "x264 encoder open failed.");
        return -1;
    }

    ctx->config = *cfg;

    return 0;
}

int
avc_enc_update_framerate(video_enc_context * ctx, int frame_rate)
{
    return 0;
}

int
avc_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps)
{
    return 0;
}

void
avc_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame)
{
    video_enc_free_frame(frame);
}

int
avc_enc_encode(video_enc_context * ctx, video_surface * surface, int flags)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    avc_enc_context * avc_ctx = (avc_enc_context *)ctx->priv_ctx;

    x264_picture_t    pic_out;
    x264_nal_t      * nalus             = 0;
    int               num_nalus         = 0;
    int               nalu              = 0;
    int               droppable         = 1;
    int               start_code_len    = 0;

    video_enc_frame_bs * frame = 0;

    if (surface->width[0] != ctx->config.width || surface->height[0] != ctx->config.height)
    {
        /* we do no scale */
        video_enc_log(ctx, "no scale is support within the avc encoder.");
        return -1;
    }

    if (surface->color != IMG_FMT_I420)
    {
        video_enc_log(ctx, "avc encoder doesn't support color convert, frame is not encoded.");
        return -1;
    }

    x264_picture_init(&avc_ctx->picture);

    avc_ctx->picture.img.i_csp          = X264_CSP_I420;
    avc_ctx->picture.img.i_plane        = 3;
    avc_ctx->picture.i_type             = flags & VID_ENC_CTRLFLAG_KEY ? X264_TYPE_IDR : X264_TYPE_AUTO;
    avc_ctx->picture.i_qpplus1          = 0;
    avc_ctx->picture.i_pts              = surface->pts;

    avc_ctx->picture.img.plane[0]       = surface->planes[0];
    avc_ctx->picture.img.plane[1]       = surface->planes[1];
    avc_ctx->picture.img.plane[2]       = surface->planes[2];

    avc_ctx->picture.img.i_stride[0]    = surface->pitch[0];
    avc_ctx->picture.img.i_stride[1]    = surface->pitch[1];
    avc_ctx->picture.img.i_stride[2]    = surface->pitch[2];

    if (x264_encoder_encode(avc_ctx->x264, &nalus, &num_nalus, &avc_ctx->picture, &pic_out) < 0)
    {
        video_enc_log(ctx, "avc encoder encode error!");
        return -1;
    }

    if (num_nalus == 0)
    {
        video_enc_log(ctx, "avc encoder encode no frame!");
        return 0;
    }

    frame = video_enc_get_free_frame(ctx, num_nalus);
    if (!frame)
    {
        video_enc_log(ctx, "avc encoder failed to get free frame!");
        return -1;
    }

    if (pic_out.i_type == X264_TYPE_IDR || pic_out.i_type == X264_TYPE_I)
    {
        frame->flags |= VIDEO_FRAME_TYPE_KEY;
    }

    frame->pts       = pic_out.i_pts;
    frame->prop      = surface->prop;
    frame->opaque    = surface->opaque;
    frame->len       = 0;
    frame->num_units = 0;
    frame->codec     = CODEC_TYPE_AVC;

    for (nalu = 0; nalu  < num_nalus; ++nalu)
    {
        frame->units[frame->num_units].start_code_len = 0;

        if (nalus[nalu].p_payload[0] == 0 && nalus[nalu].p_payload[1] == 0)
        {
            if (nalus[nalu].p_payload[2] == 1)
            {
                start_code_len = 3;
            }
            else if (nalus[nalu].p_payload[2] == 0 && nalus[nalu].p_payload[3] == 1)
            {
                start_code_len = 4;
            }
        }

        if (start_code_len == 0)
        {
            video_enc_log(ctx, "one malformed nalu(no start code) is abandoned!");
            continue;
        }

        frame->units[frame->num_units].start_code_len = start_code_len;

        int nal_ref_idc = frame->units[frame->num_units].data[start_code_len] & 0x60;
        if (droppable && nal_ref_idc)
        {
            droppable = 0;
        }

        frame->units[frame->num_units].data      = frame->data + frame->len;
        frame->units[frame->num_units].len       = nalus[nalu].i_payload;
        frame->units[frame->num_units].flags    |= (nal_ref_idc == 0 ? VID_ENC_UNIT_DROPPABLE : 0);
        frame->units[frame->num_units].reserved  = 0;

        ++frame->num_units;

        memcpy(frame->data + frame->len, nalus[nalu].p_payload, nalus[nalu].i_payload);
        frame->len += nalus[nalu].i_payload;
    }

    frame->codec_priv.avc.tl_idx = 0;
    frame->codec_priv.avc.non_reference = droppable ? 1 : 0;

    if (ctx->cb_out)
    {
        ctx->cb_out(ctx, frame, ctx->cb_arg);
    }

    return 0;
}

void
avc_enc_destroy(video_enc_context * ctx)
{
    if (ctx)
    {
        avc_enc_context * avc_ctx = (avc_enc_context *)ctx->priv_ctx;

        x264_encoder_close(avc_ctx->x264);
        free(avc_ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
