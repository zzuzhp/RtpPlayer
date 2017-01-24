#include "Decoders/VideoDecoder.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

VideoDecoder::VideoDecoder() : AVDecoder("video decoder"),
                               m_decoder(NULL),
                               m_observer(NULL)
{

}

VideoDecoder::~VideoDecoder()
{
    tear();
}

bool
VideoDecoder::build(VIDEO_CODEC_TYPE codec, AVDecoderObserver * observer)
{
    m_decoder = create_video_decoder(codec);
    if (!m_decoder || video_dec_init(m_decoder, 2) != 0)
    {
        RP_LOG_E("failed to create decoder for codec: %d.", codec);
        goto fail;
    }

    video_dec_set_cb_log(m_decoder, dec_log_output, this);

    video_dec_set_cb_output(m_decoder, dec_image_output, this);

    m_observer = observer;

    return true;

fail:

    tear();

    return false;
}

void
VideoDecoder::tear()
{
    if (m_decoder)
    {
        video_dec_destroy(m_decoder);
        m_decoder = NULL;
    }
}

void
VideoDecoder::decode(MediaFrame * frame)
{
    video_dec_decode(m_decoder, frame->data, frame->len, 0, frame->pts, frame->rtp_payload_type);
}

void
VideoDecoder::dec_image_output(video_dec_context * decoder, video_surface * image, void * cb_arg)
{
    VideoDecoder * video_decoder = (VideoDecoder *)cb_arg;
    AVFrame      * video_image   = NULL;

    /* sanity check */
    if (!image || !image->planes[0])
    {
        RP_LOG_E("decoder is delivering an invalid image!");
        return;
    }

    if (video_decoder && video_decoder->m_observer)
    {
        video_image = AVFrame::create_instance(image, decoder);

        video_decoder->m_observer->on_decoder_output(video_decoder, video_image);

        video_image->release();
    }
}

void
VideoDecoder::dec_log_output(video_dec_context * decoder, const char * msg, void * cb_arg)
{
    RP_LOG_D("%s", msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
