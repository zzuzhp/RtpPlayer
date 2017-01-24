#ifndef ___VIDEOPLAYER_H___
#define ___VIDEOPLAYER_H___

#include "Player/BasePlayer.h"
#include <android/native_window_jni.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class VideoPlayer : public BasePlayer
{
public:

    VideoPlayer();

    ~VideoPlayer();

    bool build (AVSync * sync, ANativeWindow * window, RTP_MEDIA_TYPE media, int rtp_payload_type, int port);

    void tear();

    bool video_info(int   & width,
                    int   & height,
                    float & framerate,
                    int   & bitrate_kbps,
                    int   & losses,
                    float & lossrate);

private:

    void on_decoded_image(AVFrame * image);

private:

    int     m_width;

    int     m_height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___VIDEOPLAYER_H___
