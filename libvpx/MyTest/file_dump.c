#include "file_dump.h"
#include "vp8enc.h"
#include "vpx_ports/mem_ops.h"
#include "vpx_mem/vpx_mem.h"

#include <stdlib.h>

#define FRAMERATE_SCALOR_FACTOR 0 ///< fps scalar in percentage * 100. (e.g. 1 indicates a 0.01*fps)

int 
write_ivf_header(vpx_file_dumper * dumper, int codec, int w, int h, int tb_den, int tb_num)
{
    if (!dumper)
    {
        return -1;
    }

    uint8_t ivf_header[32] = { 0 };
    ivf_header[0] = 'D';
    ivf_header[1] = 'K';
    ivf_header[2] = 'I';
    ivf_header[3] = 'F';

    mem_put_le16(ivf_header + 4, 0);                     // version
    mem_put_le16(ivf_header + 6, 32);                    // header size

    switch (codec)                                       // fourcc
    {
    case CODEC_TYPE_VP8:
        ivf_header[8]  = 'V';
        ivf_header[9]  = 'P';
        ivf_header[10] = '8';
        ivf_header[11] = '0';
        break;
    default:
        return -1;
    }

    mem_put_le16(ivf_header + 12, w);           // width
    mem_put_le16(ivf_header + 14, h);           // height
    mem_put_le32(ivf_header + 16, tb_den);      // rate
    mem_put_le32(ivf_header + 20, tb_num);      // scale
    mem_put_le32(ivf_header + 24, 0);           // length
    mem_put_le32(ivf_header + 28, 0);           // unused

#ifdef _WIN32
    fwrite(ivf_header, 1, 32, dumper->file);
#endif

    return 0;
}

int 
vpx_create_fileDumper(vpx_file_dumper ** dumper)
{
    if (!dumper)
    {
        return -1;
    }

    *dumper = (vpx_file_dumper *)vpx_malloc(sizeof(vpx_file_dumper));
    if (!*dumper)
    {
        return -1;
    }

    memset(*dumper, 0, sizeof(vpx_file_dumper));

#ifdef _WIN32
    fopen_s(&(*dumper)->file, "vpx_bs_dump.ivf", "wb");
    if (!(*dumper)->file)
    {
        free(*dumper);
    }
#endif

    return 0;
}

int 
vpx_write_frame(vpx_file_dumper * dumper, video_enc_config * config, uint8_t * frame, int length, int64_t pts)
{
    if (!dumper
#ifdef _WIN32
        || !dumper->file
#endif
        )
    {
        return -1;
    }

    if (dumper->num_frames == 0)
    {
        /* write file header */
        int tb_num = config->timebase_num;
        int tb_den = config->timebase_den;

#if FRAMERATE_SCALOR_FACTOR
        tb_num = 1;
        tb_den = config->timebase_den * FRAMERATE_SCALOR_FACTOR / config->timebase_num / 100;
#endif

        write_ivf_header(dumper, CODEC_TYPE_VP8, config->width, config->height, tb_den, tb_num);
    }

    /* write frame header */
    char header[12];

    mem_put_le32(header, (int)length);
    mem_put_le32(header + 4, (int)(pts & 0xFFFFFFFF));
    mem_put_le32(header + 8, (int)(pts >> 32));
  
#ifdef _WIN32
    fwrite(header, 1, 12, dumper->file);

    /* write frame data */
    fwrite(frame, 1, length, dumper->file);
#endif

    ++dumper->num_frames;

    return 0;
}

void 
vpx_destroy_fileDumper(vpx_file_dumper * dumper)
{
    if (dumper)
    {
#ifdef _WIN32
        if (dumper->file)
        {
            fclose(dumper->file);
        }
#endif
        vpx_free(dumper);
    }
}
