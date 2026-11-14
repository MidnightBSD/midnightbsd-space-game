#include "Spaceship.h"

#include "Renderer.h"

#include <SDL2/SDL.h>

#include <array>

void Spaceship::drawCore(float x, float y) const {
    if (maxStrength <= 0) return;
    const float radians = rotationAngle * PI / 180.0F;
    const float sine = sin(radians);
    const float cosine = cos(radians);
    const auto point = [&](float localX, float localY, RenderColor color) {
        return RenderVertex{x + localX * cosine - localY * sine,
            y + localX * sine + localY * cosine, 0.0F, color};
    };
    Renderer *renderer = Renderer::current();
    if (renderer == nullptr) return;
    const int thrustRemaining = drawThrustUntil - static_cast<int>(SDL_GetTicks64());
    if (thrusting || thrustRemaining > 0) {
        const float alpha = thrusting ? 1.0F : thrustRemaining / static_cast<float>(FADEOUTS);
        const RenderColor side{0.5F, 0.0F, 0.0F, alpha};
        const RenderColor center = strength <= maxStrength
            ? RenderColor{strength / static_cast<float>(maxStrength), 0.0F, 0.0F, alpha}
            : RenderColor{1.0F, (strength % maxStrength) / static_cast<float>(maxStrength),
                (strength % maxStrength) / static_cast<float>(maxStrength), alpha};
        const std::array<RenderVertex, 6> thrust = {
            point(0, 0, center), point((radius * 4 / 5) * cos(5 * PI / 6), (radius * 4 / 5) * sin(5 * PI / 6), side), point(radius * 1.5F * cos(PI), radius * 1.5F * sin(PI), center),
            point(0, 0, center), point(radius * 1.5F * cos(PI), radius * 1.5F * sin(PI), center), point((radius * 4 / 5) * cos(7 * PI / 6), (radius * 4 / 5) * sin(7 * PI / 6), side),
        };
        renderer->triangles(thrust);
    }
    const RenderColor shipColor{color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, 1.0F};
    const auto nose = point(radius, 0, shipColor);
    const auto port = point(radius * cos(5 * PI / 6), radius * sin(5 * PI / 6), shipColor);
    const auto center = point(radius * 3 / 5 * cos(PI), radius * 3 / 5 * sin(PI), shipColor);
    const auto starboard = point(radius * cos(7 * PI / 6), radius * sin(7 * PI / 6), shipColor);
    const std::array<RenderVertex, 6> hull = {nose, port, center, nose, center, starboard};
    renderer->triangles(hull);

    const bool overloaded = playerNumber > 0 && strength > maxStrength;
    if (drawShieldsTimeToLive > 0 || overloaded) {
        const float alpha = invinciblityTimeToLive > 0 ? 0.3125F
            : overloaded ? (strength % maxStrength) / (maxStrength * OVERLOAD_FACTOR)
            : drawShieldsTimeToLive / static_cast<float>(FADEOUTS);
        const RenderColor shield = overloaded ? RenderColor{alpha, 0.0F, 0.0F, alpha}
            : playerNumber < 0 ? RenderColor{102.0F / 255, 1.0F, 102.0F / 255, alpha}
            : RenderColor{0.0F, 128.0F / 255, 1.0F, alpha};
        const float shieldRadius = radius * shieldCoveragePercentage;
        renderer->ring(x, y, shieldRadius + 1.0F, 2.0F, shield, 12);
        renderer->ring(x, y, shieldRadius, 3.0F, shield, 12);
        renderer->ring(x, y, shieldRadius - 1.0F, 2.0F, shield, 12);
    }
}

void Spaceship::drawHealthMeter(int, int, bool) const {
    // HUD health is rendered directly by the SDL/OpenGL 3.3 gameplay overlay.
}

float Spaceship::collisionRadius() const {
    return radius * shieldCoveragePercentage;
}
