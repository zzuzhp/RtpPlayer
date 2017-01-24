#ifndef ___AVFRAME_H___
#define ___AVFRAME_H___

#include "Common/RefCount.h"
#include "Decoders/AVDecoder.h"
#include "videodec.h"
#include "audiodec.h"

#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
//// audio/video data with ref count

class AVFrame : public RefCount
{
public:

    static AVFrame * create_instance(AVDecoder * decoder, video_surface * surface, video_dec_context * ctx)
    {
        if (!decoder || !surface)
        {
            return NULL;
        }

        return new (std::nothrow) AVFrame(decoder, surface, ctx);
    }

    static AVFrame * create_instance(audio_dec_pcm * pcm, audio_dec_context * ctx)
    {
        if (!pcm)
        {
            return NULL;
        }

        audio_dec_pcm * pcm_frame = new (std::nothrow)audio_dec_pcm;
        if (!pcm_frame)
        {
            return NULL;
        }

        pcm_frame->pcm = new (std::nothrow)char[pcm->len];
        if (!pcm_frame->pcm)
        {
            return NULL;
        }

        memcpy(pcm_frame->pcm, pcm->pcm, pcm->len);

        pcm_frame->len              = pcm->len;
        pcm_frame->sample_rate      = pcm->sample_rate;
        pcm_frame->channels         = pcm->channels;
        pcm_frame->bits_per_sample  = pcm->bits_per_sample;
        pcm_frame->duration_ms      = pcm->duration_ms;
        pcm_frame->type             = pcm->type;
        pcm_frame->pts              = pcm->pts;
        pcm_frame->opaque           = pcm->opaque;

        return new AVFrame(pcm_frame, ctx);
    }

    bool is_video() const
    {
        return m_video;
    }

    const uint32_t get_pts() const
    {
        if (m_video)
        {
            return m_surface ? m_surface->pts : 0;
        }

        return m_pcm ? m_pcm->pts : 0;
    }

    const uint8_t * get_y() const
    {
        return m_surface ? m_surface->planes[0] : 0;
    }

    const uint8_t * get_u() const
    {
        return m_surface ? m_surface->planes[1] : 0;
    }

    const uint8_t * get_v() const
    {
        return m_surface ? m_surface->planes[2] : 0;
    }

    int get_width() const
    {
        return m_surface ? m_surface->width[0] : 0;
    }

    int get_height() const
    {
        return m_surface ? m_surface->height[0] : 0;
    }

    int get_stride() const
    {
        return m_surface ? m_surface->pitch[0] : 0;
    }

    char * get_pcm()
    {
        return m_pcm ? (char *)m_pcm->pcm : 0;
    }

    int get_pcm_len()
    {
        return m_pcm ? m_pcm->len : 0;
    }

    int get_sample_rate()
    {
        return m_pcm ? m_pcm->sample_rate : 0;
    }

    int get_channels()
    {
        return m_pcm ? m_pcm->channels : 0;
    }

    int get_bits_per_sample()
    {
        return m_pcm ? m_pcm->bits_per_sample : 0;
    }

    int get_audio_duration()
    {
        return m_pcm ? m_pcm->duration_ms : 0;
    }

private:

    AVFrame(AVDecoder           * decoder,
            video_surface       * surface,
            video_dec_context   * ctx) : m_decoder(decoder),
                                         m_surface(surface),
                                         m_context(ctx),
                                         m_video(true),
                                         m_a_context(NULL),
                                         m_pcm(NULL)
    {
        /* hold reference to the decoder to make sure the context is still valid before the frame is released */
        m_decoder->add_ref();
    }

    AVFrame(audio_dec_pcm       * pcm,
            audio_dec_context   * ctx) : m_pcm(pcm),
                                         m_a_context(ctx),
                                         m_video(false),
                                         m_decoder(NULL),
                                         m_surface(NULL),
                                         m_context(NULL)
    {

    }

    ~AVFrame()
    {
        if (m_video)
        {
            video_dec_release_image(m_context, m_surface);
            m_decoder->release();
        }
        else
        {
            if (m_pcm)
            {
                delete[] (char *)m_pcm->pcm;
                delete m_pcm;
            }
        }
    }

private:

    bool                 m_video;

    AVDecoder          * m_decoder;
    video_surface      * m_surface;
    video_dec_context  * m_context;

    audio_dec_context  * m_a_context;
    audio_dec_pcm      * m_pcm;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVFRAME_H___
