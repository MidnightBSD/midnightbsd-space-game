#ifndef ASTEROID_H
#define ASTEROID_H

/*
 *  Asteroid.h
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */


#include "GameObject.h"


//Asteroid Sizes; index of array containing radius
#define ASTEROID_BIG 2
#define ASTEROID_MEDIUM 1
#define ASTEROID_SMALL 0
//int asteroidSizes[3] = {10, 20, 30}; //actual definition is in asteroid.cpp

class Asteroid : public GameObject {

	public:
		static const int asteroidSizes[3];
	
		int size; //actually the index of an array which defines the radii of an asteroid. Note that the radius itself is also stored
		int rotationAngle; //asteroids can be made to spin, or at least be displayed a little differently.
		GLubyte color[3]; //the color; probably will never change
		HitData impacted; //has this asteroid been hit yet? If so, it gets tossed.
	
	//functions
		Asteroid(int, float, float);
		virtual void draw() const;
};


#endif