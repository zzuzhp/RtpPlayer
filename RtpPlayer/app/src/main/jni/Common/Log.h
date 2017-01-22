#ifndef ___LOG_H___
#define ___LOG_H___

#include "pro_time_util.h"
#include "Common/Utils.h"

#include <sstream>
#include <vector>
#include <android/log.h>

#define TAG "RTPPLAYER"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

typedef enum
{
    LOG_LEVEL_VERBOSE   = 0,
    LOG_LEVEL_DEBUG     = 1,
    LOG_LEVEL_INFO      = 2,
    LOG_LEVEL_WARNING   = 3,
    LOG_LEVEL_ERROR     = 4,
    LOG_LEVEL_FATAL     = 5
} LOGLevel;

static
std::ostream & operator << (std::ostream & lhs, const LOGLevel & rhs)
{
    switch (rhs)
    {
        case LOG_LEVEL_VERBOSE: lhs << "[verbose]"; break;
        case LOG_LEVEL_DEBUG:   lhs << "[debug  ]"; break;
        case LOG_LEVEL_INFO:    lhs << "[info   ]"; break;
        case LOG_LEVEL_WARNING: lhs << "[warning]"; break;
        case LOG_LEVEL_ERROR:   lhs << "[error  ]"; break;
        case LOG_LEVEL_FATAL:   lhs << "[fatal  ]"; break;
    }

    return lhs;
}

struct LOGMetadata
{
    std::string to_string() const
    {
        std::stringstream ss;
        ss << m_level << " " << m_location;
        return ss.str();
    }

    LOGLevel       m_level;
    std::string    m_location;
};

class Logger
{
public:

    virtual ~Logger() {}

    virtual void write (const LOGMetadata & meta, const std::string &text) = 0;

    void set_timestamp_enabled(bool enable = true) { m_timeStamp_enabled = enable; }

    bool timestamp_enabled() const { return m_timeStamp_enabled; }

protected:

    Logger() : m_timeStamp_enabled(false)
    {

    }

    void write_default (std::ostream & stream, const LOGMetadata & meta, const std::string & text)
    {
        stream << meta.m_level << " ";

        if (timestamp_enabled())
        {
            std::string time_text;
            ProGetTimeText(time_text);

            stream << time_text << " ";
        }

        stream << meta.m_location << " " << text << std::endl;
    }

private:

    bool m_timeStamp_enabled;
};

/* prints log messages in the application console window. */
class LoggerConsole : public Logger
{
public:

    void write(const LOGMetadata & meta, const std::string & text)
    {

    }
};

class LoggerSystem : public Logger
{
public:

    void write (const LOGMetadata & meta, const std::string & text)
    {
        std::stringstream ss;
        write_default(ss, meta, text);

        android_LogPriority prio = ANDROID_LOG_DEFAULT;
        switch (meta.m_level)
        {
            case LOG_LEVEL_VERBOSE: prio = ANDROID_LOG_VERBOSE; break;
            case LOG_LEVEL_INFO:    prio = ANDROID_LOG_INFO;    break;
            case LOG_LEVEL_DEBUG:   prio = ANDROID_LOG_DEBUG;   break;
            case LOG_LEVEL_WARNING: prio = ANDROID_LOG_WARN;    break;
            case LOG_LEVEL_ERROR:   prio = ANDROID_LOG_ERROR;   break;
            case LOG_LEVEL_FATAL:   prio = ANDROID_LOG_FATAL;   break;
        }

        __android_log_print(prio, TAG, ss.str().c_str());
    }
};

class LogManager : public Singleton<LogManager>
{
    friend class Singleton<LogManager>;

public:

    template <typename T>
    T * create_logger ()
    {
        T * logger = new T();
        logger->set_timestamp_enabled(true);

        add_logger(logger);

        return logger;
    }

    void write(const LOGMetadata & meta, const std::string & text)
    {
        std::vector<Logger *>::iterator itr = m_loggers.begin();
        for (; itr != m_loggers.end(); ++itr)
        {
            (*itr)->write(meta, text);
        }
    }

private:

    void add_logger (Logger * logger)
    {
        m_loggers.push_back(logger);
    }

protected:

    LogManager()
    {

    }

    std::vector<Logger *>  m_loggers;
};

static
LogManager * manager()
{
    return LogManager::instance();
}

template<typename T>
void create_logger ()
{
    manager()->create_logger<T>();
}

struct LOGEntry
{
    LOGEntry (LOGLevel level, const std::string location)
    {
        m_metaData.m_level    = level;
        m_metaData.m_location = location;
    }

    ~LOGEntry()
    {
        if (m_stream.tellp() > 0)
        {
            write_to_log();
        }
    }

    template <typename T>
    LOGEntry & operator << (const T & rhs)
    {
        m_stream << rhs;
        return *this;
    }

    void write_to_log()
    {
        manager()->write (m_metaData, m_stream.str());
    }

    const LOGMetadata & metadata() const { return m_metaData; }

private:

    LOGMetadata            m_metaData;
    std::stringstream      m_stream;
};

#define RP_LOG_STREAM(location, level, stream)    {LOGEntry(level, location) << stream;}

static
void RP_LOG_DEF (std::string location, LOGLevel level, const char * fmt, ...)
{
    static char log_msg[1024] = {0};

    va_list args;
    va_start(args, fmt);
    vsprintf(log_msg, fmt, args);
    va_end(args);

    RP_LOG_STREAM(location, level, log_msg);
}

#ifndef RP_MIN_LOG_LEVEL
    #ifndef NDEBUG
        #define RP_MIN_LOG_LEVEL LOG_LEVEL_VERBOSE  ///< debug mode default is LOG_LEVEL_VERBOSE
    #else
        #define RP_MIN_LOG_LEVEL LOG_LEVEL_INFO     ///< release mode default is LOG_LEVEL_INFO
    #endif
#endif

#if (RP_MIN_LOG_LEVEL <= LOG_LEVEL_VERBOSE)
    #define RP_LOG_V(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_VERBOSE, __VA_ARGS__)
#else
    #define RP_LOG_V(...)   ((void)0)
#endif

#if (RP_MIN_LOG_LEVEL <= LOG_LEVEL_DEBUG)
    #define RP_LOG_D(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
    #define RP_LOG_D(...)   ((void)0)
#endif

#if (RP_MIN_LOG_LEVEL <= LOG_LEVEL_INFO)
    #define RP_LOG_I(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_INFO, __VA_ARGS__)
#else
    #define RP_LOG_I(...)   ((void)0)
#endif

#if( RP_MIN_LOG_LEVEL <= LOG_LEVEL_WARNING)
    #define RP_LOG_W(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_WARNING,__VA_ARGS__)
#else
    #define RP_LOG_W(...)   ((void)0)
#endif

#if (RP_MIN_LOG_LEVEL <= LOG_LEVEL_ERROR)
    #define RP_LOG_E(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_ERROR, __VA_ARGS__)
#else
    #define RP_LOG_E(...)   ((void)0)
#endif

#if (RP_MIN_LOG_LEVEL <= LOG_LEVEL_FATAL)
    #define RP_LOG_F(...)   RP_LOG_DEF (WHERE_AM_I, LOG_LEVEL_FATAL, __VA_ARGS__)
#else
    #define RP_LOG_F(...)   ((void)0)
#endif

#define RP_FOOTPRINT        RP_LOG_D("here i am!");

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___LOG_H___
