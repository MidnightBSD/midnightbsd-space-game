/*
 *  Asteroid.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "Asteroid.h"
#include "GameRandom.h"
#include <stdlib.h>

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


/****************************************************
 * draw
 *  generically paints the asteroid at the origin
 ***************************************************/
void Asteroid::draw() const{
	float angle;
	static int numberVertices = 7;
	
	glRotatef(rotationAngle, 0,0,1);
	glColor3ubv(color);
	glBegin(GL_TRIANGLE_FAN);
		for(angle = 0; angle < 2*PI; angle += 2*PI/numberVertices){
			glVertex2f( radius*cos(angle), 
						radius*sin(angle) );
		}
	glEnd();
}
