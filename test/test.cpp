#include "../include/endian/endian.hpp"
#include "../include/endian/detail/arch.hpp"
#include <cassert>
#include <typeinfo>

template<typename Endianness> void test()
{
    char buffer[4];
    const int32_t num = 21344;
    endian::write<Endianness>(num, buffer);
    const int32_t res = endian::parse<Endianness, int32_t>(buffer);
    assert(res == num);
}

template<typename Endianness> void test2()
{
    int32_t buffer;
    const int32_t num = 21344;
    endian::write<Endianness>(num, reinterpret_cast<char*>(&buffer));
    const int32_t res = endian::parse<Endianness, int32_t>(
        reinterpret_cast<char*>(&buffer));
    assert(res == num);
}

void test3()
{
    const uint32_t orig = 1234;
    uint32_t conv = endian::reverse(orig);
    conv = endian::reverse(conv);
    assert(conv == orig);
}

int main()
{
    test<endian::big>();
    test<endian::little>();
    test2<endian::big>();
    test2<endian::little>();
    test3();

    endian::host_to_network(5);
    endian::network_to_host(5);
    if(endian::is_host<endian::little>()) {}
}
