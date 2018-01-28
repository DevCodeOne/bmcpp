#include "avr/mega328.h"
#include "avr/util.h"
#include "hal/adc.h"
#include "hal/rtc.h"
#include "ws_segments/display.h"

#include <cstdint>

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;
using AnalogPin = Pin<Port<BMCPP::AVR::C>, 1>;

BMCPP::Pixel calc_brightness(uint8_t sector) {
    return BMCPP::Pixel{std::byte(sector * 20), std::byte(sector * 5),
                        std::byte(sector * 20)};
}

int main() {
    using namespace BMCPP;
    using namespace std::literals;

    uint8_t sector = 0;
    uint8_t hours = 0, minutes = 0, seconds = 0;
    Pixel clock_color;
    Display<LedPin, SevenSegment<3>, SevenSegment<3>, Colon<1>, SevenSegment<3>,
            SevenSegment<3>>
        display;

    RTC<DS3231, I2C<Master>> rtc;
    Analog2Digital adc;

    while (true) {
        hours = rtc.hours().value;
        minutes = rtc.minutes().value;
        seconds = rtc.seconds().value;

        sector = adc.read_value<AnalogPin>() / 110;
        clock_color = calc_brightness(sector);

        display.show(
            Digit(hours / 10, clock_color), Digit(hours % 10, clock_color),
            Dots((seconds & 1) ? DotsValue::Two : DotsValue::Off, clock_color),
            Digit(minutes / 10, clock_color), Digit(minutes % 10, clock_color));

        delay(250_ms);
    }
}
