//////////////////////////////////////////////////////////////////////////
// 公共类型头文件
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_TYPES_H__
#define __EV_TYPES_H__

#include "event2/event-config.h"

// 常用跨平台宏定义
// 使用ev_thread_local修饰的变量在每一个线程都拥有一份实体，各个线程的值互不干扰
// 只能修饰POD类型(类似整型指针的标量，不带自定义的构造、拷贝、赋值、析构的类型)
// 不能修饰class类型，不能修饰函数的局部变量或者class的普通成员变量
// 可以用于修饰全局变量，函数内的静态变量
// 当修饰静态变量时(全局静态变量和局部静态变量)，要位于static的后面，比如：
// static ev_thread_local char msg_buffer[128];
#if defined(__EV_WINDOWS__)
    #define __EV_FILE__     __FILE__
    #define __EV_FUNC__     __FUNCTION__
    #define __EV_LINE__     __LINE__
    #define ev_thread_local __declspec(thread)
#elif defined(__EV_LINUX__)
    #define __EV_FILE__     __FILE__
    #define __EV_FUNC__     __PRETTY_FUNCTION__
    #define __EV_LINE__     __LINE__
    #define ev_thread_local __thread
#else
    #error "Unrecognized os platform!"
#endif // defined(__EV_WINDOWS__)

// 使用该宏可以避免出现未使用的变量的编译警告
#define EV_UNUSED_VARIABLE(var) do { (void)(var); } while (false)

namespace evpp {

// 基础类型定义
typedef signed   char          int8;
typedef unsigned char          uint8;
typedef signed   short         int16;
typedef unsigned short         uint16;
typedef signed   int           int32;
typedef unsigned int           uint32;
#if defined(__EV_WINDOWS__)
    typedef signed   long long int int64;
    typedef unsigned long long int uint64;
    #define EV_I64D                "%lld"
    #define EV_I64U                "%llu"
#elif defined(__EV_LINUX__)
    #if defined(__EV_WORDSIZE64__)
        typedef signed   long int      int64;
        typedef unsigned long int      uint64;
        #define EV_I64D                "%ld"
        #define EV_I64U                "%lu"
    #else
        typedef signed   long long int int64;
        typedef unsigned long long int uint64;
        #define EV_I64D                "%lld"
        #define EV_I64U                "%llu"
    #endif // defined(__EV_WORDSIZE64__)
#else
    #error "Unrecognized os platform!"
#endif // defined(__EV_WINDOWS__)

// socket类型定义
#if defined(__EV_WINDOWS__) && defined(__EV_WORDSIZE64__)
    typedef int64   socket_t;
    #define EV_SOCK EV_I64D
#else
    typedef int32   socket_t;
    #define EV_SOCK "%d"
#endif // defined(__EV_WINDOWS__) && defined(__EV_WORDSIZE64__)

//////////////////////////////////////////////////////////////////////////
// ev_reference
//////////////////////////////////////////////////////////////////////////
template<typename T> struct ev_reference                      { typedef T&                  type; };
template<typename T> struct ev_reference<T&>                  { typedef T&                  type; };
template<>           struct ev_reference<void>                { typedef void                type; };
template<>           struct ev_reference<const void>          { typedef const void          type; };
template<>           struct ev_reference<volatile void>       { typedef volatile void       type; };
template<>           struct ev_reference<const volatile void> { typedef const volatile void type; };

} // namespace evpp

#endif // __EV_TYPES_H__