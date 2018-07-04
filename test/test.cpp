#include "../include/endian/endian.hpp"
#include "../include/endian/detail/arch.hpp"
#include <cassert>
#include <cstdio>
#include <typeinfo>

namespace test {

template<endian::order Order> void read1()
{
    char buffer[4];
    const int32_t num = 21344;
    endian::write<Order>(num, buffer);
    const int32_t res = endian::read<Order, int32_t>(buffer);
    assert(res == num);
}

template<endian::order Order> void read2()
{
    int32_t buffer;
    const int32_t num = 21344;
    endian::write<Order>(num, reinterpret_cast<char*>(&buffer));
    const int32_t res = endian::read<Order, int32_t>(
        reinterpret_cast<char*>(&buffer));
    assert(res == num);
}

void reverse()
{
    const uint32_t orig = 1234;
    const uint32_t conv = endian::reverse(endian::reverse(orig));
    assert(conv == orig);
}

void host_network_conv()
{
    const uint32_t orig = 1234;
    const uint32_t conv = endian::network_to_host(endian::host_to_network(orig));
    assert(conv == orig);
}

} // test

int main()
{
    test::read1<endian::order::big>();
    test::read1<endian::order::little>();

    test::read2<endian::order::big>();
    test::read2<endian::order::little>();

    test::reverse();
    test::host_network_conv();

    if(endian::order::host == endian::order::little)
        std::printf("host is little endian\n");
    else
        std::printf("host is big endian\n");
}
