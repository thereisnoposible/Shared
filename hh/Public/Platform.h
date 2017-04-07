#ifndef _DEFINE_PLATFORM_H_
#define _DEFINE_PLATFORM_H_

//系统确定
#if defined(__linux__) && !defined(__APPLE__)
//pc linux
#	define ZS_LINUXOS
//unix标准
#	define ZS_UNIX

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//pc windows
#	define ZS_WINOS

#	ifdef _WIN32_WCE
#		define ZS_WINCE _WIN32_WCE//是windows ce
#	endif

#elif defined(__alpha__)

#	define ZS_ALPHAOS
#	define ZS_UNIX

#elif (defined(__MACH__) && defined(__APPLE__) || defined(__APPLE_CC__) || defined(macintosh))&& !defined(ANDROID) 
#	define ZS_APPLEOS //苹果的系统 包括 mac os 和 ios
#	define ZS_UNIX
#	if TARGET_OS_IPHONE
#		define ZS_IOS
#	elif TARGET_OS_MAC
#		define ZS_MACOS
#	endif

#elif defined(ANDROID)

#	define ZS_ANDROIDOS
#	define ZS_UNIX

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)

#	define ZS_FREEBSDOS
#	define ZS_UNIX

#endif

//////////////////////////////////////////////////////////////////////////////////////////
//编译器确定
#if defined(__GNU__) || defined(__GLIBC__)
//gcc下
#	define ZS_GUNC
#pragma GCC system_header

#elif defined(_MSC_VER) || defined(_MSC_FULL_VER)
//vc下
#	define ZS_MSVC

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)

#	ifndef ZS_POSIX
#		define ZS_POSIX
#	endif

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//确定32位还是 64位
/*

*/
#if defined(__i386) || defined(__i686) || defined(__i386__) || defined(__i686__) || defined(_M_IX86)
//32位
#	define ZS_X86

#	if defined(__i386) || defined(__i386__)
#		define ZS_I386
#		define ZS_ARCH_STRING "i386"
#	elif defined(__i686) || defined(__i686__)
#		define ZS_I686
#		define ZS_ARCH_STRING "i686"
#	elif defined(_M_IX86)
#		if (_M_IX86 == 300)
#			define ZS_I386
#			define ZS_ARCH_STRING "i386"
#		elif (_M_IX86 == 400)
#			define ZS_I486
#           define ZS_ARCH_STRING "i486"
#		elif (_M_IX86 == 500 || _M_IX86 == 600)
#			define ZS_PENTIUM
#           define ZS_ARCH_STRING "Pentium"
#		endif
#	endif

#elif defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_IA64) || defined(_M_X64)
//64位
#	define ZS_X64

#	if defined(__x86_64)
#		define ZS_X86_64
#		define ZS_ARCH_STRING "x86_64"
#	elif defined(__amd64__) || defined(__amd64)
#		define ZS_AMD64
#		define ZS_ARCH_STRING "amd64"
#	else
#		define ZS_X64_EX
#		define ZS_ARCH_STRING "x64"
#	endif

