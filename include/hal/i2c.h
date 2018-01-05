#pragma once

#include <array>
#include <cstddef>

namespace BMCPP {
namespace Hal {

struct Address {};

template <typename N>
class I2C {
    I2C() = delete;

    void write(Address address, uint8_t byte);
    template <unsigned int S>
    void write(Address address, std::array<std::byte, S> buffer);
    void receive(Address address, uint8_t byte);
    template <unsigned int S>
    void receive(Address address, std::array<std::byte, S> buffer);
};

}  // namespace Hal
}  // namespace BMCPP
