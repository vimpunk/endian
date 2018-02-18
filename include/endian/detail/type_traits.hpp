#ifndef MND_TYPE_TRAITS_HEADER
#define MND_TYPE_TRAITS_HEADER

#include <type_traits>

namespace endian {
namespace detail {

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
struct is_input_iterator : std::false_type {};

template<typename T>
struct is_input_iterator<T, void_t<
        decltype(*std::declval<T&>()),
        decltype(++std::declval<T&>()),
        decltype(std::declval<T&>()++)>>
    : std::true_type {};

} // detail
} // endian


#endif // MND_TYPE_TRAITS_HEADER
