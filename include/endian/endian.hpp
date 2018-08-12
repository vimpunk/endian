#ifndef MND_ENDIAN_HEADER
#define MND_ENDIAN_HEADER

#include "detail/arch.hpp"

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
 * The type of the iterator must represent a byte, that is:
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
template<order Order, typename T, typename InputIt>
constexpr T read(InputIt it) noexcept;
// DEPRECATED
template<order Order, typename T, typename InputIt>
constexpr T parse(InputIt it) noexcept { return parse<Order, T>(it); }

/**
 * Writes each byte of `h` to the memory pointed to by `it`, such that it converts the
 * byte order of `h` from host byte order to the specified `Order`.
 *
 * The type of the iterator must represent a byte, that is:
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
template<order Order, typename T, typename OutputIt>
constexpr T write(const T& h, OutputIt it) noexcept;

/**
 * Reverses endianness, i.e. the byte order in `t`. E.g. given the 16-bit number
 * '0x1234', this function returns '0x4321'.
 */
template<typename T>
constexpr T reverse(const T& t);

/**
 * Conditionally converts to the specified endianness if and only if the host's byte
 * order differs from `Order`.
 */
template<order Order, typename T>
constexpr T conditional_convert(const T& t) noexcept;

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
template<typename T>
constexpr T host_to_network(const T& t);

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
template<typename T>
constexpr T network_to_host(const T& t);

} // endian

#include "detail/endian.ipp"

#endif // MND_ENDIAN_HEADER
