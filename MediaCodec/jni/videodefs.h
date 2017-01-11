#ifndef ___VIDEO_DEFS_H___
#define ___VIDEO_DEFS_H___

#ifdef __cplusplus
extern "C"
{
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct video_enc_context;
struct video_dec_context;

/* defines codec type */
typedef enum VIDEO_CODEC_TYPE
{
    CODEC_TYPE_NUL    = 0,          ///< invalid codec
    CODEC_TYPE_VP8    = 1,          ///< VP8 codec
    CODEC_TYPE_AVC    = 2,          ///< AVC codec
} VIDEO_CODEC_TYPE;

/* tunes the encoder for our preferences: can be ENC_MODE_DEF or a combination of others */
typedef enum VIDEO_ENC_MODE
{
    ENC_MODE_DEF         = 0x0000,  ///< default
    ENC_MODE_FAST_DEC    = 0x0001,  ///< favors fast decode
    ENC_MODE_LOW_LATENCY = 0x0002   ///< low latency
} VIDEO_ENC_MODE;

/* defines color formats */
typedef enum IMG_COLOR_FORMAT
{
    IMG_FMT_NULL      = 0,          ///< invalid format
    IMG_FMT_I420      = 1,          ///< codec prefered format
    IMG_FMT_NV12      = 2,          ///< biplanar format with u/v interleaved
    IMG_FMT_NV21      = 3,          ///< same as NV12 with u/v reversed
    IMG_FMT_YUY2      = 4,          ///< YUYV packed YUV
    IMG_FMT_UYVY      = 5,          ///< UYVY packed YUV
    IMG_FMT_RGB24     = 6,          ///< 24 bit per pixel packed RGB
    IMG_FMT_RGB32     = 7           ///< 32 bit per pixel packed 0RGB
} IMG_COLOR_FORMAT;

/* defines any extra surface properties */
typedef struct video_surface_prop
{
    long long          ntp;         ///< ntp
    int                rotation;    ///< rotation degrees
    int                orientation; ///< 0: portrait(e.g. front camera), 1: landscape(e.g. back camera)
} video_surface_prop;

/* defines an image */
typedef struct video_surface
{
    IMG_COLOR_FORMAT     color;     ///< color primitives
    unsigned char      * planes[4]; ///< plane data
    unsigned int         width[4];  ///< plane width in pixels
    unsigned int         height[4]; ///< plane height in pixels
    unsigned int         pitch[4];  ///< plane stride in bytes
    long long            pts;       ///< presentation time stamp in timebase units
    video_surface_prop   prop;      ///< surface extra properties
    long long            opaque;    ///< user data

    void               * priv;      ///< do NOT access this
} video_surface;

/* defines parameters needed to initialized an encoder */
typedef struct video_enc_config
{
    int     width;                  ///< input and output image size
    int     height;

    int     bitrate;                ///< target bitrate in kbps
    int     strict_bitrate;         ///< if set, drop frames if target bitrate is too low.
    int     key_interval;           ///< key frame intervals in frames

    int     framerate_num;          ///< unused in vp8: we use timestamp instead
    int     framerate_den;

    int     timebase_num;           ///< time units of your timestamp
    int     timebase_den;

    int     enable_spatial_layers;  ///< not supported for vp8
    int     enable_temporal_layers; ///< 3 temporal layers

    int     max_threads;            ///< max num threads
                                    ///< vp8 specific: VP8 supports a mechanism whereby rows of macro-blocks can be simultaneously encoded on different threads.
                                    ///< However, the entropy encoding stage is limited to 1 thread unless multi-partitions is enabled, which will enable parallel encoding as well as decoding.
                                    ///< It is worth noting that if the threads number is set to > 1 then the results of repeat encodes will not always be exactly the same.

    VIDEO_ENC_MODE mode;            ///< encode mode
} video_enc_config;

/* defines data partitions for an encoded frame */
typedef struct video_enc_data_unit_bs
{
    unsigned char * data;           ///< pointer to the start address (NALU for H.264, partitions for VP8)
    int             len;            ///< data length in bytes(including any startcode)
    int             start_code_len; ///< start code length if any
    int             reserved;       ///< reserved 4 bytes
    int             flags;          ///< data unit flag: see VID_ENC_UNIT_xxx
} video_enc_data_unit_bs;

/* defines vp8 specific data */
typedef struct video_enc_priv_info_VP8
{
    unsigned short  picture_id;     ///< running index for the frames (wrapped 15-bits)
    char            key_idx;        ///< running index for key frames (not necessarily increased on every key frame)
    int             tl0_pic_idx;    ///< running index for the temporal base layer frames (indicates on which base-layer frame the current frame(other than the base layer frame) depends)
    unsigned char   temporal_idx;   ///< temporal-layer index (2-bits)
    int             layer_sync;     ///< layer sync frame (indicates whether this frame depends only on the base layer frame)
    int             non_reference;  ///< non-reference frame indicator
} video_enc_priv_info_VP8;

/* defines AVC specific data */
typedef struct video_enc_priv_info_AVC
{
    int             tl_idx;          ///< temporal layer
    int             non_reference;   ///< non-reference frame indicator
} video_enc_priv_info_AVC;

/* defines codec specific data */
typedef union video_enc_codec_priv_info
{
    video_enc_priv_info_VP8 vp8;    ///< VP8 codec specifics
    video_enc_priv_info_AVC avc;    ///< AVC codec specifics
} video_enc_codec_priv_info;

/* defines an encoded frame */
typedef struct video_enc_frame_bs
{
    int                       num_units;    ///< num units in this frame
    video_enc_data_unit_bs  * units;        ///< units

    unsigned char           * data;         ///< bitstream
    int                       len;          ///< bitstream length in bytes

    long long                 pts;          ///< pts
    int                       flags;        ///< combinations of VIDEO_FRAME_*
    double                    psnr;         ///< frame quality: ~30 (0 if not supported)
    video_surface_prop        prop;         ///< surface extra properties
    long long                 opaque;       ///< 'opaque' in the video_surface

    int                       codec;        ///< codec
    video_enc_codec_priv_info codec_priv;   ///< codec-private data

    void                    * priv;         ///< do NOT access this
} video_enc_frame_bs;

/* encoded frame callback */
typedef void(* video_enc_frame_cb)(struct video_enc_context * enc, video_enc_frame_bs * frame, void * cb_arg);
/* decoded frame callback */
typedef void(* video_dec_image_cb)(struct video_dec_context * dec, video_surface * image, void * cb_arg);

/* encoder log callback */
typedef void(* video_enc_log_cb)(struct video_enc_context * enc, const char * msg, void * cb_arg);
/* decoder log callback */
typedef void(* video_dec_log_cb)(struct video_dec_context * dec, const char * msg, void * cb_arg);

#define VIDEO_FRAME_TYPE_KEY  0x01          ///< key frame

#define VIDEO_ENC_CAP_SCALE   0x0001        ///< supports scale
#define VIDEO_ENC_CAP_CS      0X0002        ///< supports color convert
#define VIDEO_ENC_CAP_MT      0x0004        ///< supports multi-thread encoding
#define VIDEO_ENC_CAP_FLUSH   0x0008        ///< encoder needs flush

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}
#endif

#endif ///< ___VIDEO_DEFS_H___
