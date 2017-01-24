#ifndef ___RTPPLAYER_H___
#define ___RTPPLAYER_H___

#include "Player/RtpSession.h"
#include "Player/AVSync.h"
#include "Renders/VideoOut.h"
#include "Renders/AudioOut.h"
#include "Decoders/VideoDecoder.h"
#include "Decoders/AudioDecoder.h"
#include "pro_thread_mutex.h"

#include <android/native_window_jni.h>

typedef struct VideoPlayer
{
    RtpStream       * stream;
    AVOut           * vout;
    VideoDecoder    * decoder;

    uint32_t          start_ts;
    int               width;
    int               height;
} VideoPlayer;

typedef struct AudioPlayer
{
    RtpStream       * stream;
    AVOut           * aout;
    AudioDecoder    * decoder;

    uint32_t          start_ts;
    int               sample_rate;
    int               channels;
    int               bits_per_sample;
} AudioPlayer;

////////////////////////////////////////////////////////////////////////////////////////////////////
//// android rtp player

class RtpPlayer : public RtpSessionObserver,
                  public VideoDecoderObserver,
                  public AudioDecoderObserver
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

    void remove_player(VideoPlayer * player);

    void remove_player(AudioPlayer * player);

    void on_session_frame(RtpSession * session, RtpStream * stream, MediaFrame * frame);

    void on_decoded_image (VideoDecoder * decoder, AVFrame * image);

    void on_decoded_pcm (AudioDecoder * decoder, AVFrame * pcm_frame);

private:

    RtpSession                  * m_session;

    VideoPlayer                 * m_video_player;

    AudioPlayer                 * m_audio_player;

    AVSync                      * m_av_sync;

    CProThreadMutex               m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPPLAYER_H___
