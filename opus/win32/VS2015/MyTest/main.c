#include "acodec.h"

#include <windows.h>
#include <stdio.h>

#define AUDIO_FILE_NAME  "16bit-2channels-48000hz.pcm" ///< test file full path
#define AUDIO_CHANNELS   2      ///<  2 channels
#define AUDIO_SAMPLEBITS 16     ///< 16 bit
#define AUDIO_SAMPLERATE 48000  ///< 48 kHz
#define AUDIO_BLOCK_SIZE 10     ///< 10 ms

void 
enc_output(void * enc, audio_enc_frame_bs * frame, void * arg)
{
    printf("frame duration: %d(ms), ntp:%lld(ms).\n", frame->samples * 1000 / AUDIO_SAMPLERATE, frame->ntp);
}

void 
enc_log(void * enc, const char * msg, void * cb_arg)
{
    printf("%s", msg);
}

int main()
{
    long long ntp = 0;

    FILE * file = 0;
    fopen_s(&file, AUDIO_FILE_NAME, "rb");

    int frame_size = AUDIO_SAMPLERATE / 1000 * AUDIO_BLOCK_SIZE * AUDIO_CHANNELS * (AUDIO_SAMPLEBITS >> 3);
    unsigned char * frame = (unsigned char *)malloc(frame_size);

    audio_enc_config cfg;

    cfg.bitrate             = 32;
    cfg.channels            = AUDIO_CHANNELS;
    cfg.bits_per_sample     = AUDIO_SAMPLEBITS;
    cfg.sample_rate         = AUDIO_SAMPLERATE;
    cfg.enable_audio_level  = 1;

    void * handle = create_audio_encoder(CODEC_TYPE_OPUS);

    if (0 > audio_enc_init(handle, &cfg))
    {
        return -1;
    }

    audio_enc_set_cb_log(handle, &enc_log, 0);
    audio_enc_set_cb_frame(handle, &enc_output, 0);

    do
    {
        int read_len = fread(frame, 1, frame_size, file);
        if (read_len <= 0)
        {
            break;
        }

        audio_enc_encode(handle, frame, read_len, ntp += AUDIO_BLOCK_SIZE);
    } while (1);

    printf("done.\n");
    getchar();

    return 0;
}

