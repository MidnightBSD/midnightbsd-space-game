#include "Platform.h"

#include <cassert>
#include <vector>

int main() {
    assert(legacyKeyFromSdl('a') == 'a');
    assert(legacyKeyFromSdl('Z') == 'Z');
    assert(legacyKeyFromSdl(SDLK_F1) == -1);
    assert(legacyKeyFromSdl(SDLK_F12) == -12);
    assert(legacyKeyFromSdl(SDLK_LEFT) == LegacyKeyLeft);
    assert(legacyKeyFromSdl(SDLK_INSERT) == LegacyKeyInsert);
    assert(legacyKeyFromSdl(SDLK_RETURN) == 13);
    assert(legacyKeyFromSdl(SDLK_KP_ENTER) == 3);
    assert(legacyKeyFromSdl(SDLK_KP_0) == '0');
    assert(legacyKeyFromSdl(SDLK_KP_9) == '9');
    assert(!legacyKeyFromSdl(SDLK_UNKNOWN));

    const auto viewport = makeLogicalViewport(1920, 1080, 640, 480);
    assert(viewport.x == 240 && viewport.y == 0 && viewport.width == 1440 && viewport.height == 1080);
    const auto center = logicalPointFromWindow(960, 540, 1920, 1080, viewport, 1920, 1080, 640, 480);
    assert(center && center->x == 320.0F && center->y == 240.0F);
    assert(!logicalPointFromWindow(0, 540, 1920, 1080, viewport, 1920, 1080, 640, 480));

    DeadlineQueue deadlines;
    std::vector<int> observed;
    deadlines.schedule(20, [&] { observed.push_back(2); });
    deadlines.schedule(10, [&] { observed.push_back(1); });
    deadlines.schedule(10, [&] { observed.push_back(3); });
    assert(deadlines.nextDeadline() == 10);
    deadlines.runDue(10);
    assert((observed == std::vector<int>{1, 3}));
    deadlines.runDue(20);
    assert((observed == std::vector<int>{1, 3, 2}));
    return 0;
}
