#include "ivfwriter.h"
#include "vpx_ports/mem_ops.h"
#include "vpx_mem/vpx_mem.h"

#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define FRAMERATE_SCALOR_FACTOR 10 ///< fps scalar in percentage * 100. (e.g. 1 indicates a 0.01*fps)

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int 
write_ivf_header(ivfwriter * writer, int w, int h, int tb_den, int tb_num)
{
    if (!writer)
    {
        return -1;
    }

    uint8_t ivf_header[32] = { 0 };
    ivf_header[0] = 'D';
    ivf_header[1] = 'K';
    ivf_header[2] = 'I';
    ivf_header[3] = 'F';

    mem_put_le16(ivf_header + 4, 0);           ///< version
    mem_put_le16(ivf_header + 6, 32);          ///< header size

    switch (writer->codec)                     ///< fourcc
    {
    case CODEC_TYPE_VP8:
        ivf_header[8]  = 'V';
        ivf_header[9]  = 'P';
        ivf_header[10] = '8';
        ivf_header[11] = '0';
        break;
    case CODEC_TYPE_AVC:
        ivf_header[8]  = 'H';
        ivf_header[9]  = '2';
        ivf_header[10] = '6';
        ivf_header[11] = '4';
        break;
    default:
        return -1;
    }

    mem_put_le16(ivf_header + 12, w);          ///< width
    mem_put_le16(ivf_header + 14, h);          ///< height
    mem_put_le32(ivf_header + 16, tb_den);     ///< rate
    mem_put_le32(ivf_header + 20, tb_num);     ///< scale
    mem_put_le32(ivf_header + 24, 0);          ///< length
    mem_put_le32(ivf_header + 28, 0);          ///< unused

    fwrite(ivf_header, 1, 32, writer->file);
    fflush(writer->file);

    return 0;
}

int 
create_fileWriter(ivfwriter ** writer, VIDEO_CODEC_TYPE codec)
{
    if (!writer)
    {
        return -1;
    }

    *writer = (ivfwriter *)vpx_malloc(sizeof(ivfwriter));
    if (!*writer)
    {
        return -1;
    }

    (*writer)->codec = codec;
    (*writer)->num_frames = 0;

    (*writer)->file = fopen("/sdcard/stream.ivf", "wb");
    if (!(*writer)->file)
    {
        free(*writer);
    }

    return 0;
}

int 
write_frame(ivfwriter * writer, video_enc_config * config, unsigned char * frame, int length, long long pts)
{
    if (!writer || !writer->file)
    {
        return -1;
    }

    if (writer->num_frames == 0)
    {
        /* write file header */
        int tb_num = config->timebase_num;
        int tb_den = config->timebase_den;

#if FRAMERATE_SCALOR_FACTOR
        tb_num = 1;
        tb_den = config->timebase_den * FRAMERATE_SCALOR_FACTOR / config->timebase_num / 100;
#endif

        write_ivf_header(writer, config->width, config->height, tb_den, tb_num);
    }

    /* write frame header */
    char header[12];

    mem_put_le32(header, (int)length);
    mem_put_le32(header + 4, (int)(pts & 0xFFFFFFFF));
    mem_put_le32(header + 8, (int)(pts >> 32));

    fwrite(header, 1, 12, writer->file);

    /* write frame data */
    fwrite(frame, 1, length, writer->file);

    fflush(writer->file);

    ++writer->num_frames;

    return 0;
}

void
destroy_fileWriter(ivfwriter * writer)
{
    if (writer)
    {
        if (writer->file)
        {
            fclose(writer->file);
        }

        vpx_free(writer);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////

