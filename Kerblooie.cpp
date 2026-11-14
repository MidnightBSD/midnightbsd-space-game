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
#include "GameRandom.h"
#include "Renderer.h"

const int Kerblooie::kerblooieSizes[6] = {6, 24, 36, 48, 60, 72};


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
		color[counter] = colorHint[counter] + GameRandom::integer(0, 255 - colorHint[counter]) * (GameRandom::boolean() ? 1 : -1); //takes hinted color and gives it some variation
	color[3] = 255; //starts out at maximum brightness
	}
/****************************************************
 * drawInGame
 *  overrides the edge-wrapping features in the superclass
 *  they aren't needed for such simple explosion particles
 ***************************************************/
void Kerblooie::drawInGame() const{
	drawCore(position.x, position.y);
}

void Kerblooie::drawCore(float x, float y) const {
	const float particleRadius = radius + (GameRandom::integer(0, 1) * 0.25F) / 2;
	Renderer::current()->filledCircle(x, y, particleRadius, 0.0F,
		{color[0] / 255.0F, color[1] / 255.0F, color[2] / 255.0F, color[3] / 255.0F}, 5);
}
