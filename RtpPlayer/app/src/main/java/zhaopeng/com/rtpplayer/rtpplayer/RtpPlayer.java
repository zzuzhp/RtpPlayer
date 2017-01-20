package zhaopeng.com.rtpplayer.rtpplayer;

import android.util.Log;
import android.view.Surface;

public class RtpPlayer
{
    private static final String TAG = "RtpPlayer";

    public RtpPlayer()
    {

    }

    static
    {
        System.loadLibrary("mediacodec");
        System.loadLibrary("rtpplayer");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////

    /* \!brief :  create an rtp player
     * \!param :  @port -- rtp port
     * \!return:  handle of the player
     *
     */
    public native long create_rtp_player(Surface surface);

    /* \!brief :  add a single stream
     * \!param :  @type -- mediatype (see MediaType.java)
     *            @rtp_payload_type -- payload type in rtp header
     *            @clock_rate -- timestamp clock rate
     *                           video: 0 means 90000
     *                           audio: must not be zero
     * \!return:  0 for success, -1 for failure
     *
     */
    public native long add_stream(long player, int type, int rtp_payload_type, int clock_rate, int port);

    /* \!brief :  get stream statistics
     *
     */
    public native VideoInfo video_info(long  player);

    /* \!brief :  get stream statistics
     *
     */
    public native AudioInfo audio_info(long  player);

    /* \!brief :  get current play time in ms
     *
     */
    public native int get_play_time_ms(long  player);

    /* \!brief: destroy the player
     *
     */
    public native void destroy_rtp_player(long player);
}
