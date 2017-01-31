#include "BasePlayer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

BasePlayer::BasePlayer(const char * name, int evt_mask) : AVModule(AV_MODULE_MODERATOR, name, evt_mask),
                                                          m_stream(NULL),
                                                          m_decoder(NULL),
                                                          m_renderer(NULL),
                                                          m_av_sync(NULL)
{

}

BasePlayer::~BasePlayer()
{
    clear();
}

int
BasePlayer::play_time_ms()
{
    CProThreadMutexGuard mon(m_lock);

    if (m_renderer)
    {
        return (int)(m_renderer->render_time() * 1000);
    }

    return 0;
}

void
BasePlayer::clear()
{
    RtpStream   * stream   = NULL;
    AVDecoder   * decoder  = NULL;
    AVOut       * renderer = NULL;
    AVSync      * sync     = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        stream     = m_stream;
        m_stream   = NULL;

        decoder    = m_decoder;
        m_decoder  = NULL;

        renderer   = m_renderer;
        m_renderer = NULL;

        sync       = m_av_sync;
        m_av_sync  = NULL;
    }

    if (stream)
    {
        RP_FOOTPRINT
        stream->release();
        RP_FOOTPRINT
    }

    if (decoder)
    {
        RP_FOOTPRINT
        decoder->release();
        RP_FOOTPRINT
    }

    if (renderer)
    {
        RP_FOOTPRINT
        renderer->stop();
        RP_FOOTPRINT
        renderer->release();
        RP_FOOTPRINT
    }

    if (sync)
    {
        RP_FOOTPRINT
        sync->release();
        RP_FOOTPRINT
    }
}

void
BasePlayer::on_media_frame(RtpStream * stream, MediaFrame * frame)
{
    AVDecoder * decoder = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_decoder)
        {
            m_decoder->add_ref();
            decoder = m_decoder;
        }
    }

    if (decoder)
    {
        decoder->decode(frame);
        decoder->release();
    }
}

void
BasePlayer::on_decoder_output(AVDecoder * decoder, AVFrame * frame)
{
    if (frame)
    {
        if (frame->is_video())
        {
            on_decoded_image(frame);
        }
        else
        {
            on_decoded_pcm(frame);
        }
    }
}
