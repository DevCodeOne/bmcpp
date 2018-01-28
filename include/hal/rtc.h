#pragma once

#include <array>
#include <chrono>
#include <cstddef>

#include "i2c.h"

namespace BMCPP {

class DS3231 {};

template <typename RTCType, typename Interface>
class RTC {};

template <typename Interface>
class RTC<DS3231, Interface> {
   public:
    using second_type = std::duration<uint8_t, std::ratio<1>>;
    using minute_type = std::duration<uint8_t, std::ratio<60>>;
    using hour_type = std::duration<uint8_t, std::ratio<3600>>;

    RTC() = default;
    ~RTC() = default;

    second_type seconds() const;
    minute_type minutes() const;
    hour_type hours() const;

    void seconds(second_type new_seconds) const;
    void minutes(minute_type new_minutes) const;
    void hours(hour_type new_hours) const;

   private:
    static uint8_t dectobcd(uint8_t data);
    static uint8_t bcdtodec(uint8_t data);

    static constexpr inline std::byte address = std::byte(0xD0);
    static constexpr inline std::byte seconds_register = std::byte(0x0);
    static constexpr inline std::byte minutes_register = std::byte(0x1);
    static constexpr inline std::byte hours_register = std::byte(0x2);

    mutable Interface m_interface;
};

template <typename Interface>
auto RTC<DS3231, Interface>::seconds() const -> second_type {
    m_interface.write(address, seconds_register);
    uint8_t value = (uint8_t)m_interface.read(address);
    return second_type{bcdtodec(value)};
}

template <typename Interface>
auto RTC<DS3231, Interface>::minutes() const -> minute_type {
    m_interface.write(address, minutes_register);
    uint8_t value = (uint8_t)m_interface.read(address);
    return minute_type{bcdtodec(value)};
}

template <typename Interface>
auto RTC<DS3231, Interface>::hours() const -> hour_type {
    m_interface.write(address, hours_register);
    uint8_t value = (uint8_t)m_interface.read(address);
    return hour_type{bcdtodec(value)};
}

template <typename Interface>
void RTC<DS3231, Interface>::seconds(second_type new_seconds) const {
    std::array<std::byte, 2> data{seconds_register,
                                  (std::byte)dectobcd(new_seconds.value)};
    m_interface.write(address, data);
}

template <typename Interface>
void RTC<DS3231, Interface>::minutes(minute_type new_minutes) const {
    std::array<std::byte, 2> data{minutes_register,
                                  (std::byte)dectobcd(new_minutes.value)};
    m_interface.write(address, data);
}

template <typename Interface>
void RTC<DS3231, Interface>::hours(hour_type new_hours) const {
    std::array<std::byte, 2> data{hours_register,
                                  (std::byte)dectobcd(new_hours.value)};
    m_interface.write(address, data);
}

template <typename Interface>
uint8_t RTC<DS3231, Interface>::bcdtodec(uint8_t data) {
    return (data % 16) + ((data / 16) * 10);
}

template <typename Interface>
uint8_t RTC<DS3231, Interface>::dectobcd(uint8_t data) {
    return (data % 10) + ((data / 10) * 16);
}

}  // namespace BMCPP
