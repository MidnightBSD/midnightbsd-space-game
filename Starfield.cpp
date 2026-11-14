#include "Starfield.h"

#include "GameRandom.h"
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
    vertices.reserve(starCount * 6);
    for (const Star &star : stars) {
        const RenderColor color{star.whiteness, star.whiteness, star.whiteness, 1.0F};
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
        stars.push_back({{static_cast<float>(GameRandom::integer(0, FRAMEWIDTH - 1)),
                          static_cast<float>(GameRandom::integer(0, FRAMEHEIGHT - 1))},
                         GameRandom::real(0.0F, 1.0F), GameRandom::real(0.25F, 1.0F)});
    }
}
