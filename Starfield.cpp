#include "Starfield.h"

#include "GameRandom.h"
#include "Platform.h"
#include "Renderer.h"

#include <vector>

void Starfield::SetStarCount(unsigned count) {
    starCount = count;
    GenerateRandomStarfield();
}

void Starfield::Draw() {
    auto *renderer = Renderer::current();
    if (renderer == nullptr) return;
    std::vector<RenderVertex> vertices;
    vertices.reserve(starCount * 12);
    const auto now = platformMilliseconds();
    for (const Star &star : stars) {
        const float glow = star.glowAt(now);
        if (glow > 0.001F) {
            const float glowSize = star.size * (2.0F + glow * 2.0F);
            const float glowOffset = (glowSize - star.size) * 0.5F;
            const RenderColor glowColor{1.0F, 1.0F, 1.0F, glow * 0.18F};
            const float glowLeft = star.location.x - glowOffset;
            const float glowBottom = star.location.y - glowOffset;
            vertices.insert(vertices.end(), {{glowLeft, glowBottom, 0.0F, glowColor}, {glowLeft + glowSize, glowBottom, 0.0F, glowColor},
                {glowLeft + glowSize, glowBottom + glowSize, 0.0F, glowColor}, {glowLeft, glowBottom, 0.0F, glowColor},
                {glowLeft + glowSize, glowBottom + glowSize, 0.0F, glowColor}, {glowLeft, glowBottom + glowSize, 0.0F, glowColor}});
        }
        const float brightness = star.brightnessAt(now);
        const RenderColor color{brightness, brightness, brightness, 1.0F};
        const float left = star.location.x;
        const float bottom = star.location.y;
        const float right = left + star.size;
        const float top = bottom + star.size;
        vertices.insert(vertices.end(), {{left, bottom, 0.0F, color}, {right, bottom, 0.0F, color},
            {right, top, 0.0F, color}, {left, bottom, 0.0F, color}, {right, top, 0.0F, color},
            {left, top, 0.0F, color}});
    }
    renderer->triangles(vertices);
}

void Starfield::GenerateRandomStarfield() {
    stars.clear();
    stars.reserve(starCount);
    for (unsigned index = 0; index < starCount; ++index) {
        const bool pulses = GameRandom::integer(0, 7) == 0;
        stars.push_back({{static_cast<float>(GameRandom::integer(0, FRAMEWIDTH - 1)),
                          static_cast<float>(GameRandom::integer(0, FRAMEHEIGHT - 1))},
                         GameRandom::real(0.0F, 1.0F), GameRandom::real(0.25F, 1.0F),
                         GameRandom::real(0.0F, 6.283185307F),
                         GameRandom::real(5000.0F, 12000.0F),
                         pulses ? GameRandom::real(0.15F, 0.35F) : 0.0F});
    }
}
