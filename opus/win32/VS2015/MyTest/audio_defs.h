#ifndef ___AUDIODEFS_H___
#define ___AUDIODEFS_H___

/* defines audio codec type */
typedef enum AUDIO_CODEC_TYPE
{
    CODEC_TYPE_NULL = 0,          ///< invalid codec
    CODEC_TYPE_OPUS = 1           ///< OPUS codec
} AUDIO_CODEC_TYPE;

/* defines configuration for the opus encoder */
typedef struct audio_enc_config
{
    int       sample_rate;        ///< input sample rate
    int       channels;           ///< input audio channels
    int       bits_per_sample;    ///< bits per sample
    int       bitrate;            ///< bitrate in kbps
    int       enable_audio_level; ///< enable calculation for audio level
} audio_enc_config;

/* defines an encoded audio frame */
typedef struct audio_enc_frame_bs
{
    unsigned char   * data;       ///< audio frame
    int               data_len;   ///< frame length in bytes
    int               samples;    ///< samples in one channel of this frame
    int               audio_level;///< audio level
    long long         ntp;        ///< ntp of the first sample
    long long         opaque;     ///< user data of the frist sample
} audio_enc_frame_bs;

/* encoded frame callback */
typedef void(*cb_enc_output)(void * enc, audio_enc_frame_bs * frame, void * arg);

/* log callback */
typedef void(*cb_enc_log)(void * enc, const char * msg, void * cb_arg);

#endif ///< ___AUDIODEFS_H___
