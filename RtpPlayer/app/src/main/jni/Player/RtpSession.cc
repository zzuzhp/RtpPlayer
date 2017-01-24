#include "Player/RtpSession.h"
#include "Streams/AudioStream.h"
#include "Streams/VideoStream.h"

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpSession::RtpSession(RtpSessionObserver * observer) : m_observer(observer)
{

}

RtpSession::~RtpSession()
{
    while (!m_streams.empty())
    {
        destroy_stream(m_streams.back());
        m_streams.pop_back();
    }
}

RtpStream *
RtpSession::create_stream(RTP_MEDIA_TYPE media, int rtp_payload_type, int port)
{
    CProThreadMutexGuard mon(m_lock);

    RtpStream * stream = NULL;

    if (!ISVIDEO(media) && !ISAUDIO(media))
    {
        RP_LOG_E("media type %d is invalid.", media);
        return NULL;
    }

    std::vector<RtpStream *>::const_iterator itr = m_streams.begin();
    for (; itr != m_streams.end(); ++itr)
    {
        if ((*itr)->rtp_payload_type() == rtp_payload_type)
        {
            RP_LOG_E("stream %d already added to the session.", rtp_payload_type);
            return NULL;
        }
    }

    if (ISVIDEO(media))
    {
        VideoStream * video_stream = VideoStream::create_instance(this);
        if (!video_stream)
        {
            RP_LOG_E("stream %d receiver create failed.", rtp_payload_type);
            return NULL;
        }

        if (!video_stream->set_video_media(media, rtp_payload_type, port))
        {
            video_stream->release();
            return NULL;
        }

        stream = video_stream;
    }
    else
    {
        AudioStream * audio_stream = AudioStream::create_instance(this);
        if (!audio_stream)
        {
            RP_LOG_E("stream %d receiver create failed.", rtp_payload_type);
            return NULL;
        }

        if (!audio_stream->set_audio_media(media, rtp_payload_type, port))
        {
            audio_stream->release();
            return NULL;
        }

        stream = audio_stream;
    }

    m_streams.push_back(stream);
    return stream;
}

void
RtpSession::destroy_stream(RtpStream * stream)
{
    RtpStream * rtp_stream = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        std::vector<RtpStream *>::iterator itr = std::find(m_streams.begin(), m_streams.end(), stream);
        if (itr != m_streams.end())
        {
            rtp_stream = *itr;
            m_streams.erase(itr);
        }
    }

    if (rtp_stream)
    {
        RP_FOOTPRINT
        rtp_stream->release();
        RP_FOOTPRINT
    }
}

bool
RtpSession::stream_status(RtpStream * stream, StreamInfo & info)
{
    CProThreadMutexGuard mon(m_lock);

    std::vector<RtpStream *>::iterator itr = std::find(m_streams.begin(), m_streams.end(), stream);
    if (itr == m_streams.end())
    {
        RP_LOG_E("cannot get stream status for payload: %d, stream not found.", stream->rtp_payload_type());
        return false;
    }

    if (ISVIDEO(stream->media_type()))
    {
        /* video */
        VideoStream * stream = (VideoStream *)(*itr);
        stream->video_info(info.framerate, info.bitrate_kbps, info.losses, info.loss_rate);

        info.video = true;
    }
    else
    {
        /* audio */
        AudioStream * stream = (AudioStream *)(*itr);
        stream->audio_info(info.bitrate_kbps, info.losses, info.loss_rate);

        info.video = false;
    }

    return true;
}

void
RtpSession::on_media_frame(RtpStream * stream, MediaFrame * frame)
{
    if (m_observer)
    {
        m_observer->on_session_frame(this, stream, frame);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
