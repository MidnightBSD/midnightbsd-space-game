/*
 *  Asteroid.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "Asteroid.h"
#include "GameRandom.h"
#include "Renderer.h"
#include <stdlib.h>

#include <algorithm>
#include <array>
#include <cmath>

const int Asteroid::asteroidSizes[3] = {10, 20, 30};


/****************************************************
 * constructor
 *  creates an asteroid of the specified size at the specified location, with no velocity
 ***************************************************/
Asteroid::Asteroid(int sizeCode, float positionX, float positionY){
	size = sizeCode; //an index used to look up the actual radius in asteroidSizes
	radius = asteroidSizes[sizeCode]; //lookup the value
	
	mass = (size+1)*24;
	impacted.hitCode = false;
	color[0] = 106; //derivative of "eggplant" in the crayon box
	color[1] = 0;
	color[2] = 211;
	
	//make it so they're not all the same color
	color[0] += GameRandom::integer(0, 23) * (GameRandom::boolean() ? 1 : -1);
	color[1] += GameRandom::integer(0, 23);
	color[2] += GameRandom::integer(0, 23) * (GameRandom::boolean() ? 1 : -1);

	position.x = positionX;
	position.y = positionY;
	direction.x = 0;
	direction.y = 0;
	rotationAngle = GameRandom::integer(0, 359);
}


void Asteroid::drawCore(float x, float y) const {
	Renderer *renderer = Renderer::current();
	if (renderer == nullptr) return;

	const RenderColor base{color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, 1.0F};
	const auto shade = [&](float factor) {
		return RenderColor{std::clamp(base.red * factor, 0.0F, 1.0F),
			std::clamp(base.green * factor, 0.0F, 1.0F),
			std::clamp(base.blue * factor, 0.0F, 1.0F), 1.0F};
	};
	const float radians = static_cast<float>(rotationAngle) * PI / 180.0F;
	const float sine = std::sin(radians);
	const float cosine = std::cos(radians);
	const auto point = [&](float localX, float localY, RenderColor tint) {
		return RenderVertex{x + localX * cosine - localY * sine,
			y + localX * sine + localY * cosine, 0.0F, tint};
	};
	const auto center = [&](float localX, float localY) {
		return std::array<float, 2>{x + localX * cosine - localY * sine,
			y + localX * sine + localY * cosine};
	};

	// The darker outer shell gives the low-polygon body a defined edge.
	renderer->filledCircle(x, y, radius, static_cast<float>(rotationAngle), shade(0.34F), 7);
	renderer->filledCircle(x, y, radius * 0.92F, static_cast<float>(rotationAngle), base, 7);

	const std::array<RenderVertex, 15> facets = {
		point(radius * -0.68F, radius * 0.14F, shade(0.58F)), point(radius * -0.18F, radius * 0.06F, shade(0.90F)), point(radius * -0.32F, radius * 0.62F, shade(0.76F)),
		point(radius * -0.18F, radius * 0.06F, shade(0.90F)), point(radius * 0.55F, radius * 0.30F, shade(1.15F)), point(radius * 0.18F, radius * 0.70F, shade(0.82F)),
		point(radius * -0.18F, radius * 0.06F, shade(0.90F)), point(radius * 0.35F, radius * -0.62F, shade(0.66F)), point(radius * 0.55F, radius * 0.30F, shade(1.15F)),
		point(radius * -0.68F, radius * 0.14F, shade(0.58F)), point(radius * -0.30F, radius * -0.66F, shade(0.46F)), point(radius * -0.18F, radius * 0.06F, shade(0.90F)),
		point(radius * -0.18F, radius * 0.06F, shade(0.90F)), point(radius * -0.30F, radius * -0.66F, shade(0.46F)), point(radius * 0.35F, radius * -0.62F, shade(0.66F)),
	};
	renderer->triangles(facets);

	const auto crater = [&](float localX, float localY, float craterRadius) {
		const auto position = center(localX, localY);
		renderer->filledCircle(position[0], position[1], craterRadius, 0.0F, shade(0.32F), 6);
		renderer->filledCircle(position[0] - craterRadius * 0.18F, position[1] + craterRadius * 0.18F,
			craterRadius * 0.56F, 0.0F, shade(0.54F), 6);
	};
	crater(radius * 0.34F, radius * 0.38F, radius * 0.14F);
	crater(radius * -0.42F, radius * -0.26F, radius * 0.11F);
	crater(radius * 0.20F, radius * -0.42F, radius * 0.08F);
}
