#ifndef ___RTPDEPACKETIZERH264_H___
#define ___RTPDEPACKETIZERH264_H___

#include "Depacketizers/RtpDepacketizer.h"
#include "pro_thread_mutex.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////    H264 depacketizer

class RtpDepacketizerH264 : public RtpDepacketizer
{
public:

    RtpDepacketizerH264();

    ~RtpDepacketizerH264();

    int  parse(const unsigned char * packet,
               int                   packet_len,
               bool                  frame_end,
               unsigned int          pts,
               int                   payload_type,
               MediaFrame          * frame);

    void set_sync_point();

private:

    int  decode_nalu(const unsigned char * nalu,
                     int                   nalu_len,
                     int                   nalu_type,
                     bool                  marker,
                     unsigned int          pts,
                     int                   payload_type,
                     MediaFrame          * frame);

    bool check_random_access_unit(unsigned char nalu_type);

private:

    enum {STARTCODE_LENGTH = 4};                ///< we use long startcode
    enum {SPS_TYPE = 7};                        ///< sps nalu type
    enum {PPS_TYPE = 8};                        ///< pps nalu type
    enum {IDR_TYPE = 5};                        ///< idr frame nalu type

    unsigned char           * m_nalu;           ///< current nalu
    int                       m_nalu_len;       ///< current nalu length
    bool                      m_sps_sentry;     ///< check for SPS
    bool                      m_pps_sentry;     ///< check for PPS
    bool                      m_idr_sentry;     ///< check for IDR

    unsigned char           * m_frame;          ///< current frame
    int                       m_frame_len;      ///< current frame length

    int                       m_packets;        ///< number of rtp packets for a single frame

    CProThreadMutex           m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPDEPACKETIZERH264_H___
