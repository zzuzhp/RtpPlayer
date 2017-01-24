#ifndef ___BASEPLAYER_H___
#define ___BASEPLAYER_H___

#include "Renders/AVOut.h"
#include "Decoders/AVDecoder.h"
#include "Streams/RtpStream.h"
#include "Player/AVSync.h"
#include "Common/AVModule.h"
#include "pro_thread_mutex.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class BasePlayer : public RtpStreamObserver,
                   public AVDecoderObserver,
                   public AVModule
{
public:

    BasePlayer(const char * name, int evt_mask = (int)EVENT_ALL);

    virtual ~BasePlayer();

    int play_time_ms();

    const AVOut * renderer() const
    {
        return m_renderer;
    }

protected:

    void clear();

    virtual void on_decoded_image(AVFrame * frame)
    {
        RP_LOG_E("should not call on_decoded_image here.");
    }

    virtual void on_decoded_pcm(AVFrame * frame)
    {
        RP_LOG_E("should not call on_decoded_pcm here.");
    }

private:

    void on_media_frame(RtpStream * receiver, MediaFrame * frame);

    void on_decoder_output(AVDecoder * decoder, AVFrame * frame);

protected:

    RtpStream       * m_stream;

    AVDecoder       * m_decoder;

    AVOut           * m_renderer;

    AVSync          * m_av_sync;

    uint32_t          m_start_ts;

    CProThreadMutex   m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___BASEPLAYER_H___
