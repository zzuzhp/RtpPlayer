#include "audioenc.h"
#include "opus.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define OPUS_SAMPLE_BITS     16             ///< 16-bits per sample
#define OPUS_MAX_PACKET      1500           ///< output buffer size
#define OPUS_PACKET_DURATION 20             ///< Opus can encode frames of 2.5, 5, 10, 20, 40, or 60 ms.
                                            ///< It can also combine multiple frames into packets of up to 120 ms.

typedef struct AudioFrame
{
    unsigned short    * frame;              ///< audio frame of OPUS_PACKET_DURATION
    int                 samples;            ///< samples in one channel
    long long           ntp;                ///< !!!ntp of the first sample
    long long           opaque;             ///< !!!user data of the first sample
    struct AudioFrame * next;
} AudioFrame;

typedef struct opus_enc_context
{
    audio_enc_context   * ctx;

    OpusEncoder         * encoder;          ///< encoder handler
    int                   dtx_mode;         ///< dtx mode indicator

    int                   frame_samples;    ///< samples per channel

    AudioFrame          * frames;           ///< frames to be encoded
    AudioFrame          * free_frames;      ///< free frame buffers
} opus_enc_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create an opus encoder
 * returns 0 for success, negative values indicate a failure
 */
int opus_enc_create(opus_enc_context ** ctx);

/*!\brief init the encoder
 * DTX & FEC is auto enabled, frame size is 20ms
 */
int opus_enc_init(audio_enc_context * ctx, audio_enc_config * config);

/*!\brief change target bitrate
 */
int opus_enc_update_bitrate(audio_enc_context * ctx, int bitrate_in_kbps);

/*!\brief encode one audio block
 * @audio_block: audio data
 * @ntp_in_ms: ntp of the last sample
 */
int opus_enc_encode(audio_enc_context * ctx,
                    const void        * data,
                    int                 data_len_in_bytes,
                    long long           ntp_in_ms,
                    long long           opaque);

/*!\brief destroy the encoder
 */
void opus_enc_destroy(audio_enc_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
} ///< extern "C"
#endif
