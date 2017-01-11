#ifndef ___ACODEC_H___
#define ___ACODEC_H___

#include "audio_defs.h"

/*!\brief create an audio encoder
 * returns 0 for success, negative for failure
 */
void * create_audio_encoder(AUDIO_CODEC_TYPE codec);

/*!\brief initialize an audio encoder
 * returns 0 for success, negative for failure
 */
int audio_enc_init(void * handle, audio_enc_config * cfg);

/*!\brief qurey the capabilities of an encoder
 * return the caps
 */
int audio_enc_get_caps(AUDIO_CODEC_TYPE codec);

/*!\brief set a callback for the encoder logs
 * returns 0 for success, negative for failure
 */
int audio_enc_set_cb_log(void * handle, cb_enc_log cb_frame, void * arg);

/*!\brief set a callback for the encoded frames
 * returns 0 for success, negative for failure
 */
int audio_enc_set_cb_frame(void * handle, cb_enc_output cb_frame, void * arg);

/*!\brief change the bitrates or(and) framerate
 * returns 0 for success, negative for failure
 */
int audio_enc_set_bitrate(void * handle, int bitrate_in_kbps);

/*!\brief encode an frame
 * @data_len_in_bytes should be a multiply of channels*bytes_per_sample
 * @ntp_in_ms is ntp of the last sample
 * returns 0 for success, negative for failure
 */
int audio_enc_encode(void * handle, const void * data, int data_len_in_bytes, long long ntp_in_ms);

/*!\brief destroy the encoder
 */
void audio_enc_destroy(void * handle);

#endif ///< ___ACODEC_H___
