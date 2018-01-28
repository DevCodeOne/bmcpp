#pragma once

#include <cstddef>
#include <cstdint>

#include "avr/mega328.h"

namespace BMCPP {

template <uint8_t N = 0>
class Analog2Digital {
   public:
    using adc_type = BMCPP::AVR::ATMega328::ADC;
    static inline constexpr auto adc = BMCPP::AVR::getAddress<adc_type, N>;

    Analog2Digital();
    ~Analog2Digital() = default;

    template <typename Pin>
    uint16_t read_value();

   private:
    static inline bool m_initialized = false;
};

template <uint8_t N>
Analog2Digital<N>::Analog2Digital() {
    if (!m_initialized) {
        adc()->multiplexer_sel_register.clear(
            adc_type::adc_ref_sel_one, adc_type::adc_ref_sel_zero,
            adc_type::adc_left_adj, adc_type::adc_analog_sel_three,
            adc_type::adc_analog_sel_two, adc_type::adc_analog_sel_one,
            adc_type::adc_analog_sel_zero);
        adc()->multiplexer_sel_register.set(adc_type::adc_ref_sel_zero);
        adc()->control_status_registera.set(
            adc_type::adc_enable, adc_type::adc_pres_two,
            adc_type::adc_pres_one, adc_type::adc_pres_zero);
        m_initialized = true;
    }
}

template <uint8_t N>
template <typename Pin>
uint16_t Analog2Digital<N>::read_value() {
    using channel = BMCPP::AVR::ATMega328::ADC::template Channel<Pin>;

    adc()->multiplexer_sel_register.clear(
        adc_type::adc_ref_sel_one, adc_type::adc_ref_sel_zero,
        adc_type::adc_left_adj, adc_type::adc_analog_sel_three,
        adc_type::adc_analog_sel_two, adc_type::adc_analog_sel_one,
        adc_type::adc_analog_sel_zero);
    adc()->multiplexer_sel_register.set(adc_type::adc_ref_sel_zero);
    adc()
        ->multiplexer_sel_register
        .template add<adc_type::Mux(channel::channel & 1)>();
    adc()
        ->multiplexer_sel_register
        .template add<adc_type::Mux(channel::channel & 2)>();
    adc()
        ->multiplexer_sel_register
        .template add<adc_type::Mux(channel::channel & 4)>();

    adc()->control_status_registera.template add<adc_type::adc_start_conv>();

    // clang-format off
    while (adc()->control_status_registera.template isSet<adc_type::adc_start_conv>());
    // clang-format on

    uint8_t low = (uint8_t)(*adc()->value_low_register);
    uint8_t high = (uint8_t)(*adc()->value_high_register);

    return high << 8 | low;
}
}  // namespace BMCPP
