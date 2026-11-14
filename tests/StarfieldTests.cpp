#include "Starfield.h"

#include <cassert>

int main() {
    Star steady{{}, 0.6F, 1.0F, 0.0F, 8000.0F, 0.0F};
    assert(steady.glowAt(0) == 0.0F);
    assert(steady.brightnessAt(1000) == 0.6F);

    Star pulsing{{}, 0.8F, 1.0F, 1.570796327F, 8000.0F, 0.25F};
    assert(pulsing.glowAt(0) > 0.24F);
    assert(pulsing.glowAt(4000) < 0.00001F);
    assert(pulsing.brightnessAt(0) == 1.0F);
    return 0;
}
