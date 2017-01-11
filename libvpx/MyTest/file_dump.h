#ifndef ___FILE_DUMP_H___
#define ___FILE_DUMP_H___

#include "vpx/vpx_integer.h"
#include "vp8enc.h"

#include <stdio.h>

struct vpx_enc_config;

typedef struct vpx_file_dumper
{
    size_t      num_frames;
#ifdef _WIN32
    FILE      * file;
#endif
} vpx_file_dumper;

int vpx_create_fileDumper(vpx_file_dumper ** dumper);

int vpx_write_frame(vpx_file_dumper * dumper, video_enc_config * config, uint8_t * frame, int length, int64_t pts);

void vpx_destroy_fileDumper(vpx_file_dumper * dumper);

#endif ///< ___FILE_DUMP_H___
