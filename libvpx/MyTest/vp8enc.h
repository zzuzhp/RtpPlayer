#ifndef ___VP8ENC_H___
#define ___VP8ENC_H___

#include "video_defs.h"
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* VPX_ENC_CONTROL_xxx flags */
#define VPX_ENC_CONTROL_KEY 0x1

#define VPX_ENC_INIT_COLLECT_PSNR   1
#define VPX_ENC_TOKEN_PARTITIONS    3   ///< 0 or 1 or 2 or 3 ///< multi-partition enables parallel entropy encoding/decoding
#define VPX_ENC_PARTITIONS          ((1 << VPX_ENC_TOKEN_PARTITIONS) + 1)
#define VPX_ENC_DUMP_LAYERS         0   ///< 0: no dump, 1: dump base layer, 2: dump base layer + first enhanced layer, 3: dump all 3 layers

/* we use a fixed 3 layer pattern */
#define VPX_ENC_3TL             3
#define VPX_ENC_3TL_IDS         4
#define VPX_ENC_3TL_PATTERN     8

/* defines a vp8 encoder context */
typedef struct vp8_enc_context
{
    int                   temporal_ids[VPX_ENC_3TL_IDS];
    int                   encode_flags[VPX_ENC_3TL_PATTERN];
    int                   key_frame_request;
    uint8_t               flag_idc;
    vpx_image_t         * image;  ///< image to be encoded
    video_enc_config      config;
    vpx_codec_ctx_t       encoder;

    void                * cb_arg;
    cb_enc_output         cb_out;

    void                * cb_arg_log;
    cb_enc_log            cb_out_log;

    video_enc_frame_bs  * out_frame;

    size_t                buffer_bs_size;
    uint8_t             * buffer_bs;

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
    struct vpx_file_dumper * bs_dumper;
#endif
} vp8_enc_context;

/*!\brief create a vp8 encoder
 * returns 0 for success, negative for failure
 */
int vp8_enc_create(vp8_enc_context ** ctx);

/*!\brief initialize a vp8 encoder
 * returns 0 for success, negative for failure
 */
int vp8_enc_init(vp8_enc_context * ctx, video_enc_config * cfg);

/*!\brief qurey the capabilities of an encoder
 * return the caps 
 */
int vp8_enc_get_caps(vp8_enc_context * ctx);

/*!\brief set a callback for the logs
*/
int vp8_enc_set_cb_log(vp8_enc_context * ctx, cb_enc_log cb_log, void * arg);

/*!\brief set a callback for the encoded frames
 * returns 0 for success, negative for failure
 */
int vp8_enc_set_cb_output(vp8_enc_context * ctx, cb_enc_output cb_frame, void * arg);

/*!\brief change the bitrates or(and) framerate
 * returns 0 for success, negative for failure
 */
int vp8_enc_set_rates(vp8_enc_context * ctx, int new_bitrate_kbit, int frame_rate);

/*!\brief encode an frame
 * returns 0 for success, negative for failure
 */
int vp8_enc_encode(vp8_enc_context * ctx, video_surface * surface, int flags);

/*!\brief destroy the encoder
 */
void vp8_enc_destroy(vp8_enc_context * ctx);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif ///< ___VP8ENC_H___
