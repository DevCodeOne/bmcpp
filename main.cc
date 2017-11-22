#include <avr/io.h>
#include <util/delay.h>

#include "avr/mega328.h"
#include "avr/util.h"

#include "hal/port.h"

#include "ws2812b.h"

#include <chrono>

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;

using Leds = BMCPP::WS2812B<LedPin, 1>;

int main() {
    using namespace std::literals;

    LedPin::template dir<Output>();

    Leds l;

    while (true) {
        l[0].red = std::byte(255);
        l.show();
        BMCPP::delay(500_ms);
        l[0].red = std::byte(0);
        l.show();
        BMCPP::delay(250_ms);
    }
}
