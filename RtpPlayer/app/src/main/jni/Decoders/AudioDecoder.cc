#include "Decoders/AudioDecoder.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

AudioDecoder::AudioDecoder() : AVDecoder("audio decoder"),
                               m_observer(NULL),
                               m_decoder(NULL)
{

}

AudioDecoder::~AudioDecoder()
{

}

bool
AudioDecoder::build(AUDIO_CODEC_TYPE codec, AVDecoderObserver * observer)
{
    m_decoder = create_audio_decoder(codec);
    if (!m_decoder || audio_dec_init(m_decoder) != 0)
    {
        RP_LOG_E("failed to create decoder for codec: %d.", codec);
        goto fail;
    }

    audio_dec_set_cb_log(m_decoder, dec_log_output, this);

    audio_dec_set_cb_output(m_decoder, dec_pcm_output, this);

    m_observer = observer;

    return true;

fail:

    tear();

    return false;
}

void
AudioDecoder::tear()
{
    if (m_decoder)
    {
        audio_dec_destroy(m_decoder);
        m_decoder = NULL;
    }
}

void
AudioDecoder::decode(MediaFrame * frame)
{
    audio_dec_decode(m_decoder, (char *)frame->data, frame->len, 0, frame->pts, frame->rtp_payload_type);
}

void
AudioDecoder::dec_pcm_output(audio_dec_context * decoder, audio_dec_pcm * pcm, void * cb_arg)
{
    AudioDecoder * audio_decoder = (AudioDecoder *)cb_arg;
    AVFrame      * pcm_frame     = NULL;

    if (audio_decoder && audio_decoder->m_observer)
    {
        pcm_frame = AVFrame::create_instance(pcm, decoder);

        audio_decoder->m_observer->on_decoder_output(audio_decoder, pcm_frame);

        pcm_frame->release();
    }
}

void
AudioDecoder::dec_log_output(audio_dec_context * decoder, const char * msg, void * cb_arg)
{
    RP_LOG_D("%s", msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
