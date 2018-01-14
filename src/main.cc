#include <avr/io.h>
#include <util/delay.h>

#include "avr/mega328.h"
#include "avr/util.h"

#include "hal/port.h"

#include "ws_segments/display.h"
#include "ws_segments/ws2812b.h"

#include <array>
#include <chrono>

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;

int main() {
    using namespace BMCPP;

    Display<LedPin, SevenSegment<3>, SevenSegment<3>, Dots<1>, SevenSegment<3>,
            SevenSegment<3>>
        display;

    uint8_t numbers[2]{0, 0};
    while (true) {
        using namespace BMCPP::literals;
        ++numbers[0];

        if (numbers[0] == 10) {
            numbers[0] = 0;
            numbers[1]++;
            numbers[1] %= 10;
        }

        display.show(Digit(numbers[1]), Digit(numbers[0]),
                     (numbers[0] & 1) ? DotsValue::Two : DotsValue::Off,
                     Digit(numbers[1]), Digit(numbers[0]));

        using namespace std::literals;
        BMCPP::delay(500_ms);
    }
}
