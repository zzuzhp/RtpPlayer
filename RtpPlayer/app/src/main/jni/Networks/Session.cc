#include "Session.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

Session::Session(asio::io_service & io) : Dispatcher(io),
                                          m_buffer_size(0),
                                          m_read_handler(nullptr),
                                          m_read_complete_handler(nullptr),
                                          m_write_handler(nullptr)
{

}

Session::~Session()
{
    m_response.consume(m_response.size());
    m_request.consume(m_request.size());
}

void
Session::on_read(const asio::error_code & err, size_t bytes_transferred)
{
    if (bytes_transferred > 0 && m_read_handler != nullptr)
    {
        char * data = new char[bytes_transferred + 1];
        data[bytes_transferred] = 0;

        m_response.commit(bytes_transferred);
        std::istream stream(&m_response);
        stream.read(data, bytes_transferred);

        m_read_handler(data, bytes_transferred);
        delete[] data;
    }

    check_error(err, bytes_transferred, err != asio::error::eof);

    if (err == asio::error::eof ||
        (m_read_complete_handler != nullptr && m_buffer_size > 0 && bytes_transferred < m_buffer_size))
    {
        m_read_complete_handler(m_buffer_size, bytes_transferred);
    }

    m_response.consume(m_response.size());
}

void
Session::on_write(const asio::error_code & err, size_t bytes_transferred)
{
    if (!check_error(err, bytes_transferred))
    {
        return;
    }

    if (m_write_handler != nullptr)
    {
        m_write_handler(bytes_transferred);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
