#include <algorithm>

#include <cstdint>

#include "avr/mega328.h"
#include "avr/util.h"
#include "hal/adc.h"
#include "hal/rtc.h"
#include "ws_segments/display.h"

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;
using AnalogPin = Pin<Port<BMCPP::AVR::C>, 1>;

BMCPP::Pixel calc_brightness(uint8_t sector) {
    sector = std::clamp(sector, (uint8_t)0, (uint8_t)20);
    return BMCPP::Pixel{std::byte(sector * 10), std::byte(sector * 3),
                        std::byte(sector * 10)};
}

constexpr uint8_t number_of_samples = 10;

int main() {
    using namespace BMCPP;
    using namespace std::literals;

    uint8_t sample = 0;
    uint8_t samples = number_of_samples;
    uint16_t summedSamples = 0;
    uint8_t hours = 0, minutes = 0, seconds = 0;
    Pixel clock_color = calc_brightness(15);
    Display<LedPin, SevenSegment<3>, SevenSegment<3>, Colon<1>, SevenSegment<3>,
            SevenSegment<3>>
        display;

    RTC<DS3231, I2C<Master>> rtc;
    Analog2Digital adc;

    while (true) {
        hours = rtc.hours().value;
        minutes = rtc.minutes().value;
        seconds = rtc.seconds().value;

        sample = adc.read_value<AnalogPin>() / 50;
        summedSamples += sample;

        if (samples == 0) {
            samples = number_of_samples;
            clock_color = calc_brightness(summedSamples / number_of_samples);
            summedSamples = 0;
        }

        display.show(
            Digit(hours / 10, clock_color), Digit(hours % 10, clock_color),
            Dots((seconds & 1) ? DotsValue::Two : DotsValue::Off, clock_color),
            Digit(minutes / 10, clock_color), Digit(minutes % 10, clock_color));

        --samples;
        delay(250_ms);
    }
}
