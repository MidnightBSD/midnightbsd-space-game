/*
 *  Crystal.cpp
 *  Allenroids
 *
 *  Class for explosion particles
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "Crystal.h"
#include "Renderer.h"

#include <array>
#include <vector>

namespace {
struct Vector3 { float x; float y; float z; };

Vector3 rotate(Vector3 point, Vector3 axis, float degrees) {
	const float length = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
	axis.x /= length; axis.y /= length; axis.z /= length;
	const float radians = degrees * PI / 180.0F;
	const float cosine = cos(radians); const float sine = sin(radians);
	const float dot = axis.x * point.x + axis.y * point.y + axis.z * point.z;
	return {point.x * cosine + (axis.y * point.z - axis.z * point.y) * sine + axis.x * dot * (1 - cosine),
		point.y * cosine + (axis.z * point.x - axis.x * point.z) * sine + axis.y * dot * (1 - cosine),
		point.z * cosine + (axis.x * point.y - axis.y * point.x) * sine + axis.z * dot * (1 - cosine)};
}
}

void Crystal::drawCore(float x, float y) const {
	Renderer *renderer = Renderer::current();
	std::vector<RenderVertex> vertices;
	vertices.reserve(3 * 6 * 2 * 6 * 6);
	const std::array<Vector3, 3> axes{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
	const Vector3 globalAxis{0, 1, 1};
	const auto appendArm = [&](float armLength, float armThickness, RenderColor color, Vector3 armAxis) {
		const auto emit = [&](Vector3 point) {
			point = rotate(rotate(point, armAxis, 90.0F), globalAxis, rotationAngle);
			vertices.push_back({x + point.x, y + point.y, point.z, color});
		};
		for (int slice = 0; slice < 6; ++slice) {
			const float first = 2 * PI * slice / 6; const float second = 2 * PI * (slice + 1) / 6;
			const Vector3 a{armThickness * cos(first), armThickness * sin(first), -armLength};
			const Vector3 b{armThickness * cos(first), armThickness * sin(first), armLength};
			const Vector3 c{armThickness * cos(second), armThickness * sin(second), armLength};
			const Vector3 d{armThickness * cos(second), armThickness * sin(second), -armLength};
			emit(a); emit(b); emit(c); emit(a); emit(c); emit(d);
		}
		for (const float end : {-armLength, armLength}) {
			for (int stack = 0; stack < 5; ++stack) {
				const float z1 = stack * armThickness / 5; const float z2 = (stack + 1) * armThickness / 5;
				const float r1 = armThickness * sqrt(1 - (z1 / armThickness) * (z1 / armThickness));
				const float r2 = armThickness * sqrt(1 - (z2 / armThickness) * (z2 / armThickness));
				for (int slice = 0; slice < 6; ++slice) {
					const float first = 2 * PI * slice / 6; const float second = 2 * PI * (slice + 1) / 6;
					const float direction = end < 0 ? -1.0F : 1.0F;
					const Vector3 a{r1 * cos(first), r1 * sin(first), end + direction * z1};
					const Vector3 b{r2 * cos(first), r2 * sin(first), end + direction * z2};
					const Vector3 c{r2 * cos(second), r2 * sin(second), end + direction * z2};
					const Vector3 d{r1 * cos(second), r1 * sin(second), end + direction * z1};
					emit(a); emit(b); emit(c); emit(a); emit(c); emit(d);
				}
			}
		}
	};
	for (const Vector3 axis : axes) {
		appendArm(radius, thickness, {color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, color[3] / 255.0F}, axis);
		appendArm(radius * (1 + 3.0F / 32), thickness + radius * 3.0F / 32,
			{color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, color[2] / (2.0F * 255)}, axis);
	}
	renderer->triangles(vertices);
}


