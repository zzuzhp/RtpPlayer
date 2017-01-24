#include "Streams/RtpStream.h"
#include "pro_time_util.h"
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
                                                     m_client(NULL)
{

}

RtpStream::~RtpStream()
{
    RP_FOOTPRINT
    delete m_client;
    RP_FOOTPRINT
}

void
RtpStream::on_client_opened(RtpClient * client)
{
    m_task.Start();
    RP_LOG_D("rtp session(%d) started.", m_payload_type);
}

void
RtpStream::on_client_closed(RtpClient * client)
{
    m_task.Stop();
    RP_LOG_D("rtp session(%d) closed.", m_payload_type);
}

void
RtpStream::on_rtp_packet(RtpClient * client, IRtpPacket * packet)
{
    bool discontinuous = false;
    int  prev_losses   = (int)m_lossrate.GetLossCount();

    if (!client || !packet)
    {
        RP_LOG_E("on_rtp_packet invalid param.");
        return;
    }

    if (packet->payload_type() != m_payload_type)
    {
        RP_LOG_E("on_rtp_packet no such payload_type: %d.", packet->payload_type());
        return;
    }

    if (ISVIDEO(m_media) && packet->marker())
    {
        m_framerate.PushData(1);
    }

    m_bitrate.PushData(packet->payload_size() * 8);
    m_lossrate.PushData(packet->sequence());

    if ((int)m_lossrate.GetLossCount() != prev_losses)
    {
        /* new losses */
        discontinuous = true;

        std::string timeText;
        ProGetTimeText(timeText);

        RP_LOG_E("[%s]stream %d: rtp packet(s) lost! "
                 "(loss count:%d --> %d: lossrate:%4.1f%%), current rtp packet: %d.",
                 timeText.c_str(), packet->payload_type(),
                 prev_losses, (int)m_lossrate.GetLossCount(), m_lossrate.GetLossRate() * 100, packet->sequence());
    }

    on_rtp_packet(packet, discontinuous);
}

bool
RtpStream::set_media(RTP_MEDIA_TYPE media, int payload_type, int port)
{
    m_client = new(std::nothrow) RtpClient();
    if (!m_client || !m_client->build(this, port))
    {
        delete m_client;
        m_client = NULL;

        RP_LOG_E("rtp client create failed!");
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
