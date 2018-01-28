/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016, 2017 Wilhelm Meier
 <wilhelm.meier@hs-kl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include <avr/register.h>

namespace BMCPP {
namespace AVR {

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

struct ATMega328 final {
    ATMega328() = delete;
    struct Port final {
        Port() = delete;
        DataRegister<Port, ReadOnly, std::byte> in;
        DataRegister<Port, ReadWrite, std::byte> ddr;
        DataRegister<Port, ReadWrite, std::byte> out;
        template <typename L>
        struct address;
    };

    struct I2C final {
        enum Status : uint8_t {
            tws4 = 1 << 7,
            tws3 = 1 << 6,
            tws2 = 1 << 5,
            tws1 = 1 << 4,
            tws0 = 1 << 3,
            twps1 = 1 << 1,
            twps0 = 1
        };

        enum Control : uint8_t {
            twi_interrupt = 1 << 7,
            twi_enable_ack = 1 << 6,
            twi_start = 1 << 5,
            twi_stop = 1 << 4,
            twi_write_col = 1 << 3,
            twi_en = 1 << 2,
            twi_in_en = 1
        };

        I2C() = delete;

        template <size_t N>
        struct address;

        DataRegister<I2C, ReadWrite, std::byte> bitrate_register;
        ControlRegister<I2C, Status, std::byte> status_register;
        // TODO different datatype
        DataRegister<I2C, ReadWrite, std::byte> address_register;
        DataRegister<I2C, ReadWrite, std::byte> data_register;
        ControlRegister<I2C, Control, std::byte> control_register;
    };

    struct StatusRegister final {
        enum BitType : uint8_t { globalInterrupt = 1 << 7 };

        StatusRegister() = delete;
        ControlRegister<StatusRegister, BitType, std::byte> sreg;
        static inline uintptr_t address = 0x5f;
    };
} __attribute__((packed));

template <>
struct ATMega328::Port::address<B> {
    inline static constexpr uintptr_t value = 0x23;
};
template <>
struct ATMega328::Port::address<C> {
    inline static constexpr uintptr_t value = 0x26;
};
template <>
struct ATMega328::Port::address<D> {
    inline static constexpr uintptr_t value = 0x29;
};

template <>
struct ATMega328::I2C::address<0> {
    inline static constexpr uintptr_t value = 0xB8;
};

template <typename Component, uint8_t N>
constexpr Component* getAddress() {
    return reinterpret_cast<Component*>(Component::template address<N>::value);
}
template <typename Component, typename Letter>
constexpr Component* getAddress() {
    return reinterpret_cast<Component*>(
        Component::template address<Letter>::value);
}
template <typename Component>
constexpr Component* getAddress() {
    return reinterpret_cast<Component*>(Component::address);
}
}  // namespace AVR
}  // namespace BMCPP
