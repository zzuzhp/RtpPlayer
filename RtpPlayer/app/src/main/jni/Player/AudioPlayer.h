#ifndef ___AUDIOPLAYER_H___
#define ___AUDIOPLAYER_H___

#include "Player/BasePlayer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AudioPlayer : public BasePlayer
{
public:

    AudioPlayer();

    ~AudioPlayer();

    bool build (AVSync * sync, RTP_MEDIA_TYPE media, int rtp_payload_type, int port);

    void tear();

    bool audio_info(int     & sample_rate,
                    int     & channels,
                    int     & bits_per_sample,
                    int     & bitrate_kbps,
                    int     & losses,
                    float   & lossrate);

private:

    void on_decoded_pcm (AVFrame * pcm);

private:

    int     m_sample_rate;

    int     m_channels;

    int     m_bits_per_sample;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AUDIOPLAYER_H___
