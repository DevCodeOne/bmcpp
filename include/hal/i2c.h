#pragma once

#include <array>
#include <cstddef>

#include <util/twi.h>

namespace BMCPP {
namespace Hal {

struct Slave {};
struct Master {};

template <typename Mode = Master, size_t N = 0>
class I2C;

// TODO new type for address
template <size_t N>
class I2C<Master, N> {
   public:
    using i2c_type = BMCPP::AVR::ATMega328::I2C;
    static inline constexpr auto i2c = BMCPP::AVR::getAddress<i2c_type, N>;

    I2C();
    ~I2C() = default;

    bool write(const std::byte &address, const std::byte &data);
    template <size_t Size>
    bool write(const std::byte &address,
               const std::array<std::byte, Size> &data);

    std::byte read(const std::byte &address);
    template <size_t Size>
    void read(const std::byte &address, std::array<std::byte, Size> &data);

   private:
    struct Read {};
    struct Write {};

    template <typename Mode>
    bool start(std::byte address);
    void stop();

    template <typename... Flags>
    bool set_flags_and_wait(Flags... flags);

    static inline bool m_initialized;
    static inline constexpr size_t twi_clock_frequency = 100000UL;
    static inline constexpr size_t cpu_freq = F_CPU;
    static inline constexpr std::byte twi_bitrate =
        std::byte(((cpu_freq / twi_clock_frequency) - 16) / 2);
};

template <size_t N>
I2C<Master, N>::I2C() {
    if (!m_initialized) {
        *i2c()->bitrate_register = twi_bitrate;
        m_initialized = true;
    }
}

template <size_t N>
template <typename Mode>
bool I2C<Master, N>::start(std::byte address) {
    static_assert(std::is_same<Mode, Write>::value ||
                  std::is_same<Mode, Read>::value);

    address |= (std::byte)(((uint8_t)std::is_same<Mode, Read>::value) &
                           (~((uint8_t)std::is_same<Mode, Write>::value)));

    i2c()->control_register.clear(i2c_type::twi_interrupt,
                                  i2c_type::twi_enable_ack, i2c_type::twi_start,
                                  i2c_type::twi_stop, i2c_type::twi_write_col,
                                  i2c_type::twi_en, i2c_type::twi_in_en);
    set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_start,
                       i2c_type::twi_en);

    if (((uint8_t)i2c()->status_register.raw() & 0xF8) != TW_START) {
        return 1;
    }

    *i2c()->data_register = address;
    set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en);

    uint8_t twst = (uint8_t)i2c()->status_register.raw() & 0xF8;
    return (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK);
}

template <size_t N>
bool I2C<Master, N>::write(const std::byte &address, const std::byte &data) {
    start<Write>(address);
    *i2c()->data_register = data;
    bool ret = set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en);
    stop();
    return ret;
}

template <size_t N>
template <size_t Size>
bool I2C<Master, N>::write(const std::byte &address,
                           const std::array<std::byte, Size> &data) {
    start<Write>(address);
    bool ret;

    for (size_t i = 0; i < Size; ++i) {
        *i2c()->data_register = data[i];
        ret = set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en);
    }
    stop();

    return ret;
}

template <size_t N>
template <size_t Size>
void I2C<Master, N>::read(const std::byte &address,
                          std::array<std::byte, Size> &data) {
    start<Read>(address);
    for (size_t i = 0; i < Size - 1; ++i) {
        set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en,
                           i2c_type::twi_enable_ack);
        data[i] = *i2c()->data_register;
    }

    set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en);
    data[Size - 1] = *i2c()->data_register;

    stop();
}

template <size_t N>
std::byte I2C<Master, N>::read(const std::byte &address) {
    start<Read>(address);
    set_flags_and_wait(i2c_type::twi_interrupt, i2c_type::twi_en);
    std::byte ret = *i2c()->data_register;
    stop();

    return ret;
}

template <size_t N>
template <typename... Flags>
bool I2C<Master, N>::set_flags_and_wait(Flags... flags) {
    i2c()->control_register.set(flags...);
    // clang-format off
    while (!(i2c()->control_register.template isSet<i2c_type::twi_interrupt>()));
    // clang-format on
    return ((uint8_t)i2c()->status_register.raw() & 0xF8) != TW_MT_DATA_ACK;
}

template <size_t N>
void I2C<Master, N>::stop() {
    i2c()->control_register.set(i2c_type::twi_interrupt, i2c_type::twi_en,
                                i2c_type::twi_stop);
}

}  // namespace Hal
}  // namespace BMCPP
