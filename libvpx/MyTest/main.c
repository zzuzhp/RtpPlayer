#include "./MyTest/vcodec.h"
#include "vp8/common/threading.h"
#include "third_party/libyuv/include/libyuv/convert.h"

#include <stdio.h>

void 
enc_log(void * enc, const char * msg, void * cb_arg)
{
    fprintf(stderr, "VPX_test: %s", msg);
}

void 
on_enc_output(void * enc, video_enc_frame_bs * frame, void * cb_arg)
{
    int * frames = (int *)cb_arg;
    fprintf(stderr, "encoded: %4d psnr: %.1f %s\n", ++*frames, frame->psnr, frame->flags & VIDEO_FRAME_TYPE_KEY ? "(key)" : "");
#if 0
    video_enc_priv_data_VP8 vp8 = frame->codec_priv.vp8;
    fprintf(stderr, "[key_idx:%d, layer_sync:%d, non_ref:%d, pic_id:%d, temp_id:%d, tl0_pic_idx:%d]\n", vp8.key_idx, vp8.layer_sync, vp8.non_reference, vp8.picture_id, vp8.temporal_idx, vp8.tl0_pic_idx);
#endif
}

int 
main(int argc, const char **argv_)
{
    FILE * yuv = 0;
    
    fopen_s(&yuv, "e:/yuv/soccer.yuv", "rb");
    if (!yuv)
    {
        return -1;
    }
 
    int src_width     = 704;
    int src_height    = 576; 
    int encode_width  = 320;
    int encode_height = 240;
    int frames        = 0;

    assert(src_width >= encode_width && src_height >= encode_height);
    if (src_width < encode_width || src_height < encode_height)
    {
        return -1;
    }

    const int framesize = src_width * src_height * 3 / 2;
    uint8_t * frame     = (uint8_t *)calloc(1, framesize);
    if (!frame)
    {
        return -1;
    }

    void * handle = create_video_encoder(CODEC_TYPE_VP8);

    video_enc_config cfg;

    cfg.bitrate                 = 500;
    cfg.enable_temporal_layers  = 1;
    cfg.enable_spatial_layers   = 0;
    cfg.framerate_num           = 30;   ///< 30 fps
    cfg.framerate_den           = 1;
    cfg.timebase_num            = 1;
    cfg.timebase_den            = 90000;///< pts units
    cfg.key_interval            = 30;
    cfg.strict_bitrate          = 0;
    cfg.width                   = encode_width;
    cfg.height                  = encode_height;
    cfg.max_threads             = 4;

    video_surface surface;

    surface.color     = IMG_FMT_I420;
    surface.width[0]  = encode_width;
    surface.width[1]  = encode_width >> 1;
    surface.width[2]  = encode_width >> 1;
    surface.height[0] = encode_height;
    surface.height[1] = encode_height >> 1;
    surface.height[2] = encode_height >> 1;
    surface.pitch[0]  = src_width;
    surface.pitch[1]  = src_width >> 1;
    surface.pitch[2]  = src_width >> 1;
    surface.planes[0] = frame;
    surface.planes[1] = frame + src_width * src_height;
    surface.planes[2] = frame + src_width * src_height * 5 / 4;
    surface.pts       = 0;

    video_enc_init(handle, &cfg);
    video_enc_set_cb_output(handle, on_enc_output, &frames);
    video_enc_set_cb_log(handle, enc_log, 0);

    double frame_interval = 1000. * cfg.framerate_den / cfg.framerate_num;

    while (fread(frame, 1, framesize, yuv) == framesize)
    {
        video_enc_encode(handle, &surface, 0);
        surface.pts += (int64_t)(frame_interval * 90 + 0.5);

        ///< thread_sleep((int)frame_interval);
    }

    video_enc_destroy(handle);

    return 0;
}
