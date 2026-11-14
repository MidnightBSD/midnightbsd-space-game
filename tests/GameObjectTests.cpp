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

    TestObject wrappingObject;
    wrappingObject.position = {-1.0F, FRAMEHEIGHT + 1.0F};
    wrappingObject.adjustCoordinate(0.0F);
    assert(wrappingObject.position.x == FRAMEWIDTH - 1.0F);
    assert(wrappingObject.position.y == 1.0F);

    TestObject leftEdge;
    leftEdge.position = {1.0F, 200.0F};
    leftEdge.radius = 5.0F;
    TestObject rightEdge;
    rightEdge.position = {FRAMEWIDTH - 2.0F, 200.0F};
    rightEdge.radius = 5.0F;
    assert(leftEdge.intersectsWith(&rightEdge));
    return 0;
}
