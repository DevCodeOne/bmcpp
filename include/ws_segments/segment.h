#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "font.h"
#include "pixel.h"

namespace BMCPP {

class Digit {
   public:
    inline constexpr explicit Digit(uint8_t number = 0,
                                    Pixel color = {std::byte(200),
                                                   std::byte(255)})
        : m_number(number), m_color(color) {}
    inline constexpr uint8_t number() const { return m_number; }
    inline constexpr const Pixel &color() const { return m_color; }

   private:
    uint8_t m_number;
    Pixel m_color;
};

enum struct DotsValue { Off, One, Two };

class Dots {
   public:
    inline constexpr explicit Dots(DotsValue value = DotsValue::Off,
                                         Pixel color = {std::byte(200),
                                                        std::byte(255)})
        : m_value(value), m_color(color) {}

    inline constexpr DotsValue value() const { return m_value; }
    inline constexpr const Pixel &color() const { return m_color; }

   private:
    DotsValue m_value;
    Pixel m_color;
};

bool operator==(const Digit &lhs, const uint8_t &rhs) {
    return lhs.number() == rhs;
}

bool operator==(const uint8_t &lhs, const Digit &rhs) {
    return rhs.number() == lhs;
}

bool operator!=(const Digit &lhs, const uint8_t &rhs) {
    return !(lhs.number() == rhs);
}

bool operator!=(const uint8_t &lhs, const Digit &rhs) {
    return !(rhs.number() == lhs);
}

namespace literals {
constexpr Digit operator"" _d(unsigned long long value) {
    return Digit{static_cast<uint8_t>(value)};
}
}  // namespace literals

template <uint8_t Segments, uint8_t Leds_Per_Segment>
class Segment;

template <uint8_t Leds_Per_Segment>
class Segment<7, Leds_Per_Segment> {
   public:
    static inline constexpr size_t size = 7 * Leds_Per_Segment;

    template <size_t Size>
    size_t draw_digit(std::array<Pixel, Size> &data, size_t offset) const;
    void set(const Digit &digit);

   private:
    Digit m_current_digit{0};
};

template <uint8_t Leds_Per_Segment>
class Segment<2, Leds_Per_Segment> {
   public:
    static inline constexpr size_t size = 2 * Leds_Per_Segment;

    template <size_t Size>
    size_t draw_digit(std::array<Pixel, Size> &data, size_t offset) const;
    void set(const Dots &value);

   private:
    Dots m_value;
};

template <uint8_t Leds_Per_Segment>
using Colon = Segment<2, Leds_Per_Segment>;

template <uint8_t Leds_Per_Segment>
using SevenSegment = Segment<7, Leds_Per_Segment>;

template <uint8_t Leds_Per_Segment>
template <size_t Size>
size_t Segment<7, Leds_Per_Segment>::draw_digit(std::array<Pixel, Size> &data,
                                                size_t offset) const {
    size_t begin = offset;
    size_t end = offset + size;

    uint8_t digit_bitmap = numbers[m_current_digit.number()];

    for (size_t i = begin; i < end; ++i) {
        using namespace std::literals;
        data[i].red = 0_byte;
        data[i].green = 0_byte;
        data[i].blue = 0_byte;
    }

    for (uint8_t segment = 0; segment < 7; ++segment) {
        if (digit_bitmap & 1) {
            size_t begin_segment = begin + segment * Leds_Per_Segment;
            size_t end_segment =
                begin + segment * Leds_Per_Segment + Leds_Per_Segment;

            for (size_t led = begin_segment; led < end_segment; ++led) {
                using namespace std::literals;
                data[led].red = m_current_digit.color().red;
                data[led].green = m_current_digit.color().green;
                data[led].blue = m_current_digit.color().blue;
            }
        }

        digit_bitmap >>= 1;
    }

    return offset + size;
}

template <uint8_t Leds_Per_Segment>
void Segment<7, Leds_Per_Segment>::set(const Digit &digit) {
    m_current_digit = digit;
}

template <uint8_t Leds_Per_Segment>
template <size_t Size>
size_t Segment<2, Leds_Per_Segment>::draw_digit(std::array<Pixel, Size> &data,
                                                size_t offset) const {
    size_t begin = offset;
    size_t end = offset + size;

    for (size_t i = begin; i < end; ++i) {
        using namespace std::literals;
        data[i].red = 0_byte;
        data[i].green = 0_byte;
        data[i].blue = 0_byte;
    }

    uint8_t digit_bitmap = dots[(unsigned int)m_value.value()];

    for (uint8_t dot = 0; dot < 2; ++dot) {
        if (digit_bitmap & 1) {
            size_t begin_segment = begin + dot * Leds_Per_Segment;
            size_t end_segment =
                begin + dot * Leds_Per_Segment + Leds_Per_Segment;

            for (size_t led = begin_segment; led < end_segment; ++led) {
                using namespace std::literals;
                data[led].red = m_value.color().red;
                data[led].green = m_value.color().green;
                data[led].blue = m_value.color().blue;
            }
        }

        digit_bitmap >>= 1;
    }

    return offset + size;
}

template <uint8_t Leds_Per_Segment>
void Segment<2, Leds_Per_Segment>::set(const Dots &value) {
    m_value = value;
}
}  // namespace BMCPP
