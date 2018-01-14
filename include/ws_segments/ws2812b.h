#pragma once

#include <avr/interrupt.h>
#include <avr/scoped_interrupt.h>
#include <avr/util.h>
#include <hal/port.h>

#include <array>
#include <chrono>
#include <cstdint>

#include "pixel.h"

template <typename T>
constexpr T clamp_bottom(T value, T min) {
    return value < min ? min : value;
}

// TODO this has to be a strip there has to be some kind of interface for other
// classes
namespace BMCPP {
template <typename Pin, size_t NumLeds>
class WS2812B final {
   public:
    WS2812B() { Pin::template dir<Hal::Output>(); }

    Pixel &operator[](size_t index) { return m_data[index]; }

    const Pixel &operator[](size_t index) const { return m_data[index]; }

    void clear();
    // TODO implement
    void brightness(std::byte brightness);
    std::byte brightness() const;
    void show() const { WS2812B<Pin, NumLeds>::template show_data(m_data); }

    template <size_t Size>
    static void show_data(const std::array<Pixel, Size> &data);

    size_t size() const { return NumLeds; }

    // TODO Add frequency types and period types
    static constexpr unsigned int zeropulse = 350;
    static constexpr unsigned int onepulse = 900;
    static constexpr unsigned int totalperiod = zeropulse + onepulse;

    static constexpr unsigned int fixedlow = 2;
    static constexpr unsigned int fixedhigh = 4;
    static constexpr unsigned int fixedtotal = 8;

    static constexpr long int freq_in_khz = F_CPU / 1000;
    static constexpr long int zerocycles =
        ((freq_in_khz * zeropulse) / 1'000'000);
    static constexpr long int onecycles =
        ((freq_in_khz * onepulse + 500'000) / 1'000'000);
    static constexpr long int totalcycles =
        ((freq_in_khz * totalperiod + 500'000) / 1'000'000);

    static constexpr long int wait_rising = zerocycles - fixedlow;
    static constexpr long int wait_falling =
        onecycles - fixedhigh - wait_rising;
    static constexpr long int wait_remaining =
        totalcycles - fixedtotal - wait_rising - wait_falling;
    static constexpr long int lowtime =
        (wait_rising + fixedlow) * 1'000'000 / freq_in_khz;

    static_assert(lowtime <= 550, "Clockspeed is too low");

   private:
    std::array<Pixel, NumLeds> m_data;
};

template <uint8_t wait>
inline void __attribute__((always_inline)) wait_nops() {}

template <>
inline void __attribute__((always_inline)) wait_nops<1>() {
    asm volatile("nop \n\t");
}

template <>
inline void __attribute__((always_inline)) wait_nops<2>() {
    asm volatile("rjmp .+0 \n\t");
}

template <>
inline void __attribute__((always_inline)) wait_nops<4>() {
    wait_nops<2>();
    wait_nops<2>();
}

template <>
inline void __attribute__((always_inline)) wait_nops<8>() {
    wait_nops<4>();
    wait_nops<4>();
}

template <>
inline void __attribute__((always_inline)) wait_nops<16>() {
    wait_nops<8>();
    wait_nops<8>();
}

template <uint8_t wait_period>
inline void __attribute__((always_inline)) wait() {
    wait_nops<wait_period & 1>();
    wait_nops<wait_period & 2>();
    wait_nops<wait_period & 4>();
    wait_nops<wait_period & 8>();
    wait_nops<wait_period & 16>();
}

template <typename Pin, size_t NumLeds>
template <size_t Size>
void WS2812B<Pin, NumLeds>::show_data(const std::array<Pixel, Size> &data) {
    using namespace std::literals;

    Pin::template dir<Hal::Output>();

    std::byte curbyte, ctr, maskhi, masklo;

    size_t data_len = data.size() * 3;
    const std::byte *pixel_data =
        reinterpret_cast<const std::byte *>(data.data());

    using Port = typename Pin::port_type;

    volatile std::byte *port_address =
        ((volatile std::byte *)&Port::get()) - 0x20;

    maskhi = Port::ddr();
    masklo = Port::get() & ~maskhi;
    maskhi |= Port::get();

    AVR::ScopedInterrupt<AVR::RestoreOldStatus, AVR::InterruptDisable>
        interrupt;

    while (data_len--) {
        curbyte = *pixel_data++;

        asm volatile(
            "       ldi   %0,8  \n\t"
            "loop:            \n\t"
            "       out   %2,%3 \n\t"  //  '1' [01] '0' [01] - re
            : "=&d"(ctr)
            : "r"(curbyte), "I"(port_address), "r"(maskhi), "r"(masklo));
        wait<wait_rising>();
        asm volatile(
            "       sbrs  %1,7  \n\t"  //  '1' [03] '0' [02]
            "       out   %2,%4 \n\t"  //  '1' [--] '0' [03] - fe-low
            "       lsl   %1    \n\t"  //  '1' [04] '0' [04]
            : "=&d"(ctr)
            : "r"(curbyte), "I"(port_address), "r"(maskhi), "r"(masklo));
        wait<wait_falling>();
        asm volatile("       out   %2,%4 \n\t"  //  '1' [+1] '0' [+1] - fe-high
                     : "=&d"(ctr)
                     : "r"(curbyte), "I"(port_address), "r"(maskhi),
                       "r"(masklo));
        wait<wait_remaining>();
        asm volatile(
            "       dec   %0    \n\t"  //  '1' [+2] '0' [+2]
            "       brne  loop\n\t"    //  '1' [+3] '0' [+4]
            : "=&d"(ctr)
            : "r"(curbyte), "I"(port_address), "r"(maskhi), "r"(masklo));
    }

    using namespace std::literals;

    BMCPP::delay(300_us);
}
}  // namespace BMCPP
