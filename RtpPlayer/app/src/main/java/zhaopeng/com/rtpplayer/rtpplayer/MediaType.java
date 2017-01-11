package zhaopeng.com.rtpplayer.rtpplayer;

public class MediaType
{
    public static final int RTP_MEDIA_AUDIO_AAC  = 1;
    public static final int RTP_MEDIA_AUDIO_OPUS = 2;
    public static final int RTP_MEDIA_VIDEO_AVC  = 5;
    public static final int RTP_MEDIA_VIDEO_VP8  = 6;

    public static boolean IsAudio(int media)
    {
        return media == RTP_MEDIA_AUDIO_AAC || media == RTP_MEDIA_AUDIO_OPUS;
    }

    public static boolean IsVideo(int media)
    {
        return media == RTP_MEDIA_VIDEO_AVC || media == RTP_MEDIA_VIDEO_VP8;
    }
};
