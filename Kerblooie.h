#ifndef KERBLOOIE_H
#define KERBLOOIE_H

/*
 *  Kerblooie.h
 *  Allenroids
 *
 *  Created by Allen Smith on Thu Jun 17 2004.
 *
 */

#include "GameObject.h"
#include <stdlib.h>


//kerblooie Sizes; number of particles to create in an explosion
//they MUST correspond to the array indices
#define KERBLOOIE_HUGE			5
#define KERBLOOIE_BIG			4
#define KERBLOOIE_MEDIUM		3
#define KERBLOOIE_SMALL			2
#define KERBLOOIE_TINY			1
#define KERBLOOIE_INFINITESIMAL 0


class Kerblooie : public GameObject {

	public:
		static const int kerblooieSizes[6];

		int lifespan; //how long this particle's life will actually be
		int timeToLive; //milliseconds of life this particle has left
		GLubyte color[4]; //original color and fade percentage, interpreted as alpha value
		
	//functions
		Kerblooie();
		Kerblooie(Coordinate positionIn, Vector directionIn, int lifespanIn, float particleRadius, GLubyte *colorHint);
		void drawInGame() const; //overrides function in GameObject
		virtual void draw() const;
		

};

#endif