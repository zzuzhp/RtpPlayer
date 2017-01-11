#include "Renders/AudioOut.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/* this callback handler is called every time a buffer finishes playing */
AudioOut::AudioOut() : AVOutBase(false, "audio renderer"),
                       m_engineObject(NULL),
                       m_engineEngine(NULL),
                       m_outputMixObject(NULL),
                       m_bqPlayerObject(NULL),
                       m_bqPlayerPlay(NULL),
                       m_bqPlayerBufferQueue(NULL),
                       m_channels(0),
                       m_sample_rate(0)
{

}

AudioOut::~AudioOut()
{
    stop();
    clear();
}

/* Open the audio device with a given sampling rate (sr), output channels and IO buffer size in frames.
 */
bool
AudioOut::init(int sample_rate, int channels)
{
    const int bufferframes = SAMPLE_BLOCK_SIZE_MS * sample_rate / 1000;

    if (!create_engine())
    {
        goto fail;
    }

    if (!open_device(sample_rate, channels, 16))
    {
        goto fail;
    }

    if ((m_outBufSamples = bufferframes * channels) != 0)
    {
        m_outputBuffer[0] = (short *) new short[m_outBufSamples];
        m_outputBuffer[1] = (short *) new short[m_outBufSamples];

        if (!m_outputBuffer[0] || !m_outputBuffer[1])
        {
            goto fail;
        }
    }

    m_render_time           = 0.0;
    m_channels              = channels;
    m_sample_rate           = sample_rate;
    m_currentOutputIndex    = 0;
    m_currentOutputBuffer   = 0;

    return true;

fail:

    clear();

    return false;
}

void
AudioOut::clear()
{
    /* destroy buffer queue audio player object, and invalidate all associated interfaces */
    if (m_bqPlayerObject != NULL)
    {
        (*m_bqPlayerObject)->Destroy(m_bqPlayerObject);

        m_bqPlayerObject        = NULL;
        m_bqPlayerPlay          = NULL;
        m_bqPlayerBufferQueue   = NULL;
    }

    /* destroy output mix object, and invalidate all associated interfaces */
    if (m_outputMixObject != NULL)
    {
        (*m_outputMixObject)->Destroy(m_outputMixObject);
        m_outputMixObject = NULL;
    }

    /* destroy engine object, and invalidate all associated interfaces */
    if (m_engineObject != NULL)
    {
        (*m_engineObject)->Destroy(m_engineObject);

        m_engineObject = NULL;
        m_engineEngine = NULL;
    }

    m_channels    = 0;
    m_sample_rate = 0;
}

void
AudioOut::on_frame(AVFrame * frame)
{
    /* puts a buffer of size samples to the device */
    if (frame->get_bits_per_sample() != 16)
    {
        RP_LOG_E("AudioOut only support sample bits 16!");
        return;
    }

    short * buffer  = (short *)frame->get_pcm();
    int     size    = frame->get_pcm_len() / (frame->get_bits_per_sample() >> 3);

    if (m_channels != frame->get_channels() || m_sample_rate != frame->get_sample_rate())
    {
        RP_LOG_W("AudioOut input format changed from ac:%d, ar:%d to ac:%d, ar:%d.",
                  m_channels, m_sample_rate, frame->get_channels(), frame->get_sample_rate());

        clear();

        if (!init(frame->get_sample_rate(), frame->get_channels()))
        {
            RP_LOG_E("AudioOut init failed!");
            return;
        }
    }

    int     index     = m_currentOutputIndex;
    short * outBuffer = m_outputBuffer[m_currentOutputBuffer];

    for (int i = 0; i < size; ++i)
    {
        outBuffer[index++] = (short)(buffer[i]);

        if (index >= m_outBufSamples)
        {
            (*m_bqPlayerBufferQueue)->Enqueue(m_bqPlayerBufferQueue, outBuffer, m_outBufSamples * sizeof(short));

            m_cond.Wait(NULL);

            m_currentOutputBuffer = (m_currentOutputBuffer ? 0 : 1);
            outBuffer = m_outputBuffer[m_currentOutputBuffer];
            index = 0;
        }
    }

    m_currentOutputIndex = index;
    m_render_time += (double) size / (m_sample_rate * m_channels);
}

bool
AudioOut::create_engine()
{
    /* creates the OpenSL ES audio engine */
    SLresult result = slCreateEngine(&m_engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("slCreateEngine failed: %d.", result);
        return false;
    }

    /* realize the engine */
    result = (*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("realize sl engine failed: %d.", result);
        return false;
    }

    /* get the engine interface, which is needed in order to create other objects */
    result = (*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engineEngine);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("get interface 'SL_IID_ENGINE' failed: %d.", result);
        return false;
    }

    return true;
}

