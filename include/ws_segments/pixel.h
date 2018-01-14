#pragma once

#include <cstddef>

namespace BMCPP {
struct Pixel final {
    std::byte green{0};
    std::byte red{0};
    std::byte blue{0};
} __attribute__((packed));
}  // namespace BMCPP