#elif defined(__arm__) || defined(__arm64) || defined(__arm64__) || (defined(__aarch64__) && __aarch64__)
//arm平台
#	define ZS_ARM
#   if defined(__ARM64_ARCH_8__)
#       define ZS_ARCH_ARM64
#       define ZS_ARCH_ARM_VERSION          (8)
#       define ZS_ARCH_ARM_v8
#       define ZS_ARCH_STRING              "arm64"
#   elif defined(__ARM_ARCH_7A__)
#       define ZS_ARCH_ARM_VERSION          (7)
#       define ZS_ARCH_ARM_v7A
#       define ZS_ARCH_STRING              "armv7a"
#   elif defined(__ARM_ARCH_7__)
#       define ZS_ARCH_ARM_VERSION          (7)
#       define ZS_ARCH_ARM_v7
#       define ZS_ARCH_STRING              "armv7"
#   elif defined(__ARM_ARCH_6__)
#       define ZS_ARCH_ARM_VERSION          (6)
#       define ZS_ARCH_ARM_v6
#       define ZS_ARCH_STRING              "armv6"
#   elif defined(__ARM_ARCH_5TE__)
#       define ZS_ARCH_ARM_VERSION          (5)
#       define ZS_ARCH_ARM_v5te
#       define ZS_ARCH_STRING              "armv5te"
#   elif defined(__ARM_ARCH_5__)
#       define ZS_ARCH_ARM_VERSION          (5)
#       define ZS_ARCH_ARM_v5
#       define ZS_ARCH_STRING              "armv5"
#   elif defined(__ARM_ARCH)
#       define ZS_ARCH_ARM_VERSION          __ARM_ARCH
#       if __ARM_ARCH >= 8
#           define ZS_ARCH_ARM_v8
#           if defined(__arm64) || defined(__arm64__)
#               define ZS_ARCH_ARM64
#               define ZS_ARCH_STRING       "arm64"
#           else
#               define ZS_ARCH_STRING       "armv7s"
#           endif
#       elif __ARM_ARCH >= 7
#           define ZS_ARCH_ARM_v7
#           define ZS_ARCH_STRING          "armv7"
#       elif __ARM_ARCH >= 6
#           define ZS_ARCH_ARM_v6
#           define ZS_ARCH_STRING          "armv6"
#       else
#           define ZS_ARCH_ARM_v5
#           define ZS_ARCH_STRING           "armv5"
#       endif
#   elif defined(__aarch64__) && __aarch64__
#       define ZS_ARCH_ARM_v8
#       define ZS_ARCH_ARM64
#       define ZS_ARCH_STRING               "arm64-v8a"
#   else 
#       error unknown arm arch version
#   endif
#   if !defined(TB_ARCH_ARM64) && (defined(__arm64) || defined(__arm64__))
#       define ZS_ARCH_ARM64
#       ifndef ZS_ARCH_STRING
#           define ZS_ARCH_STRING           "arm64"
#       endif
#   endif
#   ifndef ZS_ARCH_STRING
#       define ZS_ARCH_STRING               "arm"
#   endif
#   if defined(__thumb__)
#       define ZS_ARCH_ARM_THUMB
#       define ZS_ARCH_STRING_2             "_thumb"
#   endif
#   if defined(__ARM_NEON__)
#       define ZS_ARCH_ARM_NEON
#       define ZS_ARCH_STRING_3             "_neon"
#   endif 
#elif defined(mips) || defined(_mips) || defined(__mips__)
//mips平台
#   define ZS_MIPS
#   define ZS_ARCH_STRING "mips"
#endif

// sse
#if defined(ZS_X86) || defined(ZS_X64)
#   if defined(__SSE__)
#       define ZS_SSE
#       define ZS_SSE_STRING "_sse"
#   endif
#   if defined(__SSE2__)
#       define ZS_SSE2
#       undef ZS_SSE_STRING
#       define ZS_SSE_STRING "_sse2"
#   endif
#   if defined(__SSE3__)
#       define ZS_SSE3
#       undef ZS_SSE_STRING
#       define ZS_SSE_STRING "_sse3"
#   endif
#endif

// elf
#if defined(__ELF__)
#   define ZS_ELF
#   define ZS_ELF_STRING "_elf"
#endif

#ifndef ZS_ELF_STRING
#   define ZS_ELF_STRING ""
#endif

// vfp
#if defined(__VFP_FP__)
#   define ZS_VFP
#   define ZS_VFP_STRING "_vfp"
#endif

#ifndef ZS_VFP_STRING
#   define ZS_VFP_STRING ""
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) \
    || defined(__amd64__) || defined(__arm64) || defined(__arm64__) || defined(__sparc64__) || defined(__PPC64__) || defined(__ppc64__) \
    || defined(__powerpc64__) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || (defined(__WORDSIZE) && (__WORDSIZE == 64))

#define ZS_BIT64
#define ZS_BIT_SIZE (64)
#define ZS_BIT_BYTE (8)

#else

#define ZS_BIT32
#define ZS_BIT_SIZE (32)
#define ZS_BIT_BYTE (4)

#endif

#endif
