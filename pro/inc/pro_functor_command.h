#if !defined(____PRO_FUNCTOR_COMMAND_H____)
#define ____PRO_FUNCTOR_COMMAND_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_z.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

class IProFunctorCommand
{
public:
    
    virtual void PRO_STDCALL Destroy() = 0;
    
    virtual void PRO_STDCALL Execute() = 0;
    
    virtual void PRO_STDCALL SetUserData(const void* userData) = 0;
    
    virtual const void* PRO_STDCALL GetUserData() const = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

template<typename RECEIVER, typename ACTION, unsigned long argCount = 9>
class CProFunctorCommand_cpp : public IProFunctorCommand, public CProCopyDisabled
{
public:
    
    static CProFunctorCommand_cpp* CreateInstance(
        RECEIVER& receiver,
        ACTION    action,
        long      arg1 = 0,
        long      arg2 = 0,
        long      arg3 = 0,
        long      arg4 = 0,
        long      arg5 = 0,
        long      arg6 = 0,
        long      arg7 = 0,
        long      arg8 = 0,
        long      arg9 = 0
        )
    {
        assert(action != NULL);
        assert(argCount <= 9);
        if (action == NULL || argCount > 9)
        {
            return (NULL);
        }
        
        CProFunctorCommand_cpp* const command = new CProFunctorCommand_cpp(
            receiver,
            action,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7,
            arg8,
            arg9
            );
        
        return (command);
    }
    
private:
    
    CProFunctorCommand_cpp(
        RECEIVER& receiver,
        ACTION    action,
        long      arg1,
        long      arg2,
        long      arg3,
        long      arg4,
        long      arg5,
        long      arg6,
        long      arg7,
        long      arg8,
        long      arg9
        ) : m_receiver(receiver)
    {
        m_userData = NULL;
        m_action   = action;
        
        const long args[9] = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 };
        memcpy(m_args, args, sizeof(m_args));
    }
    
    virtual ~CProFunctorCommand_cpp()
    {
    }
    
    virtual void PRO_STDCALL Destroy()
    {
        delete this;
    }
    
    virtual void PRO_STDCALL Execute()
    {
        if (argCount == 0)
        {
            (m_receiver.*m_action)(NULL);
        }
        else
        {
            (m_receiver.*m_action)((long*)m_args);
        }
    }
    
    virtual void PRO_STDCALL SetUserData(const void* userData)
    {
        m_userData = userData;
    }
    
    virtual const void* PRO_STDCALL GetUserData() const
    {
        return (m_userData);
    }
    
private:
    
    const void* m_userData;
    RECEIVER&   m_receiver;
    ACTION      m_action;
    long        m_args[argCount + 1];
};

/////////////////////////////////////////////////////////////////////////////
////

template<typename ACTION, unsigned long argCount = 9>
class CProFunctorCommand_c : public IProFunctorCommand, public CProCopyDisabled
{
public:
    
    static CProFunctorCommand_c* CreateInstance(
        ACTION action,
        long   arg1 = 0,
        long   arg2 = 0,
        long   arg3 = 0,
        long   arg4 = 0,
        long   arg5 = 0,
        long   arg6 = 0,
        long   arg7 = 0,
        long   arg8 = 0,
        long   arg9 = 0
        )
    {
        assert(action != NULL);
        assert(argCount <= 9);
        if (action == NULL || argCount > 9)
        {
            return (NULL);
        }
        
        CProFunctorCommand_c* const command = new CProFunctorCommand_c(
            action,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7,
            arg8,
            arg9
            );
        
        return (command);
    }
    
private:
    
    CProFunctorCommand_c(
        ACTION action,
        long   arg1,
        long   arg2,
        long   arg3,
        long   arg4,
        long   arg5,
        long   arg6,
        long   arg7,
        long   arg8,
        long   arg9
        )
    {
        m_userData = NULL;
        m_action   = action;
        
        const long args[9] = { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 };
        memcpy(m_args, args, sizeof(m_args));
    }
    
    virtual ~CProFunctorCommand_c()
    {
    }
    
    virtual void PRO_STDCALL Destroy()
    {
        delete this;
    }
    
    virtual void PRO_STDCALL Execute()
    {
        if (argCount == 0)
        {
            (*m_action)(NULL);
        }
        else
        {
            (*m_action)((long*)m_args);
        }
    }
    
    virtual void PRO_STDCALL SetUserData(const void* userData)
    {
        m_userData = userData;
    }
    
    virtual const void* PRO_STDCALL GetUserData() const
    {
        return (m_userData);
    }
    
private:
    
    const void* m_userData;
    ACTION      m_action;
    long        m_args[argCount + 1];
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_FUNCTOR_COMMAND_H____
