#include <avr/io.h>
#include <util/delay.h>

#include "avr/mega328.h"
#include "avr/util.h"

#include "hal/port.h"

#include "ws2812b.h"

#include <chrono>
#include <array>

using namespace BMCPP::Hal;

using LedPin = Pin<Port<BMCPP::AVR::B>, 2>;

using Leds = BMCPP::WS2812B<LedPin, 21>;

int main() {
    using namespace std::literals;

    LedPin::template dir<Output>();

    Leds l;

    for (int i = 0; i < 21; i++) {
        l[i].red = 0_byte;
        l[i].green = 0_byte;
        l[i].blue = 0_byte;
    }

    int index = 0;

    while (true) {
        BMCPP::delay(200_ms);
        int offset = index * 3;
        for (int i = 0; i < 3; i++) {
            l[offset + i].red = 0_byte;
            l[offset + i].green = 0_byte;
            l[offset + i].blue = 0_byte;
        }
        ++index;

        if (index == 7) {
            index = 0;
        }

        offset = index * 3;
        for (int i = 0; i < 3; i++) {
            l[offset + i].red = 122_byte;
            l[offset + i].green = 0_byte;
            l[offset + i].blue = 200_byte;
        }
        l.show();
    }
}
