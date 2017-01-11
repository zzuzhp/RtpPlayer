#ifndef ___VIDEOSTREAM_H___
#define ___VIDEOSTREAM_H___

#include "Streams/RtpStream.h"
#include "Common/AVJitter.h"
#include "Depacketizers/RtpDepacketizer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class VideoStream : public RtpStream
{
public:

    VideoStream(RtpStreamObserver * observer);

    ~VideoStream();

    bool set_video_media(RTP_MEDIA_TYPE media, int payload_type, int port);

    void video_info(float & framerate, int & bitrate_kbps, int & losses, float & lossrate);

private:

    void on_rtp_packet(IRtpPacket * packet, bool discontinuous);

private:

    RtpDepacketizer * m_depacketizer;

    AVJitter          m_frame_jitter;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___VIDEOSTREAM_H___
