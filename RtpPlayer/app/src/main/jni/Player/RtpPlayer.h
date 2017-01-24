#ifndef ___RTPPLAYER_H___
#define ___RTPPLAYER_H___

#include "Player/AVSync.h"
#include "Player/VideoPlayer.h"
#include "Player/AudioPlayer.h"
#include "pro_thread_mutex.h"

#include <android/native_window_jni.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
//// android rtp player

class RtpPlayer
{
public:

    RtpPlayer();

    ~RtpPlayer();

    bool build();

    void tear();

    bool play_video(ANativeWindow * window, RTP_MEDIA_TYPE media, int rtp_payload_type, int port);

    bool play_audio(RTP_MEDIA_TYPE media, int rtp_payload_type, int port);

    void remove_video();

    void remove_audio();

    bool video_info(int   & width,
                    int   & height,
                    float & framerate,
                    int   & bitrate_kbps,
                    int   & losses,
                    float & lossrate);

    bool audio_info(int   & sample_rate,
                    int   & channels,
                    int   & bits_per_sample,
                    int   & bitrate_kbps,
                    int   & losses,
                    float & lossrate);

    int  play_time_ms();

private:

    VideoPlayer                 * m_video_player;

    AudioPlayer                 * m_audio_player;

    AVSync                      * m_av_sync;

    CProThreadMutex               m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPPLAYER_H___
