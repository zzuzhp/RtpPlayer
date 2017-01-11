#include "opusenc.h"
#include "common.h"

#include "opus_defines.h"
#include "os_support.h"

#include <math.h>

static int
get_sample_size(audio_enc_config * config, int samples)
{
    /* storage requirments for samples */
    return (config->bits_per_sample >> 3) * samples * config->channels;
}

static int
get_sample_count(audio_enc_config * config, int data_len_in_bytes)
{
    /* get samples in one channel */
    ///< assert(data_len_in_bytes % (config->bits_per_sample >> 3) == 0);
    return data_len_in_bytes / (config->bits_per_sample >> 3) / config->channels;
}

static int
get_sample_duration(audio_enc_config * config, int samples)
{
    /* get samples duration in ms */
    return samples * 1000 / config->sample_rate;
}

/* calc sample duration and returns an offset biased on ntp */
static long long
get_sample_ntp(audio_enc_config * config, int samples, long long ntp)
{
    return ntp + get_sample_duration(config, samples);
}

/* Computes the RMS level over all samples, the returned value is positive but should be interpreted as
 * negative as per the RFC. It is constrained to [0, 127].
 */
static int
get_audio_level(const short * samples, int num_samples)
{
    static const float max_squared_value_short = 32768 * 32768;
    static const int min_level = 127;

    float sum_square = 0;
    int   sample_count = 0;

    int i = 0;
    for (i = 0; i < num_samples; ++i)
    {
        sum_square += samples[i] * samples[i];
    }

    sample_count += num_samples;

    /* Normalize by the max level. */
    float rms = sum_square / (sample_count * max_squared_value_short);
    /* 20log_10(x^0.5) = 10log_10(x) */
    rms = 10 * log10(rms);

    if (rms < -min_level)
    {
        rms = -min_level;
    }

    rms = -rms;

    return (int)(rms + 0.5);
}

/* get one audio frame to write samples in(generate one if necessary) */
static AudioFrame *
get_writable_frame(audio_enc_context * ctx)
{
    AudioFrame * frame      = 0;
    AudioFrame * frame_tail = 0;

    opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

    for (frame_tail = opus_ctx->frames; frame_tail && frame_tail->next; frame_tail = frame_tail->next);

    if (frame_tail && frame_tail->samples < opus_ctx->frame_samples)
    {
        /* frame still not full */
        return frame_tail;
    }

    /* return a new empty frame */
    if (opus_ctx->free_frames)
    {
        /* there are free frames available */
        frame = opus_ctx->free_frames;
        opus_ctx->free_frames = opus_ctx->free_frames->next;
    }
    else
    {
        /* no free frames available: alloc one */
        int size = get_sample_size(&ctx->config, opus_ctx->frame_samples);

        int alloc_size = sizeof(AudioFrame) + size;

        frame = (AudioFrame *)opus_alloc(alloc_size);
        if (!frame)
        {
            return 0;
        }

        frame->frame = (short *)((char *)frame + sizeof(AudioFrame));
    }

    frame->samples  = 0;
    frame->ntp      = -1;
    frame->next     = 0;

    if (!opus_ctx->frames)
    {
        opus_ctx->frames = frame;
    }
    else
    {
        frame_tail->next = frame;
    }

    return frame;
}

/* mark the frame as free */
static void
release_audio_frame(opus_enc_context * ctx, AudioFrame * frame)
{
    if (frame)
    {
        frame->next = ctx->free_frames;
        ctx->free_frames = frame;
    }
}

/* free the frame buffer */
static void
free_audio_frame(AudioFrame * frame)
{
    free((void *)frame);
}

static void
write_audio_frame(audio_enc_context  * ctx,
                  const void         * audio,
                  int                  len,
                  long long            ntp_in_ms,
                  long long            opaque)
{
    unsigned char * samples = (unsigned char *)audio;

    opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

    /* sample size in bytes in both channels if stereo */
    int sample_size = get_sample_size(&ctx->config, 1);

    /* buffer should contain intact samples */
    ///< assert(len % sample_size == 0);
    len = len / sample_size * sample_size;

    int samples_left = get_sample_count(&ctx->config, len);
    long long first_sample_ntp = ntp_in_ms - get_sample_duration(&ctx->config, samples_left);

    while (samples_left > 0)
    {
        AudioFrame * frame = get_writable_frame(ctx);
        if (!frame)
        {
            break;
        }

        int remaining_samples = 0;
        if ((remaining_samples = opus_ctx->frame_samples - frame->samples) > 0)
        {
            int cpy_samples  = remaining_samples > samples_left ? samples_left : remaining_samples;
            int len_in_bytes = cpy_samples * sample_size;

            memcpy((char *)frame->frame + frame->samples * sample_size, samples, len_in_bytes);

            if (frame->samples == 0)
            {
                int samples_offset = get_sample_count(&ctx->config, (int)((char *)samples - (char *)audio));

                frame->ntp    = get_sample_ntp(&ctx->config, samples_offset, first_sample_ntp);
                frame->opaque = opaque;
            }

            frame->samples += cpy_samples;
            samples_left -= cpy_samples;
            samples += len_in_bytes;
        }
    }
}

