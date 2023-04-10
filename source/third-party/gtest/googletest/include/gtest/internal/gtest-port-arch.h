// Copyright 2015, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The Google C++ Testing and Mocking Framework (Google Test)
//
// This header file defines the GTEST_OS_* macro.
// It is separate from gtest-port.h so that custom/gtest-port.h can include it.

#ifndef GOOGLETEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_ARCH_H_
#define GOOGLETEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_ARCH_H_

// Determines the platform on which Google Test is compiled.
#ifdef __CYGWIN__
# define GTEST_OS_CYGWIN 1
# elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#  define GTEST_OS_WINDOWS_MINGW 1
#  define GTEST_OS_WINDOWS 1
#elif defined _WIN32
# define GTEST_OS_WINDOWS 1
# ifdef _WIN32_WCE
#  define GTEST_OS_WINDOWS_MOBILE 1
# elif defined(WINAPI_FAMILY)
#  include <winapifamily.h>
#  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   define GTEST_OS_WINDOWS_DESKTOP 1
#  elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
#   define GTEST_OS_WINDOWS_PHONE 1
#  elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#   define GTEST_OS_WINDOWS_RT 1
#  elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_TV_TITLE)
#   define GTEST_OS_WINDOWS_PHONE 1
#   define GTEST_OS_WINDOWS_TV_TITLE 1
#  else
    // WINAPI_FAMILY defined but no known partition matched.
    // Default to desktop.
#   define GTEST_OS_WINDOWS_DESKTOP 1
#  endif
# else
#  define GTEST_OS_WINDOWS_DESKTOP 1
# endif  // _WIN32_WCE
#elif defined __OS2__
# define GTEST_OS_OS2 1
#elif defined __APPLE__
# define GTEST_OS_MAC 1
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  define GTEST_OS_IOS 1
# endif
#elif defined __DragonFly__
# define GTEST_OS_DRAGONFLY 1
#elif defined __FreeBSD__
# define GTEST_OS_FREEBSD 1
#elif defined __Fuchsia__
# define GTEST_OS_FUCHSIA 1
#elif defined(__GLIBC__) && defined(__FreeBSD_kernel__)
# define GTEST_OS_GNU_KFREEBSD 1
#elif defined __linux__
# define GTEST_OS_LINUX 1
# if defined __ANDROID__
#  define GTEST_OS_LINUX_ANDROID 1
# endif
#elif defined __MVS__
# define GTEST_OS_ZOS 1
#elif defined(__sun) && defined(__SVR4)
# define GTEST_OS_SOLARIS 1
#elif defined(_AIX)
# define GTEST_OS_AIX 1
#elif defined(__hpux)
# define GTEST_OS_HPUX 1
#elif defined __native_client__
# define GTEST_OS_NACL 1
#elif defined __NetBSD__
# define GTEST_OS_NETBSD 1
#elif defined __OpenBSD__
# define GTEST_OS_OPENBSD 1
#elif defined __QNX__
# define GTEST_OS_QNX 1
#elif defined(__HAIKU__)
#define GTEST_OS_HAIKU 1
#elif defined ESP8266
#define GTEST_OS_ESP8266 1
#elif defined ESP32
#define GTEST_OS_ESP32 1
#elif defined(__XTENSA__)
#define GTEST_OS_XTENSA 1
#endif  // __CYGWIN__

#ifndef GTEST_OS_CYGWIN
# define GTEST_OS_CYGWIN 0
#endif

#ifndef GTEST_OS_WINDOWS_MINGW
# define GTEST_OS_WINDOWS_MINGW 0
#endif

#ifndef GTEST_OS_WINDOWS
# define GTEST_OS_WINDOWS 0
#endif

#ifndef GTEST_OS_WINDOWS
# define GTEST_OS_WINDOWS 0
#endif

#ifndef GTEST_OS_WINDOWS_MOBILE
# define GTEST_OS_WINDOWS_MOBILE 0
#endif

#ifndef GTEST_OS_WINDOWS_DESKTOP
# define GTEST_OS_WINDOWS_DESKTOP 0
#endif

#ifndef GTEST_OS_WINDOWS_PHONE
# define GTEST_OS_WINDOWS_PHONE 0
#endif

#ifndef GTEST_OS_WINDOWS_RT
# define GTEST_OS_WINDOWS_RT 0
#endif

#ifndef GTEST_OS_WINDOWS_PHONE
# define GTEST_OS_WINDOWS_PHONE 0
#endif

#ifndef GTEST_OS_WINDOWS_TV_TITLE
# define GTEST_OS_WINDOWS_TV_TITLE 0
#endif

#ifndef GTEST_OS_WINDOWS_DESKTOP
# define GTEST_OS_WINDOWS_DESKTOP 0
#endif

#ifndef GTEST_OS_WINDOWS_DESKTOP
# define GTEST_OS_WINDOWS_DESKTOP 0
#endif

