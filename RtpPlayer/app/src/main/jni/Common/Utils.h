#ifndef ___UTILS_H___
#define ___UTILS_H___

#include "pro_thread_mutex.h"

#include <string>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#define __FILENAME__    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define WHERE_AM_I      WhereAMI(__FUNCTION__, __FILENAME__, __LINE__).where()

struct WhereAMI
{
    WhereAMI()
    {

    }

    WhereAMI (const std::string & function,
              const std::string & file,
              const size_t      & line) : m_function(function ), m_file(file), m_line(line)
    {

    }

    const std::string & file() const { return m_file; }
    const std::string & function() const { return m_function; }
    size_t line_number() const { return m_line; }

    const std::string where() const
    {
        std::ostringstream oss;
        oss << m_file << "[" <<  m_function << "(" << m_line << ")" << "]";
        return oss.str();
    }

private:

    std::string     m_function;
    std::string     m_file;
    size_t          m_line;
};

template<typename T>
class Singleton
{
public:

    static T * instance()
    {
        if (!m_instance)
        {
            CProThreadMutexGuard mon(m_lock);

            if (!m_instance)
            {
                m_instance = new T();
            }
        }

        return m_instance;
    }

    static void destroy()
    {
        delete m_instance;
        m_instance = NULL;
    }

protected:

    Singleton<T>()
    {

    }

    virtual ~Singleton<T>()
    {

    }

private:

    static CProThreadMutex m_lock;

    static T * m_instance;
};

template<typename T>
CProThreadMutex Singleton<T>::m_lock;

template<typename T>
T * Singleton<T>::m_instance = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___UTILS_H___
