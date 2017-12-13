/*
 *  Kerblooie.cpp
 *  Allenroids
 *
 *  Class for explosion particles
 *
 *  Created by Allen Smith on Thu Jun 17 2004.
 *
 */

#include "Kerblooie.h"

const int Kerblooie::kerblooieSizes[6] = {6, 24, 36, 48, 60, 72};


/****************************************************
 * constructor
 ***************************************************/
Kerblooie::Kerblooie(){}

/****************************************************
 * constructor
 ***************************************************/
Kerblooie::Kerblooie(Coordinate positionIn, Vector directionIn, int lifespanIn, float particleDiameter, GLubyte *colorHint){
	int counter;
	
	position = positionIn;
	direction = directionIn;
	mass = 0; //hardly worth setting
	
	lifespan = lifespanIn;
	timeToLive = lifespan; //it was just born, and has as long to life as it is supposed to
	radius = particleDiameter/2; //I don't know why I use diameters here
	
	for(counter = 0; counter < 3; counter++)
		color[counter] = colorHint[counter] + rand()%(256-colorHint[counter])*(int)pow(-1,rand()); //takes hinted color and gives it some variation
	color[3] = 255; //starts out at maximum brightness
}

/****************************************************
 * drawInGame
 *  overrides the edge-wrapping features in the superclass
 *  they aren't needed for such simple explosion particles
 ***************************************************/
void Kerblooie::drawInGame() const{
	glPushMatrix();
		glTranslatef(position.x, position.y, 0);
		draw();//just call the main draw function
	glPopMatrix();
}

/****************************************************
 * draw
 *  paints the kerblooie particle
 ***************************************************/
void Kerblooie::draw() const{
	float angle; //used to draw round particle
	float randomParticleSize; //each kerblooie particle's size is set to "twinkle" slightly
	
	glColor4ubv( color );
	randomParticleSize = radius + (rand()%2 * 0.25)/2;
	glBegin(GL_TRIANGLE_FAN);
		for(angle = 0; angle < 2*PI; angle += 2*PI/5){
			glVertex2f( randomParticleSize*cos(angle), 
						randomParticleSize*sin(angle) );
		}
	glEnd();
}