/* opens the OpenSL ES device for output */
bool
AudioOut::open_device(int sample_rate, int channels, int bits_per_sample)
{
    SLresult result;
    SLuint32 sr   = 0;
    SLuint32 bits = 0;

    if (channels <= 0)
    {
        RP_LOG_E("invalid audio channels: %d.", channels);
        return false;
    }

    if (bits_per_sample != 8 && bits_per_sample != 16)
    {
        RP_LOG_E("only 8-bits/16-bits per sample is supported, bits per sample is: %d.", bits_per_sample);
        return false;
    }

    bits = bits_per_sample == 8 ? SL_PCMSAMPLEFORMAT_FIXED_8 : SL_PCMSAMPLEFORMAT_FIXED_16;

    /* configure the audio source */
    switch (sample_rate)
    {
    case 8000:
        sr = SL_SAMPLINGRATE_8;
        break;
    case 11025:
        sr = SL_SAMPLINGRATE_11_025;
        break;
    case 16000:
        sr = SL_SAMPLINGRATE_16;
        break;
    case 22050:
        sr = SL_SAMPLINGRATE_22_05;
        break;
    case 24000:
        sr = SL_SAMPLINGRATE_24;
        break;
    case 32000:
        sr = SL_SAMPLINGRATE_32;
        break;
    case 44100:
        sr = SL_SAMPLINGRATE_44_1;
        break;
    case 48000:
        sr = SL_SAMPLINGRATE_48;
        break;
    case 64000:
        sr = SL_SAMPLINGRATE_64;
        break;
    case 88200:
        sr = SL_SAMPLINGRATE_88_2;
        break;
    case 96000:
        sr = SL_SAMPLINGRATE_96;
        break;
    case 192000:
        sr = SL_SAMPLINGRATE_192;
        break;
    default:
        break;
    }

    if (sr == 0)
    {
        RP_LOG_E("invalid audio sample rate: %d.", sample_rate);
        return false;
    }

    const SLInterfaceID ids[] = {SL_IID_VOLUME};
    const SLboolean     req[] = {SL_BOOLEAN_FALSE};

    result = (*m_engineEngine)->CreateOutputMix(m_engineEngine, &m_outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("CreateOutputMix failed: %d.", result);
        return false;
    }

    /* realize the output mix */
    result = (*m_outputMixObject)->Realize(m_outputMixObject, SL_BOOLEAN_FALSE);

    int speakers;
    if (channels > 1)
    {
        speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    }
    else
    {
        speakers = SL_SPEAKER_FRONT_CENTER;
    }

    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm =
    {
        SL_DATAFORMAT_PCM,              ///< formatType
        channels,                       ///< numChannels
        sr,                             ///< samplesPerSec
        bits,                           ///< bitsPerSample
        bits,                           ///< containerSize
        speakers,                       ///< channelMask
        SL_BYTEORDER_LITTLEENDIAN       ///< endianness
    };

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    /* configure audio sink */
    SLDataLocator_OutputMix loc_outmix  = {SL_DATALOCATOR_OUTPUTMIX, m_outputMixObject};
    SLDataSink              audioSnk    = {&loc_outmix, NULL};

    /* create audio player */
    const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean     req1[] = {SL_BOOLEAN_TRUE};

    result = (*m_engineEngine)->CreateAudioPlayer(m_engineEngine, &m_bqPlayerObject, &audioSrc, &audioSnk, 1, ids1, req1);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("CreateAudioPlayer failed: %d.", result);
        return false;
    }

    /* realize the player */
    result = (*m_bqPlayerObject)->Realize(m_bqPlayerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("buffer queue realize failed: %d.", result);
        return false;
    }

    /* get the play interface */
    result = (*m_bqPlayerObject)->GetInterface(m_bqPlayerObject, SL_IID_PLAY, &m_bqPlayerPlay);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("buffer queue realize failed: %d.", result);
        return false;
    }

    /* get the buffer queue interface */
    result = (*m_bqPlayerObject)->GetInterface(m_bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_bqPlayerBufferQueue);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("get interface for 'SL_IID_ANDROIDSIMPLEBUFFERQUEUE' failed: %d.", result);
        return false;
    }

    /* register callback on the buffer queue */
    result = (*m_bqPlayerBufferQueue)->RegisterCallback(m_bqPlayerBufferQueue, on_buffer_finished, this);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("RegisterCallback failed: %d.", result);
        return false;
    }

    /* set the player's state to playing */
    result = (*m_bqPlayerPlay)->SetPlayState(m_bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS)
    {
        RP_LOG_E("AudioOut SetPlayState failed: %d.", result);
        return false;
    }

    return true;
}

void
AudioOut::buffer_available()
{
    m_cond.Signal();
}

void
AudioOut::on_buffer_finished(SLAndroidSimpleBufferQueueItf bq, void * context)
{
    AudioOut * inst = (AudioOut *)context;
    inst->buffer_available();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
