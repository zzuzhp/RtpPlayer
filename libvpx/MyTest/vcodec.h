#ifndef ___VCODEC_H___
#define ___VCODEC_H___

#include "video_defs.h"

/*!\brief create a vp8 encoder
* returns 0 for success, negative for failure
*/
void * create_video_encoder(VIDEO_CODEC_TYPE type);

/*!\brief initialize a vp8 encoder
* returns 0 for success, negative for failure
*/
int video_enc_init(void * handle, video_enc_config * cfg);

/*!\brief qurey the capabilities of an encoder
* return the caps
*/
int video_enc_get_caps(void * handle);

/*!\brief set a callback for the encoder logs
* returns 0 for success, negative for failure
*/
int video_enc_set_cb_log(void * handle, cb_enc_log cb_frame, void * arg);

/*!\brief set a callback for the encoded frames
* returns 0 for success, negative for failure
*/
int video_enc_set_cb_output(void * handle, cb_enc_output cb_frame, void * arg);

/*!\brief change the bitrates or(and) framerate
* returns 0 for success, negative for failure
*/
int video_enc_set_rates(void * handle, int new_bitrate_kbit, int frame_rate);

/*!\brief encode an frame
* set @surface to 0 to indicate a flush of the encoder.
* returns 0 for success, negative for failure
*/
int video_enc_encode(void * handle, video_surface * surface, int flags);

/*!\brief destroy the encoder
*/
void video_enc_destroy(void * handle);

#endif ///< ___VCODEC_H___
