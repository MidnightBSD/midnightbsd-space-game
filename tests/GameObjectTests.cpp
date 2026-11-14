#include "Spaceship.h"

#include <cassert>

class TestObject final : public GameObject {
public:
    void draw() const override {}
};

int main() {
    Spaceship ship;
    ship.position = {0.0F, 0.0F};
    ship.radius = 20.0F;
    ship.shieldCoveragePercentage = 0.5F;

    TestObject nearby;
    nearby.position = {14.0F, 0.0F};
    nearby.radius = 5.0F;

    const GameObject &asGameObject = ship;
    assert(asGameObject.intersectsWith(&nearby));
    assert(ship.radius == 20.0F);

    nearby.position = {16.0F, 0.0F};
    assert(!asGameObject.intersectsWith(&nearby));
    assert(ship.radius == 20.0F);
    return 0;
}
