#include "GameRandom.h"

#include <cassert>

int main() {
    GameRandom::seed(12345U);
    const int firstInteger = GameRandom::integer(-20, 20);
    const float firstReal = GameRandom::real(-1.0F, 1.0F);
    const bool firstBoolean = GameRandom::boolean();

    GameRandom::seed(12345U);
    assert(GameRandom::integer(-20, 20) == firstInteger);
    assert(GameRandom::real(-1.0F, 1.0F) == firstReal);
    assert(GameRandom::boolean() == firstBoolean);

    for(int counter = 0; counter < 1000; ++counter) {
        const int value = GameRandom::integer(2, 3);
        assert(value == 2 || value == 3);
        assert(GameRandom::real(0.0F, 1.0F) >= 0.0F);
        assert(GameRandom::real(0.0F, 1.0F) < 1.0F);
    }

    return 0;
}
