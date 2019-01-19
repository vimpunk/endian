#ifndef MND_ENDIAN_HEADER
#define MND_ENDIAN_HEADER

#include "detail/arch.hpp"
#include "detail/type_traits.hpp"

namespace endian {

enum class order {
    little,
    big,
    network = big,
#if defined(MND_BIG_ENDIAN)
    host = big,
#elif defined(MND_LITTLE_ENDIAN)
    host = little,
#endif
};

/**
 * Parses `sizeof(T)` bytes from the memory pointed to by `it`, and reconstructs from it
 * an integer of type `T`, converting from the specified `Order` to host byte order.
 *
 * The value type of the iterator must represent a byte, that is:
 * `sizeof(typename std::iterator_traits<InputIt>::value_type) == sizeof(char)`.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `sizeof(T)` bytes.
 *
 * The byte sequence must have at least `sizeof(T)` bytes.
 * `Order` must be either `endian::order::big`, `endian::order::little`,
 * `endian::order::network`, or `endian::order::host`.
 *
 * This is best used when data received during IO is read into a buffer and numbers
 * need to be read from it. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * // Receive into `buffer`.
 * // ...
 * // Assume that the first four bytes in `buffer` constitute a 32-bit big endian
 * // integer.
 * int32_t n = endian::read<endian::order::big, int32_t>(buffer.data());
 * ```
 */
template<order Order, class T, class InputIt>
MND_CONSTEXPR T read(InputIt it) noexcept;

template<class T, class InputIt>
MND_CONSTEXPR T read_le(InputIt it) noexcept
{
    return read<order::little, T>(it);
}

template<class T, class InputIt>
MND_CONSTEXPR T read_be(InputIt it) noexcept
{
    return read<order::big, T>(it);
}

/**
 * Parses `N` bytes from the memory pointed to by `it`, and reconstructs from it
 * an unsigned integer of type `T` that is at least as large to fit `N` bytes
 * (but at most 8 bytes), converting from the specified `Order` to host byte
 * order.
 *
 * The value type of the iterator must represent a byte, that is:
 * `sizeof(typename std::iterator_traits<InputIt>::value_type) == sizeof(char)`.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `sizeof(T)` bytes.
 *
 * The byte sequence must have at least `sizeof(T)` bytes.
 * `Order` must be either `endian::order::big`, `endian::order::little`,
 * `endian::order::network`, or `endian::order::host`.
 *
 * This is best used when data received during IO is read into a buffer and numbers
 * need to be read from it. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * // Receive into `buffer`.
 * // ...
 * // Assume that the first three bytes in `buffer` constitute a 24-bit big endian
 * // integer.
 * int32_t n = endian::read<endian::order::big, 3>(buffer.data());
 * ```
 */
template<order Order, size_t N, class InputIt,
    class T = typename detail::integral_type_for<N>::type>
MND_CONSTEXPR T read(InputIt it) noexcept;

template<size_t N, class InputIt,
    class T = typename detail::integral_type_for<N>::type>
MND_CONSTEXPR T read_le(InputIt it) noexcept
{
    return read<order::little, N>(it);
}

template<size_t N, class InputIt,
    class T = typename detail::integral_type_for<N>::type>
MND_CONSTEXPR T read_be(InputIt it) noexcept
{
    return read<order::big, N>(it);
}

// DEPRECATED: use `read` instead.
template<order Order, class T, class InputIt>
MND_CONSTEXPR T parse(InputIt it) noexcept { return read<Order, T>(it); }

/**
 * Writes each byte of `h` to the memory pointed to by `it`, such that it converts the
 * byte order of `h` from host byte order to the specified `Order`.
 *
 * The value type of the iterator must represent a byte, that is:
 * `sizeof(typename std::iterator_traits<InputIt>::value_type) == sizeof(char)`.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `sizeof(T)` bytes.
 *
 * The byte sequence must have at least `sizeof(T)` bytes.
 * `Order` must be either `endian::order::big`, `endian::order::little`,
 * `endian::order::network`, or `endian::order::host`.
 *
 * This is best used when data transferred during IO is written to a buffer first, and
 * among the data to be written are integers. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * const int32_t number = 42;
 * // Write `number` as a big endian number to `buffer`.
 * endian::write<endian::order::big>(number, &buffer[0]);
 * // Write `number` as a little endian number to `buffer`.
 * endian::write<endian::order::little>(number, &buffer[4]);
 * ```
 */
template<order Order, class T, class OutputIt>
MND_CONSTEXPR void write(const T& h, OutputIt it) noexcept;

template<class T, class OutputIt>
MND_CONSTEXPR void write_le(const T& h, OutputIt it) noexcept
{
    write<order::little>(h, it);
}

template<class T, class OutputIt>
MND_CONSTEXPR void write_be(const T& h, OutputIt it) noexcept
{
    write<order::big>(h, it);
}

/**
 * Writes the first `N` byte of `h` to the memory pointed to by `it`, such that it converts the
 * byte order of `h` from host byte order to the specified `Order`. Thus `N`
 * must be at most `sizeof(h)`.
 *
 * The value type of the iterator must represent a byte, that is:
 * `sizeof(typename std::iterator_traits<InputIt>::value_type) == sizeof(char)`.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `N` bytes.
 *
 * The byte sequence must have at least `N` bytes.
 * `Order` must be either `endian::order::big`, `endian::order::little`,
 * `endian::order::network`, or `endian::order::host`.
 *
 * This is best used when data transferred during IO is written to a buffer first, and
 * among the data to be written are integers. E.g.:
 * ```
 * std::array<char, 1024> buffer;
 * const int32_t number = 42;
 * // Write a three byte `number` as a big endian number to `buffer`.
 * endian::write<endian::order::big, 3>(number, &buffer[0]);
 * // Write a three byte `number` as a little endian number to `buffer`.
 * endian::write<endian::order::little, 3>(number, &buffer[4]);
 * ```
 */
template<order Order, size_t N, class T, class OutputIt>
MND_CONSTEXPR void write(const T& h, OutputIt it) noexcept;

template<size_t N, class T, class OutputIt>
MND_CONSTEXPR void write_le(const T& h, OutputIt it) noexcept
{
    write<order::little, N>(h, it);
}

template<size_t N, class T, class OutputIt>
MND_CONSTEXPR void write_be(const T& h, OutputIt it) noexcept
{
    write<order::big, N>(h, it);
}

/**
 * Reverses endianness, i.e. the byte order in `t`. E.g. given the 16-bit number
 * '0x1234', this function returns '0x4321'.
 */
template<class T>
MND_CONSTEXPR T reverse(const T& t);

#ifndef MND_UNKNOWN_ENDIANNESS
/// These are only available if your platform has a defined endianness.
/**
 * Conditionally converts to the specified endianness if and only if the host's byte
 * order differs from `Order`.
 */
template<order Order, class T>
MND_CONSTEXPR T conditional_convert(const T& t) noexcept;

/**
 * Conditionally converts to network byte order if and only if the host's byte order is
 * different from the network byte order.
 *
 * Functionally equivalent to the POSIX ntoh* functions or to this code:
 * ```
 * if(endian::order::host != endian::order::network)
 *     t = endian::reverse(t);
 * ```
 * Or:
 * ```
 * t = endian::conditional_convert<endian::order::network>(t);
 * ```
 */
template<class T>
MND_CONSTEXPR T host_to_network(const T& t);

/**
 * Conditionally converts to host byte order if and only if the host's byte order is
 * different from the network byte order.
 *
 * Functionally equivalent to the POSIX ntoh* functions or to this code:
 * ```
 * if(endian::order::host != endian::order::network)
 *     t = endian::reverse(t);
 * ```
 * Or:
 * ```
 * t = endian::conditional_convert<endian::order::network>(t);
 * ```
 */
template<class T>
MND_CONSTEXPR T network_to_host(const T& t);
#endif // MND_UNKNOWN_ENDIANNESS

} // endian

#include "detail/endian.ipp"

#endif // MND_ENDIAN_HEADER
