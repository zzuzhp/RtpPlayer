#include "audiodec.h"
#include "opus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPUS_FULLBAND           48000       ///< max sample rate supported by opus
#define OPUS_SAMPLE_PRECISION   16          ///< bits per sample of input/output pcm samples for opus
#define OPUS_SAMPLE_LENGTH      (OPUS_SAMPLE_PRECISION >> 3)    ///< pcm sample size in bytes
#define OPUS_PLC_TIME_UNIT_NUM  5           ///< for the PLC and FEC cases, frame size MUST be a multiple of 2.5ms
#define OPUS_PLC_TIME_UNIT_DEN  2

typedef short OPUS_SAMPLE;                  ///< opus use fixed 16 bit sample

////////////////////////////////////////////////////////////////////////////////////////////////////
////

typedef struct opus_dec_context
{
    audio_dec_context   * ctx;              ///< parent context

    int                   sample_rate;      ///< out pcm sample rate
    int                   channels;         ///< out pcm channels

    int                   max_frame_size;
    OPUS_SAMPLE         * pcm;              ///< out pcm buffer

    long long             last_pts;         ///< for lost packet duration deduction
    OpusDecoder         * decoder;          ///< decoder handler
} opus_dec_context;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*!\brief create an opus decoder
 * returns 0 for success, negative values indicate a failure
 */
int opus_dec_create(opus_dec_context ** ctx);

/*!\brief init the decoder
 * @channels: output pcm channels
 * output pcm sample rate is 48khz
 */
int opus_dec_init(audio_dec_context * ctx);

/*!\brief decodes an opus packet
 */
int opus_dec_decode(audio_dec_context * ctx,
                    const char        * packet,
                    int                 packet_len,
                    int                 flags,
                    long long           pts,
                    long long           opaque);

/*!\brief destroy the decoder
 */
void opus_dec_destroy(audio_dec_context * ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#ifdef __cplusplus
} ///< extern "C"
#endif
