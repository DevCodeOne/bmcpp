#include "avr/mega328.h"
#include "avr/util.h"
#include "hal/rtc.h"
#include "ws_segments/display.h"

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;

int main() {
    using namespace BMCPP;
    using namespace std::literals;

    Display<LedPin, SevenSegment<3>, SevenSegment<3>, Colon<1>, SevenSegment<3>,
            SevenSegment<3>>
        display;

    RTC<DS3231, I2C<Master>> rtc;
    uint8_t hours = 0, minutes = 0, seconds = 0;

    while (true) {
        hours = rtc.hours().value;
        minutes = rtc.minutes().value;
        seconds = rtc.seconds().value;

        display.show(Digit(hours / 10), Digit(hours % 10),
                     (seconds & 1) ? ColonValue::Two : ColonValue::Off,
                     Digit((minutes / 10)), Digit(minutes % 10));

        delay(250_ms);
    }
}
