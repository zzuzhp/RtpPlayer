#ifndef ___RTPSESSION_H___
#define ___RTPSESSION_H___

#include "Streams/RtpStream.h"
#include "pro_thread_mutex.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class RtpSession;
class RtpSessionObserver
{
public:
    virtual void on_session_frame(RtpSession * session, RtpStream * stream, MediaFrame * frame) = 0;
};

typedef struct StreamInfo
{
    bool    video;

    /* audio & video */
    int     bitrate_kbps;
    int     losses;
    float   loss_rate;

    /* video */
    float   framerate;
} StreamInfo;

class RtpSession : public RtpStreamObserver
{
public:

    RtpSession(RtpSessionObserver * observer);

    ~RtpSession();

    RtpStream * create_stream(RTP_MEDIA_TYPE media, int rtp_payload_type, int port);

    void destroy_stream(RtpStream * stream);

    bool stream_status(RtpStream * stream, StreamInfo & info);

private:

    void on_media_frame(RtpStream * receiver, MediaFrame * frame);

private:

    RtpSessionObserver          * m_observer;

    std::vector<RtpStream *>      m_streams;

    CProThreadMutex               m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPSESSION_H___
