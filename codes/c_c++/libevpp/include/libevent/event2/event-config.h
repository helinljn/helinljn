/*
 * Copyright (c) 2000-2007 Niels Provos <provos@citi.umich.edu>
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef EVENT2_EVENT_CONFIG_H_INCLUDED_
#define EVENT2_EVENT_CONFIG_H_INCLUDED_

#if (defined(_MSC_VER) && !defined(__INTEL_COMPILER)) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    #if !defined(__EV_WINDOWS__)
        #define __EV_WINDOWS__
    
        #if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0600)
            // Only Windows Vista and Windows Server 2008 and later versions are supported!
            #undef  _WIN32_WINNT
            #define _WIN32_WINNT 0x0600
        #endif // !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0600)
    #endif // !defined(__EV_WINDOWS__)
#elif defined(__linux__)
    #if !defined(__EV_LINUX__)
        #define __EV_LINUX__
    #endif // !defined(__EV_LINUX__)
#else
    #error "Unrecognized os platform!"
#endif // (defined(_MSC_VER) && !defined(__INTEL_COMPILER)) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)

#if defined(__cplusplus)
    #if !defined(__STDC_LIMIT_MACROS)
        #define __STDC_LIMIT_MACROS
    #endif // !defined(__STDC_LIMIT_MACROS)

    #if !defined(__STDC_CONSTANT_MACROS)
        #define __STDC_CONSTANT_MACROS
    #endif // !defined(__STDC_CONSTANT_MACROS)
#endif // defined(__cplusplus)

#include <stdint.h>

#if !defined(__EV_WORDSIZE64__) && !defined(__EV_WORDSIZE32__)
    #if UINTPTR_MAX >= UINT64_MAX
        #define __EV_WORDSIZE64__
    #elif UINTPTR_MAX >= UINT32_MAX
        #define __EV_WORDSIZE32__
    #else
        #error "Unrecognized os platform!"
    #endif // UINTPTR_MAX >= UINT64_MAX
#endif // !defined(__EV_WORDSIZE64__) && !defined(__EV_WORDSIZE32__)

#if defined(__EV_WINDOWS__)
    #include <event2/event-config-windows.h>
#elif defined(__EV_LINUX__)
    #include <event2/event-config-linux.h>
#else
    #error "Unrecognized os platform!"
#endif // defined(__EV_WINDOWS__)

#endif // EVENT2_EVENT_CONFIG_H_INCLUDED_
