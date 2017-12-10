#include "endian.hpp"
#include <cassert>

template<typename Endianness> void test()
{
    char buffer[4];
    const int32_t num = 21344;
    endian::write<Endianness>(num, buffer);
    const int32_t res = endian::parse<Endianness, int32_t>(buffer);
    assert(res == num);
}

int main()
{
    test<endian::big>();
    test<endian::little>();
}
