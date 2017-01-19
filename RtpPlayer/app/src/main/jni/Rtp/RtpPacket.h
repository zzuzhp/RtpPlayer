#ifndef ___RTPPACKET_H___
#define ___RTPPACKET_H___

#include "Common/RefCount.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#pragma pack(push, 1)
#pragma pack(pop)

struct RTP_HEADER
{
#if defined(WORDS_BIGENDIAN)
    unsigned char v  : 2;
    unsigned char p  : 1;
    unsigned char x  : 1;
    unsigned char cc : 4;

    unsigned char m  : 1;
    unsigned char pt : 7;
#else
    unsigned char cc : 4;
    unsigned char x  : 1;
    unsigned char p  : 1;
    unsigned char v  : 2;

    unsigned char pt : 7;
    unsigned char m  : 1;
#endif

    uint16_t    seq;
    uint32_t    ts;
    uint32_t    ssrc;
};

#pragma pack(push, 1)
#pragma pack(pop)

struct RTP_PACKET
{
    RTP_HEADER hdr;
    uint16_t   size;
    char       payload[1];
};

/////////////////////////////////////////////////////////////////////////////
////

class RtpPacket : public RefCount
{
public:

    static RtpPacket * create_instance(const void * payload, uint16_t size);

    static bool parse(const char   * buffer,
                      uint16_t       size,
                      RTP_HEADER   & hdr, //// network byte order
                      const char  *& payload,
                      uint16_t     & payload_size);

    void set_marker(bool m);

    bool marker() const;

    void set_payload_type(char pt);

    char payload_type() const;

    void set_sequence(uint16_t seq);

    uint16_t sequence() const;

    void set_timestamp(uint32_t ts);

    uint32_t timestamp() const;

    void set_ssrc(uint32_t ssrc);

    uint32_t ssrc() const;

    char * payload();

    uint16_t payload_size() const;

private:

    RtpPacket();

    virtual ~RtpPacket();

    bool init(const void * payload, uint16_t size);

private:

    RTP_PACKET * m_packet;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPPACKET_H___
