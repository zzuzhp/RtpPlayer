package zhaopeng.com.rtpplayer.rtpplayer;

public class AudioInfo
{
    public int    sample_rate;
    public int    channels;
    public int    bits_per_sample;
    public int    bitrate;
    public int    losses;
    public float  loss_rate;

    public AudioInfo(int sample_rate, int channels, int bits_per_sample, int bitrate, int losses, float loss_rate)
    {
        this.sample_rate     = sample_rate;
        this.channels        = channels;
        this.bits_per_sample = bits_per_sample;
        this.bitrate         = bitrate;
        this.losses          = losses;
        this.loss_rate       = loss_rate;
    }
};
