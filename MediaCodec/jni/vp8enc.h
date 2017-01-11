#ifndef ___VP8ENC_H___
#define ___VP8ENC_H___

#include "videodefs.h"
#include "common.h"
#include "videoenc.h"
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#include "ivfwriter.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define VPX_ENC_INIT_COLLECT_PSNR   0   ///< close to reduce complexity
#define VPX_ENC_TOKEN_PARTITIONS    3   ///< 0 or 1 or 2 or 3 ///< multi-partition enables parallel entropy encoding/decoding
#define VPX_ENC_PARTITIONS          ((1 << VPX_ENC_TOKEN_PARTITIONS) + 1)
#define VPX_ENC_DUMP_LAYERS         0   ///< 0: no dump, 1: dump base layer, 2: dump base layer + first enhanced layer, 3: dump all 3 layers

/* we use a fixed 3 layer pattern */
#define VPX_ENC_3TL                 3
#define VPX_ENC_3TL_IDS             4
#define VPX_ENC_3TL_PATTERN         8

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* defines a vp8 encoder context */
typedef struct vp8_enc_context
{
    video_enc_context   * ctx;

    int                   temporal_ids[VPX_ENC_3TL_IDS];
    int                   encode_flags[VPX_ENC_3TL_PATTERN];
    int                   key_frame_request;
    uint8_t               flag_idc;
    vpx_image_t         * image;
    video_enc_config      config;
    vpx_codec_ctx_t       encoder;

    uint16_t              picture_id;

    int                   tl0_pic_idx;
    int                   last_base_layer_sync;

    long long             last_pts;
    /* statistics */
    uint64_t              cx_time;
    uint32_t              frames;
#if VPX_ENC_INIT_COLLECT_PSNR
    double                psnr[3];
#endif

#if VPX_ENC_DUMP_LAYERS
    struct ivfwriter * bs_dumper;
#endif
} vp8_enc_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create a vp8 encoder
 * returns 0 for success, negative for failure
 */
int vp8_enc_create(vp8_enc_context ** ctx);

/*!\brief initialize a vp8 encoder
 * returns 0 for success, negative for failure
 */
int vp8_enc_init(video_enc_context * ctx, video_enc_config * cfg);

/*!\brief change the framerate
 * returns 0 for success, negative for failure
 */
int vp8_enc_update_framerate(video_enc_context * ctx, int frame_rate);

/*!\brief change the bitrate
 * returns 0 for success, negative for failure
 */
int vp8_enc_update_bitrate(video_enc_context * ctx, int bitrate_in_kbps);

/*!\brief release the frame
 */
void vp8_enc_release_frame(video_enc_context * ctx, video_enc_frame_bs * frame);

/*!\brief encode an frame
 * returns 0 for success, negative for failure
 */
int vp8_enc_encode(video_enc_context * ctx, video_surface * surface, int flags);

/*!\brief destroy the encoder
 */
void vp8_enc_destroy(video_enc_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}  ///< extern "C"
#endif

#endif ///< ___VP8ENC_H___
