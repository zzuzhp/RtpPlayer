package zhaopeng.com.rtpplayer.rtpplayer;

public class VideoInfo
{
    public int    width;
    public int    height;
    public float  framerate;
    public int    bitrate;
    public int    losses;
    public float  loss_rate;

    public VideoInfo(int width, int height, float framerate, int bitrate, int losses, float loss_rate)
    {
        this.width     = width;
        this.height    = height;
        this.framerate = framerate;
        this.bitrate   = bitrate;
        this.losses    = losses;
        this.loss_rate = loss_rate;
    }
};
