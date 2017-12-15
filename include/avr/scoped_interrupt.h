#pragma once

namespace BMCPP::AVR {

#include <avr/interrupt.h>

struct InterruptEnable {};
struct InterruptDisable {};

struct RestoreOldStatus {};
struct OppositeToFirstAction {};

template <typename Mode, typename FirstAction>
class ScopedInterrupt {};

template <typename FirstAction>
class ScopedInterrupt<OppositeToFirstAction, FirstAction> {
   public:
    ScopedInterrupt() {
        if constexpr (std::is_same<FirstAction, InterruptEnable>::value) {
            sei();
        } else {
            cli();
        }
    }

    ~ScopedInterrupt() {
        if constexpr (std::is_same<FirstAction, InterruptEnable>::value) {
            cli();
        } else {
            sei();
        }
    }
};

template <typename FirstAction>
class ScopedInterrupt<RestoreOldStatus, FirstAction> {
   public:
    ScopedInterrupt() {
        using status_register = ATMega328::StatusRegister;
        m_value = std::byte(
            (uint8_t)getAddress<status_register>()
                ->sreg.get<status_register::BitType::globalInterrupt>());
        if constexpr (std::is_same<FirstAction, InterruptEnable>::value) {
            sei();
        } else {
            cli();
        }
    }

    ~ScopedInterrupt() {
        if ((uint8_t)m_value) {
            sei();
        } else {
            cli();
        }
    }

   private:
    std::byte m_value;
};
}
