package zhaopeng.com.rtpplayer.ui;

import android.util.Log;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;

public class CPURate
{
    private static final String TAG = "CPURate";

    private long idle;
    private long cpu;

    public CPURate()
    {
        idle = -1;
        cpu  = -1;
    }

    public float CPULoad()
    {
        float cpuLoad;
        long idle_new = 0;
        long cpu_new = 0;

        try
        {
            RandomAccessFile reader = new RandomAccessFile("/proc/stat", "r");
            String load = reader.readLine();

            String[] toks = load.split(" +");  // Split on one or more spaces

            idle_new = Long.parseLong(toks[4]);
            cpu_new  = Long.parseLong(toks[2]) +
                       Long.parseLong(toks[3]) +
                       Long.parseLong(toks[5]) +
                       Long.parseLong(toks[6]) +
                       Long.parseLong(toks[7]) +
                       Long.parseLong(toks[8]);
        }
        catch (IOException ex)
        {
            ex.printStackTrace();
        }

        if (idle == -1 && cpu == -1)
        {
            cpuLoad = 0.0f;
        }
        else
        {
            cpuLoad = (float) (cpu_new - cpu) / ((cpu_new + idle_new) - (cpu + idle));
            Log.d(TAG, String.format("cpu usage: %f", cpuLoad));
        }

        idle = idle_new;
        cpu  = cpu_new;

        return cpuLoad;
    }
};
