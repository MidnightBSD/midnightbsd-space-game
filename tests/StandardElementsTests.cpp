#include "StandardElements.h"

#include <array>
#include <cassert>

int main() {
    std::array<unsigned char, 5> bytes{1, 2, 3, 4, 5};
    flipBytes(bytes.data(), bytes.size());
    assert((bytes == std::array<unsigned char, 5>{5, 4, 3, 2, 1}));

    flipBytes(bytes.data(), bytes.size());
    assert((bytes == std::array<unsigned char, 5>{1, 2, 3, 4, 5}));
    return 0;
}
