#ifndef MND_ARCH_HEADER
#define MND_ARCH_HEADER

#include <cstddef> // for __GLIBC__

// C++11 and below don't support contexpr as used here.
#if __cplusplus >= 201300
# define MND_CONSTEXPR constexpr 
#else
# define MND_CONSTEXPR
#endif

// Ported from boost/predef/other/endian.h.

//  Copyright (C) 2012 David Stone
//  Copyright Beman Dawes 2013

//  Distributed under the Boost Software License, Version 1.0.
//  http://www.boost.org/LICENSE_1_0.txt
#if defined(__GLIBC__) \
 || defined(__GLIBCXX__) \
 || defined(__GNU_LIBRARY__) \
 || defined(__ANDROID__)
# include <endian.h>
#else
# if defined(macintosh) \
  || defined(Macintosh) \
  || (defined(__APPLE__) && defined(__MACH__))
#  include <machine/endian.h>
# elif defined(BSD) || defined(_SYSTYPE_BSD)
#  if defined(__OpenBSD__)
#   include <machine/endian.h>
#  else
#   include <sys/endian.h>
#  endif
# endif
#endif

#if defined(__BYTE_ORDER)
# if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
#  define MND_BIG_ENDIAN
# elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define MND_LITTLE_ENDIAN
# else
#  define MND_UNKNOWN_ENDIANNESS
# endif
#elif defined(_BYTE_ORDER)
# if defined(_BIG_ENDIAN) && (_BYTE_ORDER == _BIG_ENDIAN)
#  define MND_BIG_ENDIAN
# elif defined(_LITTLE_ENDIAN) && (_BYTE_ORDER == _LITTLE_ENDIAN)
#  define MND_LITTLE_ENDIAN
# else
#  define MND_UNKNOWN_ENDIANNESS
# endif
#else
# define MND_UNKNOWN_ENDIANNESS
#endif

#ifndef __has_builtin
  #define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

#if defined(_MSC_VER)
// Microsoft documents these as being compatible since Windows 95 and
// specifically lists runtime library support since Visual Studio 2003 (aka
// 7.1).  Clang/c2 uses the Microsoft rather than GCC intrinsics, so we check
// for defined(_MSC_VER) before defined(__clang__).
# include <cstdlib>
# define MND_BYTE_SWAP_16(x) _byteswap_ushort(x)
# define MND_BYTE_SWAP_32(x) _byteswap_ulong(x)
# define MND_BYTE_SWAP_64(x) _byteswap_uint64(x)

//  GCC and Clang recent versions provide intrinsic byte swaps via builtins.
#elif (defined(__clang__) \
  && __has_builtin(__builtin_bswap32) \
  && __has_builtin(__builtin_bswap64)) \
  || (defined(__GNUC__ ) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
// Prior to 4.8, gcc did not provide __builtin_bswap16 on some platforms so we emulate it
// see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624.
// Clang has a similar problem, but their feature test macros make it easier to detect.
# if (defined(__clang__) && __has_builtin(__builtin_bswap16)) \
  || (defined(__GNUC__) &&(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)))
#   define MND_BYTE_SWAP_16(x) __builtin_bswap16(x)
# else
#   define MND_BYTE_SWAP_16(x) __builtin_bswap32((x) << 16)
# endif
# define MND_BYTE_SWAP_32(x) __builtin_bswap32(x)
# define MND_BYTE_SWAP_64(x) __builtin_bswap64(x)

//  Linux systems provide the byteswap.h header.
#elif defined(__linux__)
# include <byteswap.h>
# define MND_BYTE_SWAP_16(x) bswap_16(x)
# define MND_BYTE_SWAP_32(x) bswap_32(x)
# define MND_BYTE_SWAP_64(x) bswap_64(x)

// We need to provide our own implementation.
#else
namespace endian {
namespace detail {

constexpr uint16_t swap_u16(uint16_t n) noexcept
{
    return (n << 8) | (n >> 8);
}

constexpr uint32_t swap_u32(uint32_t n) noexcept
{
    n = ((n << 8) & 0xFF00FF00) | ((n >> 8) & 0xFF00FF);
    return (n << 16) | (n >> 16);
}

constexpr uint64_t swap_u64(uint64_t n) noexcept
{
    n = ((n << 8)  & 0xFF00FF00FF00FF00ull) | ((n >> 8)  & 0x00FF00FF00FF00FFull);
    n = ((n << 16) & 0xFFFF0000FFFF0000ull) | ((n >> 16) & 0x0000FFFF0000FFFFull);
    return (n << 32) | (n >> 32);
}

} // detail
} // endian
# define MND_BYTE_SWAP_16(x) endian::detail::swap_u16(static_cast<uint16_t>(x))
# define MND_BYTE_SWAP_32(x) endian::detail::swap_u32(static_cast<uint32_t>(x))
# define MND_BYTE_SWAP_64(x) endian::detail::swap_u64(static_cast<uint64_t>(x))
#endif

#endif // MND_ARCH_HEADER