#ifndef GTEST_OS_OS2
# define GTEST_OS_OS2 0
#endif

#ifndef GTEST_OS_MAC
# define GTEST_OS_MAC 0
#endif

#ifndef GTEST_OS_IOS
# define GTEST_OS_IOS 0
#endif

#ifndef GTEST_OS_DRAGONFLY
# define GTEST_OS_DRAGONFLY 0
#endif

#ifndef GTEST_OS_FREEBSD
# define GTEST_OS_FREEBSD 0
#endif

#ifndef GTEST_OS_FUCHSIA
# define GTEST_OS_FUCHSIA 0
#endif

#ifndef GTEST_OS_GNU_KFREEBSD
# define GTEST_OS_GNU_KFREEBSD 0
#endif

#ifndef GTEST_OS_LINUX
# define GTEST_OS_LINUX 0
#endif

#ifndef GTEST_OS_LINUX_ANDROID
# define GTEST_OS_LINUX_ANDROID 0
#endif

#ifndef GTEST_OS_ZOS
# define GTEST_OS_ZOS 0
#endif

#ifndef GTEST_OS_SOLARIS
# define GTEST_OS_SOLARIS 0
#endif

#ifndef GTEST_OS_AIX
# define GTEST_OS_AIX 0
#endif

#ifndef GTEST_OS_HPUX
# define GTEST_OS_HPUX 0
#endif

#ifndef GTEST_OS_NACL
# define GTEST_OS_NACL 0
#endif

#ifndef GTEST_OS_NETBSD
# define GTEST_OS_NETBSD 0
#endif

#ifndef GTEST_OS_OPENBSD
# define GTEST_OS_OPENBSD 0
#endif

#ifndef GTEST_OS_QNX
# define GTEST_OS_QNX 0
#endif

#ifndef GTEST_OS_HAIKU
# define GTEST_OS_HAIKU 0
#endif

#ifndef GTEST_OS_ESP8266
# define GTEST_OS_ESP8266 0
#endif

#ifndef GTEST_OS_ESP32
# define GTEST_OS_ESP32 0
#endif

#ifndef GTEST_OS_XTENSA
# define GTEST_OS_XTENSA 0
#endif

#ifndef GTEST_USES_PCRE
# define GTEST_USES_PCRE 0
#endif

#ifndef GTEST_HAS_DOWNCAST_
# define GTEST_HAS_DOWNCAST_ 0
#endif

#ifndef GTEST_HAS_MUTEX_AND_THREAD_LOCAL_
# define GTEST_HAS_MUTEX_AND_THREAD_LOCAL_ 0
#endif

#ifndef GTEST_HAS_NOTIFICATION_
# define GTEST_HAS_NOTIFICATION_ 0
#endif

#ifndef GTEST_HAS_ABSL
# define GTEST_HAS_ABSL 0
#endif

#ifndef GTEST_LINKED_AS_SHARED_LIBRARY
# define GTEST_LINKED_AS_SHARED_LIBRARY 0
#endif

#ifndef GTEST_CREATE_SHARED_LIBRARY
# define GTEST_CREATE_SHARED_LIBRARY 0
#endif

#ifndef GTEST_DONT_DEFINE_FAIL
# define GTEST_DONT_DEFINE_FAIL 0
#endif

#ifndef GTEST_DONT_DEFINE_SUCCEED
# define GTEST_DONT_DEFINE_SUCCEED 0
#endif

#ifndef GTEST_DONT_DEFINE_EXPECT_TRUE
# define GTEST_DONT_DEFINE_EXPECT_TRUE 0
#endif

#ifndef GTEST_DONT_DEFINE_EXPECT_FALSE
# define GTEST_DONT_DEFINE_EXPECT_FALSE 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_EQ
# define GTEST_DONT_DEFINE_ASSERT_EQ 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_NE
# define GTEST_DONT_DEFINE_ASSERT_NE 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_LE
# define GTEST_DONT_DEFINE_ASSERT_LE 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_LT
# define GTEST_DONT_DEFINE_ASSERT_LT 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_GE
# define GTEST_DONT_DEFINE_ASSERT_GE 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_GT
# define GTEST_DONT_DEFINE_ASSERT_GT 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_TRUE
# define GTEST_DONT_DEFINE_ASSERT_TRUE 0
#endif

#ifndef GTEST_DONT_DEFINE_ASSERT_FALSE
# define GTEST_DONT_DEFINE_ASSERT_FALSE 0
#endif

#ifndef GTEST_DONT_DEFINE_TEST
# define GTEST_DONT_DEFINE_TEST 0
#endif

#ifndef GTEST_USES_SIMPLE_RE
# define GTEST_USES_SIMPLE_RE 0
#endif

#ifndef GTEST_FOR_GOOGLE_
# define GTEST_FOR_GOOGLE_ 0
#endif

#endif  // GOOGLETEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_ARCH_H_
