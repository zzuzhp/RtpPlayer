#ifndef ___RTPSTREAM_H___
#define ___RTPSTREAM_H___

#include "rtp_framework.h"
#include "pro_stat.h"
#include "pro_functor_command_task.h"
#include "Common/AVModule.h"
#include "Common/common.h"

class RtpStream;
class RtpStreamObserver
{
public:
    virtual void on_media_frame(RtpStream * stream, MediaFrame * frame) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class RtpStream : public AVModule,
                  public IRtpSessionObserver
{
public:

    virtual ~RtpStream();

    const RTP_MEDIA_TYPE media_type() const { return m_media; }

    const int rtp_payload_type() const { return m_payload_type; }

protected:

    RtpStream(RtpStreamObserver * observer, const char * name, int evt_mask = (int)EVENT_ALL);

    unsigned long PRO_STDCALL AddRef()
    {
        return RefCount::AddRef();
    }

    unsigned long PRO_STDCALL Release()
    {
        return RefCount::Release();
    }

    virtual void on_rtp_packet(IRtpPacket * packet, bool discontinuous){};

    bool set_media(RTP_MEDIA_TYPE media, int payload_type, int port);

    void PRO_STDCALL OnRecvSession(IRtpSession * session, IRtpPacket * packet);

    void PRO_STDCALL OnOkSession(IRtpSession * session);

    void PRO_STDCALL OnSendSession(IRtpSession * session, bool packetErased);

    void PRO_STDCALL OnCloseSession(IRtpSession * session, long errorCode);

    void send_frame(MediaFrame * frame);

private:

    void on_send_frame(long * args);

protected:

    IProReactor                 * m_reactor;

    IRtpSession                 * m_session;

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