int 
opus_enc_create(opus_enc_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    *ctx = (opus_enc_context *)opus_alloc(sizeof(opus_enc_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(opus_enc_context));

    return 0;
}

int 
opus_enc_init(audio_enc_context * ctx, audio_enc_config * config)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

    int opus_app = OPUS_APPLICATION_VOIP;

    if (config->bits_per_sample != OPUS_SAMPLE_BITS ||
       (config->channels != 1 && config->channels != 2) ||
       (config->sample_rate != 8000 && config->sample_rate != 12000 && config->sample_rate != 16000 && config->sample_rate != 24000 && config->sample_rate != 48000))
    {
        /* invalid arguments */
        audio_enc_log(ctx, "opus_enc_init failed, wrong param!");
        return -1;
    }

    int error;
    opus_ctx->encoder = opus_encoder_create(config->sample_rate, (int)config->channels, opus_app, &error);
    if (error != OPUS_OK || !opus_ctx->encoder)
    {
        audio_enc_log(ctx, "opus_encoder_create failed!");
        opus_enc_destroy(ctx);

        return -1;
    }

    opus_encoder_ctl(opus_ctx->encoder, OPUS_SET_LSB_DEPTH(16));
    opus_encoder_ctl(opus_ctx->encoder, OPUS_SET_BITRATE(config->bitrate * 1000));

    opus_ctx->dtx_mode = 0;

    opus_ctx->frame_samples = config->sample_rate / 1000 * OPUS_PACKET_DURATION;
    opus_ctx->frames        = 0;
    opus_ctx->free_frames   = 0;

    ctx->config = *config;

    return 0;
}

int 
opus_enc_update_bitrate(audio_enc_context * ctx, int bitrate_in_kbps)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

    return opus_encoder_ctl(opus_ctx->encoder, OPUS_SET_BITRATE(bitrate_in_kbps * 1000));
}

int 
opus_enc_encode(audio_enc_context * ctx,
                const void        * data,
                int                 data_len_in_bytes,
                long long           ntp_in_ms,
                long long           opaque)
{
    FUNC_CHECK_NONZERO_PARAM(ctx && data && data_len_in_bytes > 0, -1);

    opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

    AudioFrame * frame = 0;
    unsigned char encoded[OPUS_MAX_PACKET + 257];

    /* write audio frame */
    write_audio_frame(ctx, data, data_len_in_bytes, ntp_in_ms, opaque);

    /* encode any frames ready to encode */
    while (frame = opus_ctx->frames, frame && frame->samples == opus_ctx->frame_samples)
    {
        int samples_encoded = 0;

        /* encode one frame */
        int num_bytes = opus_encode(opus_ctx->encoder, frame->frame, frame->samples, encoded, OPUS_MAX_PACKET);

        if (num_bytes > 0)
        {
            /* The Opus encoder produces "packets", a single packet may contain multiple audio frames, so long as they share a
             * common set of parameters, including the operating mode, audio bandwidth, frame size, and channel count (mono vs. stereo).
             */
            samples_encoded = opus_packet_get_samples_per_frame(encoded, ctx->config.sample_rate) * opus_packet_get_nb_frames(encoded, num_bytes);
            ///< assert(samples_encoded == frame_samples);

            if (num_bytes <= 2)
            {
                /* Indicates DTX since the packet has nothing but a header. In principle,
                 * there is no need to send this packet.
                 */
                opus_ctx->dtx_mode = 1;
            }

            /* transmit this frame (including the frame in DTX mode) */
            if (num_bytes <= OPUS_MAX_PACKET)
            {
                if (ctx->cb_out)
                {
                    audio_enc_frame_bs frame_bs;

                    frame_bs.data        = encoded;
                    frame_bs.data_len    = num_bytes;
                    frame_bs.samples     = samples_encoded;
                    frame_bs.opaque      = frame->opaque;
                    frame_bs.ntp         = frame->ntp;
                    frame_bs.audio_level = get_audio_level(frame->frame, samples_encoded * ctx->config.channels);

                    ctx->cb_out(ctx, &frame_bs, ctx->cb_arg);
#if 0
                    /* write encoded stream */
                    static FILE * file = 0;
                    if (file == 0)
                    {
                        file = fopen("out.opus", "wb");
                    }

#define INT2CHAR(i, c) \
                    { \
                        c[0] = i >> 24; \
                        c[1] = (i >> 16) & 0xFF; \
                        c[2] = (i >> 8) & 0xFF; \
                        c[3] = i & 0xFF; \
                    }

                    unsigned char int_field[4];

                    /* four bytes frame length: Big endian */
                    INT2CHAR(num_bytes, int_field);
                    int write_bytes = fwrite(int_field, 1, 4, file);
#if 0
                    opus_uint32 enc_state;
                    opus_encoder_ctl(opus_ctx->encoder, OPUS_GET_FINAL_RANGE(&enc_state));

                    INT2CHAR(enc_state, int_field);
                    write_bytes += fwrite(int_field, 1, 4, file);
#endif
                    /* followed by the frame */
                    write_bytes += fwrite(encoded, 1, num_bytes, file);

                    ///< audio_enc_log(ctx, "opus_enc_encode write %d bytes!", write_bytes);
                    fflush(file);
#endif
                }
            }
        }

        /* shift out the frame */
        opus_ctx->frames = opus_ctx->frames->next;
        /* return the frame to the free list */
        release_audio_frame(opus_ctx, frame);

        opus_ctx->dtx_mode = 0;
    }

    return 0;
}

void 
opus_enc_destroy(audio_enc_context * ctx)
{
    audio_enc_log(ctx, "opus_enc_destroy.");

    if (ctx)
    {
        opus_enc_context * opus_ctx = (opus_enc_context *)ctx->priv_ctx;

        opus_encoder_destroy(opus_ctx->encoder);

        AudioFrame * frame = 0;
        for (frame = opus_ctx->frames; frame; frame = frame->next)
        {
            free_audio_frame(frame);
        }

        for (frame = opus_ctx->free_frames; frame; frame = frame->next)
        {
            free_audio_frame(frame);
        }

        opus_free(opus_ctx);
    }
}
