#include "opusdec.h"
#include "common.h"

#include "os_support.h"
#include "opus_defines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int
opus_dec_create(opus_dec_context ** ctx)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);

    *ctx = (opus_dec_context *)opus_alloc(sizeof(opus_dec_context));
    if (!*ctx)
    {
        return -1;
    }

    memset(*ctx, 0, sizeof(opus_dec_context));

    return 0;
}

int
opus_dec_init(audio_dec_context * ctx)
{
    int error;
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);
    opus_dec_context * opus_ctx = (opus_dec_context *)ctx->priv_ctx;

    opus_ctx->sample_rate       = OPUS_FULLBAND;
    opus_ctx->channels          = 2;
    opus_ctx->max_frame_size    = OPUS_FULLBAND * OPUS_SAMPLE_LENGTH;

    opus_ctx->pcm = (OPUS_SAMPLE *)opus_alloc(opus_ctx->max_frame_size * opus_ctx->channels * OPUS_SAMPLE_LENGTH);
    if (!opus_ctx->pcm)
    {
        audio_dec_log(ctx, "cannot alloc for opus output frame of size %d bytes.", opus_ctx->max_frame_size);
        return -1;
    }

    /* output pcm at 48khz/2channels */
    opus_ctx->decoder = opus_decoder_create(opus_ctx->sample_rate, opus_ctx->channels, &error);
    if (error != OPUS_OK)
    {
        audio_dec_log(ctx, "cannot create opus decoder: %s", opus_strerror(error));
        return -1;
    }

    return 0;
}

int
opus_dec_decode(audio_dec_context * ctx,
                const char        * packet,
                int                 packet_len,
                int                 flags,
                long long           pts,
                long long           opaque)
{
    FUNC_CHECK_NONZERO_PARAM(ctx, -1);
    opus_dec_context * opus_ctx = (opus_dec_context *)ctx->priv_ctx;

    int lost = 0;
    int samples = opus_ctx->max_frame_size; ///< number of samples per channel of available space in 'opus_ctx->pcm'

    if (lost)
    {
        /* PLC */
        int lost_duration = pts - opus_ctx->last_pts;
        int lost_samples = lost_duration * OPUS_PLC_TIME_UNIT_DEN / OPUS_PLC_TIME_UNIT_NUM * opus_ctx->sample_rate;

        if ((lost_duration * OPUS_PLC_TIME_UNIT_DEN) % OPUS_PLC_TIME_UNIT_NUM)
        {
            audio_dec_log(ctx, "opus lost packet(s) duration: %d(ms) not dividable by 2.5", lost_duration);
        }

        lost_samples = MEDIACODEC_MIN(lost_samples, opus_ctx->max_frame_size);

        samples = opus_decode(opus_ctx->decoder, 0, 0, opus_ctx->pcm, lost_samples, 0);
    }
    else
    {
        /* normal case */
        samples = opus_decode(opus_ctx->decoder, packet, packet_len, opus_ctx->pcm, samples, 0);
    }

    if (samples <= 0)
    {
        audio_dec_log(ctx, "error decoding opus packet: %d.", samples);
        return -1;
    }

    opus_ctx->last_pts = pts;

    /* send the frame */
    if (ctx->cb_out)
    {
        audio_dec_pcm pcm;

        pcm.pcm             = opus_ctx->pcm;
        pcm.len             = samples * opus_ctx->channels * OPUS_SAMPLE_LENGTH;
        pcm.sample_rate     = opus_ctx->sample_rate;
        pcm.bits_per_sample = OPUS_SAMPLE_PRECISION;
        pcm.channels        = opus_ctx->channels;
        pcm.duration_ms     = samples * 1000 / opus_ctx->sample_rate;
        pcm.type            = 0;
        pcm.pts             = pts;
        pcm.opaque          = opaque;

        ctx->cb_out(ctx, &pcm, ctx->cb_arg);
    }

    return 0;
}

void
opus_dec_destroy(audio_dec_context * ctx)
{
    if (ctx)
    {
        opus_dec_context * opus_ctx = (opus_dec_context *)ctx->priv_ctx;

        opus_decoder_destroy(opus_ctx->decoder);

        opus_free(opus_ctx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
