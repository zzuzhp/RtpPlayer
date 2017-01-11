#include "Streams/RtpStream.h"
#include "pro_net.h"
#include "pro_time_util.h"
#include "pro_bsd_wrapper.h"
#include "pro_functor_command.h"

#include <string>
#include <new>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpStream::RtpStream(RtpStreamObserver * observer,
                     const char        * name,
                     int                 evt_mask) : AVModule(AV_MODULE_SOURCE, name, evt_mask),
                                                     m_observer(observer),
                                                     m_media(RTP_MEDIA_INVALID),
                                                     m_payload_type(0),
                                                     m_reactor(NULL),
                                                     m_session(NULL)
{

}

RtpStream::~RtpStream()
{
    if (m_session)
    {
        DeleteRtpSession(m_session);
    }

    if (m_reactor)
    {
        ProDeleteReactor(m_reactor);
    }
}

void PRO_STDCALL
RtpStream::OnOkSession(IRtpSession * session)
{
    m_task.Start();
    RP_LOG_D("rtp session(%d) started.", m_payload_type);
}

void PRO_STDCALL
RtpStream::OnSendSession(IRtpSession * session, bool packetErased)
{

}

void PRO_STDCALL
RtpStream::OnCloseSession(IRtpSession * session, long errorCode)
{
    m_task.Stop();
    RP_LOG_D("rtp session(%d) closed.", m_payload_type);
}

void PRO_STDCALL
RtpStream::OnRecvSession(IRtpSession * session, IRtpPacket * packet)
{
    bool discontinuous = false;
    int  prev_losses   = (int)m_lossrate.GetLossCount();

    if (!session || !packet)
    {
        RP_LOG_E("OnRecvSession invalid param.");
        return;
    }

    if (packet->GetPayloadType() != m_payload_type)
    {
        RP_LOG_E("OnRecvSession no such payload_type: %d.", packet->GetPayloadType());
        return;
    }

    if (ISVIDEO(m_media) && packet->GetMarker())
    {
        m_framerate.PushData(1);
    }

    m_bitrate.PushData(packet->GetPayloadSize() * 8);
    m_lossrate.PushData(packet->GetSequence());

    if ((int)m_lossrate.GetLossCount() != prev_losses)
    {
        /* new losses */
        discontinuous = true;

        std::string timeText;
        ProGetTimeText(timeText);

        RP_LOG_E("[%s]stream %d: rtp packet(s) lost! "
                 "(loss count:%d --> %d: lossrate:%4.1f%%), current rtp packet: %d.",
                 timeText.c_str(), packet->GetPayloadType(),
                 prev_losses, (int)m_lossrate.GetLossCount(), m_lossrate.GetLossRate() * 100, packet->GetSequence());
    }

    on_rtp_packet(packet, discontinuous);
}

bool
RtpStream::set_media(RTP_MEDIA_TYPE media, int payload_type, int port)
{
    InitRtpFramework();

    RTP_SESSION_INFO info;
    memset(&info, 0, sizeof(RTP_SESSION_INFO));

    info.mmType        = ISVIDEO(media) ? RTP_MMT_VIDEO : RTP_MMT_AUDIO;
    info.in.enable     = true;
    info.sessionType_r = RTP_ST_UDPCLIENT;

    m_reactor = ProCreateReactor(2, 2);
    if (!m_reactor)
    {
        RP_LOG_E("reactor create failed!");
        return false;
    }

    m_session = CreateRtpSessionUdpclient(this, m_reactor, &info, true, NULL, port);
    if (!m_session)
    {
        RP_LOG_E("player create failed: %d!", pbsd_errno());
        ProDeleteReactor(m_reactor);

        m_reactor = NULL;
        return false;
    }

    m_media         = media;
    m_payload_type  = payload_type;

    return true;
}

void
RtpStream::on_send_frame(long * args)
{
    MediaFrame * media_frame = (MediaFrame *)args[0];

    if (!media_frame)
    {
        RP_LOG_E("rtp receiver got one null frame!");
        return;
    }

    if (m_observer)
    {
        m_observer->on_media_frame(this, media_frame);
    }

    delete[] media_frame->data;
    delete media_frame;
}

void
RtpStream::send_frame(MediaFrame * frame)
{
    IProFunctorCommand * command = NULL;

    MediaFrame * media_frame = new (std::nothrow) MediaFrame;

    media_frame->type               = m_media;
    media_frame->data               = new (std::nothrow)uint8_t[frame->len];
    media_frame->len                = frame->len;
    media_frame->pts                = frame->pts;
    media_frame->rtp_payload_type   = frame->rtp_payload_type;
    media_frame->flags              = frame->flags;
    media_frame->packets            = frame->packets;

    memcpy(media_frame->data, frame->data, frame->len);

    /* !!! 32-bit version */
    command = CProFunctorCommand_cpp<RtpStream, ACTION, 1>::CreateInstance(*this,
                                                                           &RtpStream::on_send_frame,
                                                                           (long)media_frame);
    if (!command)
    {
        delete[] media_frame->data;
        delete media_frame;
        return;
    }

    if (!m_task.Put(command))
    {
        delete[] media_frame->data;
        delete media_frame;
        command->Destroy();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
