#ifndef MND_TYPE_TRAITS_HEADER
#define MND_TYPE_TRAITS_HEADER

#include <type_traits>
#include <cstdint>

namespace endian {
namespace detail {

template<typename T>
struct is_endian_reversible
{
    // TODO: is is_pod right?
    static constexpr bool value = std::is_integral<T>::value || std::is_pod<T>::value;
};

template<typename... Ts>
struct make_void { using type = void; };

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

template<typename T, typename>
struct is_input_iterator : std::false_type {};

template<typename T>
struct is_input_iterator<T, void_t<
        decltype(*std::declval<T&>()),
        decltype(++std::declval<T&>()),
        decltype(std::declval<T&>()++)>>
    : std::true_type {};

template <size_t N>
struct integral_type_for
{
    static_assert(N <= 8, "N may be at most 8 bytes large");
    using type = typename std::conditional<
        (N > sizeof(uint8_t)),
        typename std::conditional<
            (N > sizeof(uint16_t)),
            typename std::conditional<
                (N > sizeof(uint32_t)),
                uint64_t,
                uint32_t
            >::type,
            uint16_t
        >::type,
        uint8_t
    >::type;
};

} // detail
} // endian


#endif // MND_TYPE_TRAITS_HEADER
