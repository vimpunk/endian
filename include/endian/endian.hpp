#ifndef MND_ENDIAN_HEADER
#define MND_ENDIAN_HEADER

#include "detail/arch.hpp"

#include <cstdint>
#include <type_traits>

namespace endian {

// Tags for marking the desired endianness.
struct big {};
struct little {};
using network = big;
#if defined(MND_BIG_ENDIAN)
using host = big;
#elif defined(MND_LITTLE_ENDIAN)
using host = little;
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

template<typename T> struct is_endianness;
template<typename T> struct is_endian_reversible;
template<typename T, typename = void> struct is_iterator;
template<typename T> T byte_swap(const T& t) noexcept;
template<typename Endianness, typename T> T conditional_reverse(const T& t) noexcept;

} // detail

/**
 * Parses `sizeof(T)` bytes from the memory pointed to by `it`, and reconstructs from it
 * an integer of type `T`, converting from the specified `Endianness` to host byte order.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `sizeof(T)` bytes.
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
    static_assert(detail::is_endianness<Endianness>::value,
        "Endianness type requirements not met, which must be either endian::big, "
        "endian::little, endian::network, or endian::host");
    static_assert(detail::is_endian_reversible<T>::value,
        "T must be an integral or POD type");
    static_assert(detail::is_iterator<InputIt>::value,
        "Iterator type requirements not met");
    return detail::parse<T>(it, Endianness());
}

/**
 * Writes each byte of `h` to the memory pointed to by `it`, such that it converts the
 * byte order of `h` from host byte order to the specified `Endianness`.
 *
 * It's undefined behaviour if `it` points to a buffer smaller than `sizeof(T)` bytes.
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
template<typename Endianness, typename T, typename OutputIt>
constexpr T write(const T& h, OutputIt it) noexcept
{
    static_assert(detail::is_endianness<Endianness>::value,
        "Endianness type requirements not met, which must be either endian::big, "
        "endian::little, endian::network, or endian::host");
    static_assert(detail::is_endian_reversible<T>::value,
        "T must be an integral or POD type");
    static_assert(detail::is_iterator<OutputIt>::value,
        "Iterator type requirements not met");
    detail::write<T>(h, it, Endianness());
}

/**
 * Reverses endianness, i.e. the byte order in `t`. E.g. given the 32-bit number
 * '0x1234', this function returns '0x4321'.
 */
template<typename T>
constexpr T reverse(const T& t)
{
    return detail::byte_swap<T>(t);
}

/**
 * Conditionally converts to the specified endianness if and only if the host's byte
 * order differs from `Endianness`.
 */
template<typename Endianness, typename T>
constexpr T convert_to(const T& t) noexcept
{
    return detail::conditional_reverse<Endianness, T>(t);
}

template<typename T>
constexpr T host_to_network(const T& t)
{
    return convert_to<network>(t);
}

template<typename T>
constexpr T network_to_host(const T& t)
{
    return convert_to<host>(t);
}

template<typename Endianness>
constexpr bool is_host() noexcept
{
    return std::is_same<Endianness, host>::value;
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

template<typename T>
struct is_endianness
{
    static constexpr bool value =
        std::is_same<typename std::decay<T>::type, endian::big>::value ||
        std::is_same<typename std::decay<T>::type, endian::little>::value;
};

template<typename T>
struct is_endian_reversible
{
    static constexpr bool value = std::is_integral<T>::value || std::is_pod<T>::value;
};

template<typename... Ts>
struct make_void { using type = void; };

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

template<typename T, typename>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<T, void_t<
        decltype(*std::declval<T&>()),
        decltype(++std::declval<T&>()),
        decltype(std::declval<T&>()++)>>
    : std::true_type {};

template<size_t Size>
struct byte_swapper {};

template<>
struct byte_swapper<2>
{
    template<typename T>
    T swap(const T& t) { return MND_BYTE_SWAP_16(t); }
};

template<>
struct byte_swapper<4>
{
    template<typename T>
    T swap(const T& t) { return MND_BYTE_SWAP_32(t); }
};

template<>
struct byte_swapper<8>
{
    template<typename T>
    T swap(const T& t) { return MND_BYTE_SWAP_64(t); }
};

template<typename T>
T byte_swap(const T& t) noexcept
{
    return byte_swapper<sizeof(T)>().swap(t);
};

template<typename Endianness>
struct conditional_reverser
{
    template<typename T>
    constexpr T operator()(const T& t) { return reverse(t); }
};

template<>
struct conditional_reverser<host>
{
    template<typename T>
    constexpr T operator()(const T& t) { return t; }
};

template<typename Endianness, typename T>
T conditional_reverse(const T& t) noexcept
{
    return conditional_reverser<Endianness>()(t);
}

} // detail
} // endian

#endif // MND_ENDIAN_HEADER
