#include <jni.h>
#include <new>

#include "pro_time_util.h"
#include "Player/RtpPlayer.h"

struct MM
{
    int                 java;
    RTP_MEDIA_TYPE      native;
    VIDEO_CODEC_TYPE    codec;
    const char        * name;
} g_media[] =
{
        /* see MediaType.java & common.h */
        {1, RTP_MEDIA_AUDIO_AAC,    CODEC_TYPE_NUL, "aac"},
        {2, RTP_MEDIA_AUDIO_OPUS,   CODEC_TYPE_NUL, "opus"},
        {5, RTP_MEDIA_VIDEO_AVC,    CODEC_TYPE_AVC, "avc"},
        {6, RTP_MEDIA_VIDEO_VP8,    CODEC_TYPE_VP8, "vp8"}
};

typedef struct RTPPlayer
{
    RtpPlayer           * player;
    ANativeWindow       * window;
    /* java global vars */
    JavaVM              * javaVM;
    jobject               javaObj;
} RTPPlayer;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    create_rtp_player
 * Signature: (I)J
 */
extern "C"
jlong JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_create_1rtp_1player(JNIEnv  * env,
                                                                    jobject   thiz,
                                                                    jobject   surface)
{
    /* initialize the logger before all log prints */
    create_logger<LoggerSystem>();
    create_logger<LoggerFile>();

    ANativeWindow * window = ANativeWindow_fromSurface(env, surface);
    if (!window)
    {
        RP_LOG_E("invalid param: rtp player needs a surface to initialize!");
        return 0;
    }

    RTPPlayer * player = new (std::nothrow)RTPPlayer();
    memset(player, 0, sizeof(RTPPlayer));

    player->player = new (std::nothrow)RtpPlayer();
    if (!player->player || !player->player->build())
    {
        RP_LOG_E("rtp player create failed!");
        delete player->player;
        delete player;

        return 0;
    }

    player->window = window;

    env->GetJavaVM(&player->javaVM);
    player->javaObj = env->NewGlobalRef(thiz);

    return (jlong)player;
}

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    add_stream
 * Signature: (JIIII)J
 */
extern "C"
jlong JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_add_1stream(JNIEnv  * env,
                                                            jobject   thiz,
                                                            jlong     handle,
                                                            jint      type,
                                                            jint      payload_type,
                                                            jint      clock,
                                                            jint      port)
{
    RTPPlayer * player = (RTPPlayer *)handle;
    if (!player || !player->player)
    {
        return -1;
    }

    for (int i = 0; i < sizeof(g_media) / sizeof(g_media[0]); ++i)
    {
        if (g_media[i].java == type)
        {
            RP_LOG_D("add one stream: %d, payload type: %d.", type, payload_type);

            if (ISVIDEO(g_media[i].native))
            {
                player->player->play_video(player->window, g_media[i].native, payload_type, port);
            }
            else if (ISAUDIO(g_media[i].native))
            {
                player->player->play_audio(g_media[i].native, payload_type, port);
            }

            break;
        }
    }

    return 0;
}

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    video_info
 * Signature: (J)Lzhaopeng/com/rtpplayer/rtpplayer/VideoInfo;
 */
extern "C"
jobject JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_video_1info (JNIEnv * env, jobject thiz, jlong handle)
{
    jclass    objectClass;
    jmethodID method;
    jobject   object;

    float framerate = 0.0f, loss_rate = 0.0f;
    int width = 0, height = 0, bitrate = 0, losses = 0;

    RTPPlayer * player = (RTPPlayer *)handle;
    if (!player || !player->player)
    {
        return object;
    }

    if (!player->player->video_info(width, height, framerate, bitrate, losses, loss_rate))
    {
        return object;
    }

    objectClass = env->FindClass("zhaopeng/com/rtpplayer/rtpplayer/VideoInfo");
    method      = env->GetMethodID(objectClass, "<init>", "(IIFIIF)V");
    object      = env->NewObject(objectClass, method, width, height, framerate, bitrate, losses, loss_rate);

    return object;
}

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    audio_info
 * Signature: (J)Lzhaopeng/com/rtpplayer/rtpplayer/AudioInfo;
 */
extern "C"
jobject JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_audio_1info (JNIEnv * env, jobject thiz, jlong handle)
{
    jclass    objectClass;
    jmethodID method;
    jobject   object;

    float loss_rate = 0.0f;
    int sample_rate = 0, channels = 0, bits_per_sample = 0, bitrate = 0, losses = 0;

    RTPPlayer * player = (RTPPlayer *)handle;
    if (!player || !player->player)
    {
        return object;
    }

    if (!player->player->audio_info(sample_rate, channels, bits_per_sample, bitrate, losses, loss_rate))
    {
        return object;
    }

    objectClass = env->FindClass("zhaopeng/com/rtpplayer/rtpplayer/AudioInfo");
    method      = env->GetMethodID(objectClass, "<init>", "(IIIIIF)V");
    object      = env->NewObject(objectClass, method, sample_rate, channels, bits_per_sample, bitrate, losses, loss_rate);

    return object;
}

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    get_play_time_ms
 * Signature: (J)I
 */
extern "C"
jint JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_get_1play_1time_1ms(JNIEnv * env, jobject thiz, jlong handle)
{
    RTPPlayer * player = (RTPPlayer *)handle;
    if (player && player->player)
    {
        return player->player->play_time_ms();
    }

    return -1;
}

/*
 * Class:     zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer
 * Method:    destroy_rtp_player
 * Signature: (J)V
 */
extern "C"
void JNICALL
Java_zhaopeng_com_rtpplayer_rtpplayer_RtpPlayer_destroy_1rtp_1player(JNIEnv  * env,
                                                                     jobject   thiz,
                                                                     jlong     handle)
{
    RP_LOG_D("rtp player destroy");

    RTPPlayer * player = (RTPPlayer *)handle;
    if (player)
    {
        delete player->player;
        delete player;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
