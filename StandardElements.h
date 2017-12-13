#ifndef STANDARDELEMENTS_H
#define STANDARDELEMENTS_H

/*
 *  StandardElements.h
 *  Allenroids
 *
 *  Created by Allen Smith on Thu Jun 17 2004.
 *
 */

#if defined(__APPLE__) || defined(MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#include <stdlib.h>


#define PI					3.141592654


#define FRAMEWIDTH			640
#define FRAMEHEIGHT			480

//how long a ghost of a shield or thrust will appear
#define FADEOUTS			625
//how long the ship is invincible at the beginning of a level
#define INVINCIBLE_LENGTH	3000
//shot lifespan, in milliseconds
#define SHOT_LIFESPAN		1000

//at what amount of surplus shields will the system overload?
//i.e., 0.75 means overload after 6 extra on maxShields of 8
#define OVERLOAD_FACTOR		0.5


/****************************************************
 *
 * Data Structures
 *
 ***************************************************/
 
typedef struct coordinate {
	float x;
	float y;
} Coordinate, Vector;

//a hit!
typedef struct hitData {
	short hitCode; //used by functions to pass special data about the hit
	int mass; //mass of the object that hit caused the hit
	Vector direction; //where the hit came from
} HitData;



/****************************************************
 *
 * Mathematical Macros
 *
 ***************************************************/

//Radians to Degrees
#define degrees(radians) ( (radians) * 180/PI)

//Degrees to Radians
#define radians(degrees) ( (degrees) * PI/180)

//distance formula between two Coordinates
#define distanceBetween(coordPtr1, coordPtr2) ( 									\
	sqrt( pow(coordPtr2.x - coordPtr1.x,2) + pow(coordPtr2.y - coordPtr1.y,2) )		\
)

//Dot product of a two-dimensional vector
#define dotProduct(vector1, vector2) (												\
	vector1.x * vector2.x + vector1.y * vector2.y									\
)

//Length of the vector
#define magnitudeOfVector(vector) ( sqrt( pow( (vector).x,2) + pow( (vector).y,2) ) )

//Angle between two vectors, in radians
#define angleBetweenVectors(vector1, vector2) (										\
	acos( dotProduct(vector1, vector2)  /											\
		  (magnitudeOfVector(vector1) * magnitudeOfVector(vector2)) )				\
)

/****************************************************
 *
 * Functions
 *
 ***************************************************/
extern void flipBytes(void *bytes, size_t bufferSize);

#endif