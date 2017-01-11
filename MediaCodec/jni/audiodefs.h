#ifndef ___AUDIODEFS_H___
#define ___AUDIODEFS_H___

#ifdef __cplusplus
extern "C"
{
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct audio_enc_context;
struct audio_dec_context;

/* defines audio codec type */
typedef enum AUDIO_CODEC_TYPE
{
    CODEC_TYPE_NULL = 0,            ///< invalid codec
    CODEC_TYPE_OPUS = 1,            ///< OPUS codec
    CODEC_TYPE_AAC  = 2             ///< AAC codec
} AUDIO_CODEC_TYPE;

/* defines configuration for the opus encoder */
typedef struct audio_enc_config
{
    int       sample_rate;          ///< input sample rate
    int       channels;             ///< input audio channels
    int       bits_per_sample;      ///< bits per sample
    int       bitrate;              ///< bitrate in kbps
    int       enable_audio_level;   ///< enable calculation for audio level
} audio_enc_config;

/* defines an encoded audio frame */
typedef struct audio_enc_frame_bs
{
    unsigned char * data;           ///< audio frame
    int             data_len;       ///< frame length in bytes
    int             samples;        ///< samples in one channel of this frame
    int             audio_level;    ///< audio level
    long long       ntp;            ///< ntp of the first sample
    long long       opaque;         ///< user data of the frist sample
} audio_enc_frame_bs;

/* defines an decoded audio pcm block */
typedef struct audio_dec_pcm
{
    void      * pcm;                ///< pcm data
    int         len;                ///< data length in bytes
    int         sample_rate;        ///< sample rate
    int         channels;           ///< channels
    int         bits_per_sample;    ///< bits per sample
    int         duration_ms;        ///< duration in ms
    int         type;               ///< 0: speech, 1: cn
    long long   pts;                ///< pts
    long long   opaque;             ///< user data
} audio_dec_pcm;

/* encoded frame callback */
typedef void(* audio_enc_frame_cb)(struct audio_enc_context * enc, audio_enc_frame_bs * frame, void * arg);
/* decoded pcm callback */
typedef void(* audio_dec_pcm_cb)(struct audio_dec_context * dec, audio_dec_pcm * pcm, void * arg);

/* encoder log callback */
typedef void(* audio_enc_log_cb)(struct audio_enc_context * enc, const char * msg, void * cb_arg);
/* decoder log callback */
typedef void(* audio_dec_log_cb)(struct audio_dec_context * dec, const char * msg, void * cb_arg);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
}
#endif

#endif ///< ___AUDIODEFS_H___
