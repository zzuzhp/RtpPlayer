#ifndef ___RTPSTREAM_H___
#define ___RTPSTREAM_H___

#include "pro_stat.h"
#include "pro_functor_command_task.h"
#include "Common/AVModule.h"
#include "Common/common.h"
#include "Rtp/RtpPacket.h"
#include "Rtp/RtpClient.h"

class RtpStream;
class RtpStreamObserver
{
public:
    virtual void on_media_frame(RtpStream * stream, MediaFrame * frame) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class RtpStream : public AVModule,
                  public RtpClientObserver
{
public:

    virtual ~RtpStream();

    const RTP_MEDIA_TYPE media_type() const { return m_media; }

    const int rtp_payload_type() const { return m_payload_type; }

protected:

    RtpStream(RtpStreamObserver * observer, const char * name, int evt_mask = (int)EVENT_ALL);

    unsigned long AddRef()
    {
        return RefCount::AddRef();
    }

    unsigned long Release()
    {
        return RefCount::Release();
    }

    virtual void on_rtp_packet(IRtpPacket * packet, bool discontinuous){};

    bool set_media(RTP_MEDIA_TYPE media, int payload_type, int port);

    void send_frame(MediaFrame * frame);

    /* !
     * RtpClientObserver
     */
    void on_rtp_packet(RtpClient * client, IRtpPacket * packet);

    void on_client_opened(RtpClient * client);

    void on_client_closed(RtpClient * client);

private:

    void on_send_frame(long * args);

protected:

    RtpClient                   * m_client;

    RTP_MEDIA_TYPE                m_media;

    int                           m_payload_type;

    CProStatBitRate               m_framerate;

    CProStatBitRate               m_bitrate;

    CProStatLossRate              m_lossrate;

    typedef void (RtpStream::*ACTION)(long *);
    CProFunctorCommandTask        m_task;

    RtpStreamObserver           * m_observer;

    CProThreadMutex               m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPSTREAM_H___
