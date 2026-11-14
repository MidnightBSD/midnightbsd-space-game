#include "Spaceship.h"

#include "Renderer.h"

#include <SDL2/SDL.h>

#include <algorithm>
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
    const auto shade = [&](float factor, float alpha = 1.0F) {
        return RenderColor{std::clamp(shipColor.red * factor, 0.0F, 1.0F),
                           std::clamp(shipColor.green * factor, 0.0F, 1.0F),
                           std::clamp(shipColor.blue * factor, 0.0F, 1.0F), alpha};
    };
    const auto nose = point(radius, 0, shade(1.15F));
    const auto port = point(radius * cos(5 * PI / 6), radius * sin(5 * PI / 6), shade(0.72F));
    const auto center = point(radius * 3 / 5 * cos(PI), radius * 3 / 5 * sin(PI), shade(0.38F));
    const auto starboard = point(radius * cos(7 * PI / 6), radius * sin(7 * PI / 6), shade(0.88F));
    const std::array<RenderVertex, 6> hull = {nose, port, center, nose, center, starboard};
    renderer->triangles(hull);

    const RenderColor panel = shade(1.35F, 0.42F);
    const std::array<RenderVertex, 12> inlays = {
        point(radius * 0.08F, radius * 0.12F, panel), point(radius * 0.55F, radius * 0.17F, panel), point(radius * 0.18F, radius * 0.24F, panel),
        point(radius * 0.08F, radius * 0.12F, panel), point(radius * 0.18F, radius * 0.24F, panel), point(radius * -0.22F, radius * 0.42F, panel),
        point(radius * 0.08F, radius * -0.12F, panel), point(radius * 0.18F, radius * -0.24F, panel), point(radius * 0.55F, radius * -0.17F, panel),
        point(radius * 0.08F, radius * -0.12F, panel), point(radius * -0.22F, radius * -0.42F, panel), point(radius * 0.18F, radius * -0.24F, panel),
    };
    renderer->triangles(inlays);

    const RenderColor canopy{0.22F, 0.75F, 1.0F, 0.82F};
    const std::array<RenderVertex, 6> cockpit = {
        point(radius * 0.14F, radius * 0.18F, canopy), point(radius * 0.14F, radius * -0.18F, canopy), point(radius * 0.58F, 0.0F, canopy),
        point(radius * -0.16F, radius * 0.10F, shade(0.20F)), point(radius * -0.16F, radius * -0.10F, shade(0.20F)), point(radius * 0.14F, 0.0F, shade(0.20F)),
    };
    renderer->triangles(cockpit);

    const RenderColor nozzle{0.08F, 0.11F, 0.15F, 1.0F};
    const std::array<RenderVertex, 6> engines = {
        point(radius * -0.42F, radius * 0.24F, nozzle), point(radius * -0.72F, radius * 0.18F, nozzle), point(radius * -0.60F, radius * 0.06F, nozzle),
        point(radius * -0.42F, radius * -0.24F, nozzle), point(radius * -0.60F, radius * -0.06F, nozzle), point(radius * -0.72F, radius * -0.18F, nozzle),
    };
    renderer->triangles(engines);

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
