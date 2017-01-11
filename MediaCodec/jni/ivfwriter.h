#ifndef ___IVFWRITER_H___
#define ___IVFWRITER_H___

#include "videodefs.h"

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

typedef struct ivfwriter
{
    VIDEO_CODEC_TYPE   codec;
    size_t             num_frames;
    FILE             * file;
} ivfwriter;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

int create_fileWriter(ivfwriter ** writer, VIDEO_CODEC_TYPE codec);

int write_frame(ivfwriter * writer, video_enc_config * config, unsigned char * frame, int length, long long pts);

void destroy_fileWriter(ivfwriter * writer);

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___IVFWRITER_H___
