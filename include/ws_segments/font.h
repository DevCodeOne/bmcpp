#pragma once

#include <cstddef>

#include <array>
#include <tuple>

/*
 *  1
 * 0 2
 *  6
 * 5 3
 *  4
 */

namespace BMCPP {

template <uint8_t Lit_Segment, uint8_t... Lit_Segments>
constexpr uint8_t make_digit_font() {
    static_assert(Lit_Segment <= 7);
    if constexpr (sizeof...(Lit_Segments) > 0) {
        return 1 << Lit_Segment | make_digit_font<Lit_Segments...>();
    } else {
        return 1 << Lit_Segment;
    }
}

inline constexpr std::array<uint8_t, 10> numbers{
    make_digit_font<0, 1, 2, 3, 4, 5>(),     // 0
    make_digit_font<2, 3>(),                 // 1
    make_digit_font<1, 2, 6, 5, 4>(),        // 2
    make_digit_font<1, 2, 6, 3, 4>(),        // 3
    make_digit_font<0, 6, 2, 3>(),           // 4
    make_digit_font<1, 0, 6, 3, 4>(),        // 5
    make_digit_font<1, 0, 6, 5, 3, 4>(),     // 6
    make_digit_font<1, 2, 3>(),              // 7
    make_digit_font<0, 1, 2, 3, 4, 5, 6>(),  // 8
    make_digit_font<0, 1, 2, 3, 4, 6>(),     // 9
};

inline constexpr std::array<uint8_t, 3> dots{uint8_t{0}, make_digit_font<0>(),
                                             make_digit_font<0, 1>()};

}  // namespace BMCPP
