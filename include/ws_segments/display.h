#pragma once

#include <tuple>

#include "segment.h"
#include "ws2812b.h"

template <typename Segment, typename... Segments>
constexpr size_t calculate_strip_length() {
    if constexpr (sizeof...(Segments) > 0) {
        return Segment::size + calculate_strip_length<Segments...>();
    } else {
        return Segment::size;
    }
}

namespace BMCPP {
template <typename Pin, typename... Segments>
class Display {
   public:
    static constexpr inline size_t number_of_leds =
        calculate_strip_length<Segments...>();

    using led_type = BMCPP::WS2812B<Pin, number_of_leds>;
    template <typename... SegmentValues>
    void show(const SegmentValues... values);
    void clear();

   private:
    template <size_t index, typename CurrentValue, typename... SegmentValues>
    void set_values(const CurrentValue &value, const SegmentValues... values);
    template <size_t index, typename CurrentValue>
    void set_values(const CurrentValue &value);

    template <size_t index>
    void show(size_t offset);

    std::tuple<Segments...> m_segments;
    std::array<Pixel, number_of_leds> m_pixels;
};

template <typename Pin, typename... Segments>
void Display<Pin, Segments...>::clear() {
    for (size_t i = 0; i < m_pixels.size(); ++i) {
        using namespace std::literals;
        m_pixels[i].red = 0_byte;
        m_pixels[i].green = 0_byte;
        m_pixels[i].blue = 0_byte;
    }

    led_type::show_data(m_pixels);
}

template <typename Pin, typename... Segments>
template <typename... SegmentValues>
void Display<Pin, Segments...>::show(const SegmentValues... values) {
    set_values<0, SegmentValues...>(values...);
    show<0>(0);
}

template <typename Pin, typename... Segments>
template <size_t index>
void Display<Pin, Segments...>::show(size_t offset) {
    auto segment = m_segments.template get<index>();

    if constexpr (index + 1 < sizeof...(Segments)) {
        show<index + 1>(segment.draw_digit(m_pixels, offset));
    } else {
        segment.draw_digit(m_pixels, offset);
        led_type::show_data(m_pixels);
    }
}

template <typename Pin, typename... Segments>
template <size_t index, typename CurrentValue, typename... SegmentValues>
void Display<Pin, Segments...>::set_values(const CurrentValue &value,
                                           const SegmentValues... values) {
    m_segments.template get<index>().set(value);
    set_values<index + 1>(values...);
}

template <typename Pin, typename... Segments>
template <size_t index, typename CurrentValue>
void Display<Pin, Segments...>::set_values(const CurrentValue &value) {
    m_segments.template get<index>().set(value);
}
}  // namespace BMCPP
