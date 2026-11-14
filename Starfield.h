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
