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

template<endian::order Order> void read3()
{
    // Buffer size doesn't matter as long as it's at least N large. (N = 3)
    char buffer[8];
    const int32_t num = 0x00ffaabb;
    endian::write<Order, 3>(num, buffer);
    // NOTE: don't start reading from the beginning of the buffer as buffer
    // contains a 4 byte value and we want 3 bytes.
    const int32_t res = endian::read<Order, 3>(buffer);
    std::printf("expected: 0x%x actual: 0x%x\n", num, res);
    assert(res == num);
}

template<endian::order Order> void read4()
{
    char buffer[4];
    const int32_t num = 0x0000a01f;
    endian::write<Order, 3>(num, buffer);
    const int32_t res = endian::read<Order, 3>(buffer);
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

void typedefs()
{
    char buffer[4];
    const int32_t num = 0x0000a01f;
    endian::write_le(num, buffer);
    auto res1 = endian::read_le<4>(buffer);
    assert(res1 == num);
    res1 = endian::read_le<int32_t>(buffer);
    assert(res1 == num);

    endian::write_be(num, buffer);
    auto res2 = endian::read_be<4>(buffer);
    assert(res2 == num);
    res2 = endian::read_be<int32_t>(buffer);
    assert(res2 == num);
}

} // test

int main()
{
    test::read1<endian::order::big>();
    test::read1<endian::order::little>();

    test::read2<endian::order::big>();
    test::read2<endian::order::little>();

    test::read3<endian::order::big>();
    test::read3<endian::order::little>();

    test::read4<endian::order::big>();
    test::read4<endian::order::little>();

    test::typedefs();

    test::reverse();
    test::host_network_conv();

    if(endian::order::host == endian::order::little)
        std::printf("host is little endian\n");
    else
        std::printf("host is big endian\n");
}
