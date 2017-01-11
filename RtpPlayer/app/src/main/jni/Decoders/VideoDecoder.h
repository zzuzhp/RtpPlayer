#ifndef ___VIDEODECODER_H___
#define ___VIDEODECODER_H___

#include "videodec.h"
#include "Decoders/AVDecoder.h"
#include "Common/AVFrame.h"

#include <new>

class VideoDecoder;
class VideoDecoderObserver
{
public:
    virtual void on_decoded_image (VideoDecoder * decoder, AVFrame * image) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class VideoDecoder : public AVDecoder
{
public:

    static VideoDecoder * create_instance()
    {
        return new (std::nothrow)VideoDecoder();
    }

    bool build(VIDEO_CODEC_TYPE codec, VideoDecoderObserver * observer);

    void tear();

    void decode(MediaFrame * frame);

private:

    VideoDecoder();

    ~VideoDecoder();

    static void dec_image_output(video_dec_context * dec, video_surface * image, void * cb_arg);

    static void dec_log_output(video_dec_context * dec, const char * msg, void * cb_arg);

private:

    VideoDecoderObserver * m_observer;

    video_dec_context    * m_decoder;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___VIDEODECODER_H___
