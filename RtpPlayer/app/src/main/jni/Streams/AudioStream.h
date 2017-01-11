#ifndef ___AUDIOSTREAM_H___
#define ___AUDIOSTREAM_H___

#include "Streams/RtpStream.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AudioStream : public RtpStream
{
public:

    AudioStream(RtpStreamObserver * observer);

    ~AudioStream();

    bool set_audio_media(RTP_MEDIA_TYPE media, int payload_type, int port);

    void audio_info(int & bitrate_kbps, int & losses, float & lossrate);

private:

    void on_rtp_packet(IRtpPacket * packet, bool discontinuous);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AUDIOSTREAM_H___
