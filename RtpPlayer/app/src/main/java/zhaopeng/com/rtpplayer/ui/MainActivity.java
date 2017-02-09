package zhaopeng.com.rtpplayer.ui;

import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import java.io.IOException;
import java.io.RandomAccessFile;

import zhaopeng.com.rtpplayer.R;
import zhaopeng.com.rtpplayer.rtpplayer.AudioInfo;
import zhaopeng.com.rtpplayer.rtpplayer.MediaType;
import zhaopeng.com.rtpplayer.rtpplayer.RtpPlayer;
import zhaopeng.com.rtpplayer.rtpplayer.VideoInfo;

public class MainActivity extends AppCompatActivity
{
    private static final String            TAG = "MainActivity";
    private static final int              port = 5000;
    private static final int             media = (int)MediaType.RTP_MEDIA_VIDEO_VP8;
    private static final int           payload = 96;
    private static final int            a_port = 5002;
    private static final int           a_media = (int)MediaType.RTP_MEDIA_AUDIO_OPUS;
    private static final int         a_payload = 97;
    private static final int         a_clock_r = 48000;
    private static final int VIDEO_INFO_UPDATE = 0;
    private static final int  PLAY_TIME_UPDATE = 1;
    private static final int          CPU_RATE = 2;

    private CPURate     cpu;
    private RtpPlayer   player;
    private long        handle;
    private SurfaceView surface   = null;
    private TextView    av_info   = null;
    private TextView    play_time = null;
    private TextView    cpu_rate  = null;
    private volatile boolean exit = false;
    private Handler msgHandler    = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            if (msg.what == VIDEO_INFO_UPDATE)
            {
                av_info.setText(String.valueOf(msg.obj));
            }
            else if (msg.what == PLAY_TIME_UPDATE)
            {
                play_time.setText(String.valueOf(msg.obj));
            }
            else if (msg.what == CPU_RATE)
            {
                cpu_rate.setText(String.valueOf(msg.obj));
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        av_info   = (TextView) findViewById(R.id.stream_info);
        play_time = (TextView) findViewById(R.id.play_time);
        cpu_rate  = (TextView) findViewById(R.id.cpu);

        av_info.setText(String.format("listening to: ") +
                        getWIFILocalIpAdress() +
                        String.format(":%d/%d", port, a_port) +
                        String.format("\nconfigured for %s, %s", mediaType(media), mediaType(a_media)));

        cpu = new CPURate();
        player = new RtpPlayer();

        createSurface();

        new Thread()
        {
            public void run()
            {
                do
                {
                    VideoInfo info   = player.video_info(handle);
                    AudioInfo a_info = player.audio_info(handle);

                    try
                    {
                        String av_msg = "";

                        if (info != null && info.width != 0 && info.height != 0)
                        {
                            av_msg = "Video: " +
                                     String.format("%4s %13s ",mediaType(media), abbrvString(info.width, info.height)) +
                                     String.format("%4.1ffps %4dkbps loss: %d\n", info.framerate, info.bitrate, info.losses);
                        }

                        if (a_info != null && a_info.sample_rate != 0 && a_info.channels != 0 && a_info.bits_per_sample != 0)
                        {
                            av_msg += "Audio: " +
                                      String.format("%4s ", mediaType(a_media)) +
                                      String.format("%5dhz %1dch(s) %2dbits %4dkbps loss: %d", a_info.sample_rate, a_info.channels, a_info.bits_per_sample, a_info.bitrate, a_info.losses);
                        }

                        if (av_msg.length() > 0)
                        {
                            Message msg = new Message();

                            msg.what = VIDEO_INFO_UPDATE;
                            msg.obj  = av_msg;

                            msgHandler.sendMessage(msg);
                        }

                        {
                            int time = player.get_play_time_ms(handle);

                            Message msg = new Message();

                            msg.what = PLAY_TIME_UPDATE;
                            msg.obj  = formatTime(time);

                            msgHandler.sendMessage(msg);
                        }

                        {
                            Message msg = new Message();

                            msg.what = CPU_RATE;
                            msg.obj  = String.format("cpu: %.1f%%", Math.max(0, cpu.CPULoad() * 100));

                            msgHandler.sendMessage(msg);
                        }

                        Thread.sleep(1000);
                    }
                    catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                } while (!exit);
            }
        }.start();
    }

