#include "../include/endian/endian.hpp"
#include "../include/endian/detail/arch.hpp"
#include <cassert>
#include <typeinfo>

template<endian::order Order> void test()
{
    char buffer[4];
    const int32_t num = 21344;
    endian::write<Order>(num, buffer);
    const int32_t res = endian::parse<Order, int32_t>(buffer);
    assert(res == num);
}

template<endian::order Order> void test2()
{
    int32_t buffer;
    const int32_t num = 21344;
    endian::write<Order>(num, reinterpret_cast<char*>(&buffer));
    const int32_t res = endian::parse<Order, int32_t>(
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
    test<endian::order::big>();
    test<endian::order::little>();
    test2<endian::order::big>();
    test2<endian::order::little>();
    test3();

    endian::host_to_network(5);
    endian::network_to_host(5);

    if(endian::order::little == endian::order::host) {}
}
