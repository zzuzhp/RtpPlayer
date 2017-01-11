#ifndef ___AUDIODECODER_H___
#define ___AUDIODECODER_H___

#include "audiodec.h"
#include "Decoders/AVDecoder.h"
#include "Common/AVFrame.h"

#include <new>

class AudioDecoder;
class AudioDecoderObserver
{
public:
    virtual void on_decoded_pcm (AudioDecoder * decoder, AVFrame * pcm_frame) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AudioDecoder : public AVDecoder
{
public:

    static AudioDecoder * create_instance()
    {
        return new (std::nothrow)AudioDecoder();
    }

    bool build(AUDIO_CODEC_TYPE codec, AudioDecoderObserver * observer);

    void tear();

    void decode(MediaFrame * frame);

private:

    AudioDecoder();

    ~AudioDecoder();

    static void dec_pcm_output(audio_dec_context * dec, audio_dec_pcm * pcm, void * arg);

    static void dec_log_output(audio_dec_context * dec, const char * msg, void * cb_arg);

private:

    AudioDecoderObserver * m_observer;

    audio_dec_context    * m_decoder;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AUDIODECODER_H___
