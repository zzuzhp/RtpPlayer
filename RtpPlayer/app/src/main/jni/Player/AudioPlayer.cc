#include "AudioPlayer.h"
#include "Streams/AudioStream.h"
#include "Decoders/AudioDecoder.h"
#include "Renders/AudioOut.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AudioPlayer::AudioPlayer() : BasePlayer("AudioPlayer")
{

}

AudioPlayer::~AudioPlayer()
{
    tear();
}

bool
AudioPlayer::build (AVSync * sync, RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    {
        CProThreadMutexGuard mon(m_lock);

        AUDIO_CODEC_TYPE codec;
        if (media == RTP_MEDIA_AUDIO_AAC)
        {
            codec = CODEC_TYPE_AAC;
        }
        else if (media == RTP_MEDIA_AUDIO_OPUS)
        {
            codec = CODEC_TYPE_OPUS;
        }
        else
        {
            RP_LOG_E("play_audio failed for invalid media: %d!", media);
            return false;
        }

        m_renderer = AudioOut::create_instance();
        if (!m_renderer)
        {
            RP_LOG_E("audio out create failed!");
            goto fail;
        }

        m_renderer->set_source(sync);
        if (!m_renderer->start())
        {
            RP_LOG_E("audio out start failed!");
            goto fail;
        }

        m_decoder = AudioDecoder::create_instance();
        if (!m_decoder || !((AudioDecoder *)m_decoder)->build(codec, this))
        {
            RP_LOG_E("failed to create decoder for codec: %d.", codec);
            goto fail;
        }

        m_stream = AudioStream::create_instance(this);
        if (!m_stream)
        {
            RP_LOG_E("stream %d receiver create failed.", rtp_payload_type);
            goto fail;
        }

        if (!((AudioStream *)m_stream)->set_audio_media(media, rtp_payload_type, port))
        {
            RP_LOG_E("stream %d set_video_media failed.", rtp_payload_type);
            goto fail;
        }

        m_sample_rate     = 0;
        m_channels        = 0;
        m_bits_per_sample = 0;

        sync->add_ref();
        m_av_sync = sync;
    }

    return true;

fail:

    tear();

    return false;
}

void
AudioPlayer::tear()
{
    clear();

    m_sample_rate     = 0;
    m_channels        = 0;
    m_bits_per_sample = 0;
}

bool
AudioPlayer::audio_info(int     & sample_rate,
                        int     & channels,
                        int     & bits_per_sample,
                        int     & bitrate_kbps,
                        int     & losses,
                        float   & lossrate)
{
    CProThreadMutexGuard mon(m_lock);

    AudioStream * stream = (AudioStream *)m_stream;
    if (!stream)
    {
        RP_LOG_E("can not get audio info, stream not exist.");
        return false;
    }

    stream->audio_info(bitrate_kbps, losses, lossrate);

    sample_rate     = m_sample_rate;
    channels        = m_channels;
    bits_per_sample = m_bits_per_sample;
}

void
AudioPlayer::on_decoded_pcm (AVFrame * frame)
{
    AVSync * av_sync = NULL;

    if (frame->get_sample_rate() <= 0 || frame->get_channels() <= 0 || frame->get_bits_per_sample() <= 0)
    {
        RP_LOG_E("invalid frame from audio decoder callback, ar: %d, ac: %d, bits: %d.",
                  frame->get_sample_rate(), frame->get_channels(), frame->get_bits_per_sample());
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_sample_rate == 0 || m_channels == 0 || m_bits_per_sample == 0)
        {
            m_start_ts = frame->get_pts();
            RP_LOG_I("received first audio pts: %u, (video buffering: %dms)", frame->get_pts(), m_av_sync->video_latency_ms());
        }

        m_sample_rate     = frame->get_sample_rate();
        m_channels        = frame->get_channels();
        m_bits_per_sample = frame->get_bits_per_sample();

        if (m_av_sync)
        {
            m_av_sync->add_ref();
            av_sync = m_av_sync;
        }
    }

    if (av_sync)
    {
        av_sync->push_audio(frame);
        av_sync->release();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
