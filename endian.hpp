#ifndef MND_ENDIAN_HEADER
#define MND_ENDIAN_HEADER

#include <cstdint>

namespace endian {

// Tags for marking the desired endianness.
struct big {};
struct little {};
using network = big;
#ifdef ENDIAN_BIG
using host = big;
//#elif ENDIAN_LITTLE
#else // TODO for now
using host = little;
//#else
//# error "Architecture endianness not supported."
#endif

namespace detail {
template<typename T, typename InputIt>
constexpr T parse(InputIt it, big _) noexcept;
template<typename T, typename InputIt>
constexpr T parse(InputIt it, little _) noexcept;
template<typename T, typename OutputIt>
constexpr void write(const T& h, OutputIt it, big _) noexcept;
template<typename T, typename OutputIt>
constexpr void write(const T& h, OutputIt it, little _) noexcept;
} // detail

/**
 * Parses `sizeof(T)` bytes from the memory pointed to by `it`, and reconstructs from it
 * an integer of type `T`, converting from the specified `Endianness` to host byte order.
 *
 * The byte sequence must have at least `sizeof(T)` bytes.
 * `Endianness` must be either `endian::big`, `endian::little`, `endian::network`, or
 * `endian::host`.
 *
 * This is best used when data received during IO is read into a buffer and numbers
 * need to be parsed from it. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * // Receive into `buffer`.
 * // ...
 * // The first four bytes constitute a 32-bit integer, represented as big endian in the
 * // buffer, so parse the beginning of the buffer.
 * int32_t n = endian::parse<endian::big, int32_t>(buffer.data());
 * ```
 */
template<typename Endianness, typename T, typename InputIt>
constexpr T parse(InputIt it) noexcept
{
    return detail::parse<T>(it, Endianness());
}

/**
 * Writes each byte of `h` to the memory pointed to by `it`, such that it converts the
 * byte order of `h` from host byte order to the specified `Endianness`.
 *
 * The byte sequence must have at least `sizeof(T)` bytes.
 * `Endianness` must be either `endian::big`, `endian::little`, `endian::network`, or
 * `endian::host`.
 *
 * This is best used when data transferred during IO is written to a buffer first, and
 * among the data to be written are integers. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * const int32_t number = 42;
 * // Write `number` as a big endian number to `buffer`.
 * endian::write<endian::big>(number, &buffer[0]);
 * // Write `number` as a little endian number to `buffer`.
 * endian::write<endian::little>(number, &buffer[4]);
 * ```
 */
template<typename Endianness, typename T, typename InputIt>
constexpr T write(const T& h, InputIt it) noexcept
{
    detail::write<T>(h, it, Endianness());
}

// ~ * ~

namespace detail {

/**
 * Parses an integer of type `T` from buffer pointed to by `it` and converts it to big
 * endian order.
 */
template<typename T, typename InputIt>
constexpr T parse(InputIt it, big _) noexcept
{
    T h = 0;
    for(int i = 0; i < int(sizeof h); ++i)
    {
        h <<= 8;
        h |= static_cast<uint8_t>(*it++);
    }
    return h;
}

/**
 * Parses an integer of type `T` from buffer pointed to by `it` and converts it to
 * little endian order.
 */
template<typename T, typename InputIt>
constexpr T parse(InputIt it, little _) noexcept
{
    T h = 0;
    for(int i = 0; i < int(sizeof h); ++i)
    {
        h |= static_cast<uint8_t>(*it++) << i * 8;
    }
    return h;
}

/** Converts `h` to big endian order, writing it to buffer pointed to by `it`. */
template<typename T, typename OutputIt>
constexpr void write(const T& h, OutputIt it, big _) noexcept
{
    for(int shift = 8 * (int(sizeof h) - 1); shift >= 0; shift -= 8)
    {
        *it++ = static_cast<uint8_t>((h >> shift) & 0xff);
    }
}

/** Converts `h` to little endian order, writing it to buffer pointed to by `it`. */
template<typename T, typename OutputIt>
constexpr void write(const T& h, OutputIt it, little _) noexcept
{
    for(int i = 0; i < int(sizeof h); ++i)
    {
        *it++ = static_cast<uint8_t>((h >> i * 8) & 0xff);
    }
}

} // detail
} // endian

#endif // MND_ENDIAN_HEADER