    private void createSurface()
    {
        surface = (SurfaceView) findViewById(R.id.surfaceView);

        surface.getHolder().addCallback(new SurfaceHolder.Callback()
        {
            @Override
            public void surfaceDestroyed(SurfaceHolder holder)
            {
                Log.d(TAG, "surface destroyed");
                exit = true;
                player.destroy_rtp_player(handle);
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder)
            {
                Log.d(TAG, "surface created");

                handle = player.create_rtp_player(holder.getSurface());

                player.add_stream(handle,   media,   payload,         0, port);
                player.add_stream(handle, a_media, a_payload, a_clock_r, a_port);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
            {
                Log.d(TAG, "surface changed");
            }
        });
    }

    public String getWIFILocalIpAdress()
    {
        WifiManager wifiManager = (WifiManager) this.getSystemService(this.WIFI_SERVICE);
        if (!wifiManager.isWifiEnabled())
        {
            wifiManager.setWifiEnabled(true);
        }

        int address = wifiManager.getConnectionInfo().getIpAddress();

        return formatIpAddress(address);
    }

    private String formatIpAddress(int ipAddress)
    {
        return (ipAddress & 0xFF) + "." + ((ipAddress >> 8) & 0xFF) + "." + ((ipAddress >> 16) & 0xFF) + "." + (ipAddress >> 24 & 0xFF);
    }

    private String mediaType(int media)
    {
        if (media == (int)MediaType.RTP_MEDIA_VIDEO_AVC)
        {
            return "AVC";
        }
        else if (media == (int)MediaType.RTP_MEDIA_VIDEO_VP8)
        {
            return "VP8";
        }
        else if (media == (int)MediaType.RTP_MEDIA_AUDIO_AAC)
        {
            return "AAC";
        }
        else if (media == (int)MediaType.RTP_MEDIA_AUDIO_OPUS)
        {
            return "OPUS";
        }

        return "null";
    }

    private String abbrvString(int width, int height)
    {
        /* Acronyms and Abbreviations */
        if (width == 128 && height == 96)
        {
            return "SQCIF";
        }
        else if (width == 160 && height == 120)
        {
            return "QQVGA";
        }
        else if (width == 176 && height == 144)
        {
            return "QCIF";
        }
        else if (width == 320 && height == 240)
        {
            return "QVGA";
        }
        else if (width == 352 && height == 240)
        {
            return "525 SIF";
        }
        else if (width == 352 && height == 288)
        {
            return "CIF";
        }
        else if (width == 352 && height == 480)
        {
            return "525 HHR";
        }
        else if (width == 352 && height == 576)
        {
            return "625 HHR";
        }
        else if (width == 640 && height == 480)
        {
            return "VGA";
        }
        else if (width == 704 && height == 480)
        {
            return "525 4SIF";
        }
        else if (width == 720 && height == 480)
        {
            return "525 SD";
        }
        else if (width == 704 && height == 576)
        {
            return "4CIF";
        }
        else if (width == 720 && height == 576)
        {
            return "625 SD";
        }
        else if (width == 800 && height == 600)
        {
            return "SVGA";
        }
        else if (width == 1024 && height == 768)
        {
            return "XGA";
        }
        else if (width == 1280 && height == 960)
        {
            return "4VGA";
        }
        else if (width == 1280 && height == 1024)
        {
            return "SXGA";
        }
        else if (width == 1408 && height == 960)
        {
            return "525 16SIF";
        }
        else if (width == 1408 && height == 1152)
        {
            return "16CIF";
        }
        else if (width == 1600 && height == 1200)
        {
            return "4SVGA";
        }
        else if (width == 1280 && height == 720)
        {
            return "720p HD";
        }
        else if (width == 1920 && (height == 1080 || height == 1088))
        {
            return "1080 HD";
        }
        else if ((width == 3840 || width == 4096) && height == 2160)
        {
            return "4K";
        }

        return String.format("%dx%d", width, height);
    }

    private String formatTime(int ms)
    {
        int totalSeconds    = ms / 1000;
        int seconds         = totalSeconds % 60;
        int minutes         = (totalSeconds / 60) % 60;
        int hours           = totalSeconds / 3600;

        return String.format("%02d:%02d:%02d", hours, minutes, seconds);
    }
}
