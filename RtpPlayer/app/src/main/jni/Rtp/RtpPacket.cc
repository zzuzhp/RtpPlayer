#include "RtpPacket.h"

#include <arpa/inet.h>
#include <new>

#define MAX_PAYLOAD_BYTES (1024 * 63) //// 60--->63

/////////////////////////////////////////////////////////////////////////////
////

RtpPacket *
RtpPacket::create_instance(RTP_HEADER header, const void * payload, uint16_t size)
{
    RtpPacket * packet = NULL;
    if (payload == NULL || size == 0 || size > MAX_PAYLOAD_BYTES)
    {
        return NULL;
    }

    packet = new(std::nothrow)RtpPacket();
    if (!packet || !packet->init(header, payload, size))
    {
        delete packet;
        packet = NULL;
    }

    return packet;
}

RtpPacket::RtpPacket() : m_packet(NULL)
{

}

RtpPacket::~RtpPacket()
{
    delete[] (char *)m_packet;
}

bool
RtpPacket::parse(const char   * buffer,
                 uint16_t       size,
                 RTP_HEADER   & hdr,
                 const char  *& payload,
                 uint16_t     & payload_size)
{
    payload = NULL;
    payload_size = 0;

    if (buffer == NULL || size < sizeof(RTP_HEADER))
    {
        /* wrong param */
        return false;
    }

    bool ret = false;

    do
    {
        unsigned long need_size = sizeof(RTP_HEADER);
        const char * pos = buffer;

        hdr = *(RTP_HEADER *)pos;
        if (hdr.v != 2)
        {
            /* wrong version */
            break;
        }

        pos += sizeof(RTP_HEADER);

        need_size += 4 * hdr.cc;
        if (size < need_size)
        {
            break;
        }

        /* skip CC(s) */
        pos += 4 * hdr.cc;

        if (hdr.x != 0)
        {
            /* check for header extension */
            need_size += 2;
            need_size += 2;
            if (size < need_size)
            {
                break;
            }

            pos += 2;
            /* number of 32-bit words in the extension */
            const uint16_t ext_count = ntohs(*(uint16_t *)pos);
            pos += 2;

            need_size += 4 * ext_count;
            if (size < need_size)
            {
                break;
            }

            /* skip the header extension */
            pos += 4 * ext_count;
        }

        payload = pos;
        payload_size = (uint16_t)(buffer + size - pos);

        if (hdr.p != 0)
        {
            if (size < need_size + 1)
            {
                break;
            }

            unsigned char padding_size = buffer[size - 1];
            if (padding_size < 1)
            {
                padding_size = 1;
            }

            need_size += padding_size;
            if (size < need_size)
            {
                break;
            }

            payload_size -= padding_size;
        }

        if (payload_size == 0)
        {
            payload = NULL;
        }

        ret = true;
    } while (0);

    if (!ret)
    {
        payload = NULL;
        payload_size = 0;
    }

    return ret;
}

bool
RtpPacket::init(RTP_HEADER header, const void * payload, uint16_t size)
{
    m_packet = (RTP_PACKET *)new(std::nothrow) char[sizeof(RTP_PACKET) + size - 1];
    if (!m_packet)
    {
        return false;
    }

    memset(m_packet, 0, sizeof(RTP_PACKET));

    m_packet->hdr = header;

    if (payload != NULL)
    {
        memcpy(m_packet->payload, payload, size);
        m_packet->size = size;
    }

    return true;
}

void
RtpPacket::set_marker(bool m)
{
    m_packet->hdr.m = m ? 1 : 0;
}

bool
RtpPacket::marker() const
{
    return m_packet->hdr.m != 0;
}

void
RtpPacket::set_payload_type(char pt)
{
    m_packet->hdr.pt = pt;
}

char
RtpPacket::payload_type() const
{
    return m_packet->hdr.pt;
}

void
RtpPacket::set_sequence(uint16_t seq)
{
    m_packet->hdr.seq = htons(seq);
}

uint16_t
RtpPacket::sequence() const
{
    return ntohs(m_packet->hdr.seq);
}

void
RtpPacket::set_timestamp(uint32_t ts)
{
    m_packet->hdr.ts = htonl(ts);
}

uint32_t
RtpPacket::timestamp() const
{
    return ntohl(m_packet->hdr.ts);
}

void
RtpPacket::set_ssrc(uint32_t ssrc)
{
    m_packet->hdr.ssrc = htonl(ssrc);
}

uint32_t
RtpPacket::ssrc() const
{
    return ntohl(m_packet->hdr.ssrc);
}

char *
RtpPacket::payload()
{
    return m_packet->payload;
}

uint16_t
RtpPacket::payload_size() const
{
    return m_packet->size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
