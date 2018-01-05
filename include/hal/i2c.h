#pragma once

#include <cstddef>

namespace BMCPP {
namespace Hal {

struct Address {};

template <typename N>
class I2C {
    I2C() = delete;

    void write(Address address, uint8_t byte);
    template<unsigned int N>
    void write(Address address, std::array<byte, N> buffer);
    void receive(Address address, uint8_t byte);
    template<unsigned int N>
    void receive(Address address, std::array<byte, N> buffer);
};

}  // namespace Hal
