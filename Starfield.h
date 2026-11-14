//==============================================================================
//
// File:		Starfield.h
//
// Purpose:		Draw a background starfield.
//
// Modified:	04/07/2009 Allen Smith. Creation Date.
//
//==============================================================================
#ifndef _Starfield_
#define _Starfield_

#include <cmath>
#include <cstdint>
#include <vector>

#include "StandardElements.h"


////////////////////////////////////////////////////////////////////////////////
//
// struct Star
//
////////////////////////////////////////////////////////////////////////////////
struct Star
{
	Coordinate	location;
	float		whiteness;	// [0.0 - 1.0]; grayscale value
	float		size;		// [0.25 - 1.0]; point diameter
	float		pulsePhase{};	// radians; offsets the occasional glow from other stars
	float		pulsePeriodMilliseconds{};
	float		pulseAmplitude{};	// zero for steady background stars

	[[nodiscard]] float glowAt(std::uint64_t milliseconds) const {
		if (pulseAmplitude <= 0.0F || pulsePeriodMilliseconds < 1.0F) return 0.0F;
		const float angle = pulsePhase + 6.283185307F *
			(static_cast<float>(milliseconds % static_cast<std::uint64_t>(pulsePeriodMilliseconds)) /
			 pulsePeriodMilliseconds);
		const float wave = (std::sin(angle) + 1.0F) * 0.5F;
		return pulseAmplitude * wave * wave * wave * wave;
	}

	[[nodiscard]] float brightnessAt(std::uint64_t milliseconds) const {
		const float brightness = whiteness * (1.0F + glowAt(milliseconds));
		return brightness > 1.0F ? 1.0F : brightness;
	}
};


////////////////////////////////////////////////////////////////////////////////
//
// class Starfield
//
////////////////////////////////////////////////////////////////////////////////
class Starfield
{
	public:
		void SetStarCount(unsigned starCount);
		void Draw();

	private:
		void GenerateRandomStarfield();
		unsigned starCount{};
		std::vector<Star>	stars;
};


#endif
