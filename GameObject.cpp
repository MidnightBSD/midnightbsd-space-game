/*
 *  GameObject.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Mon Jun 21 2004.
 *
 */

#include "GameObject.h"

/****************************************************
 * ~GameObject
 *  destructor. This is mainly here to shut up the 
 *	compiler.
 ***************************************************/
GameObject::~GameObject()
{
	//nothing to do here.
}

/****************************************************
 * drawInGame
 *  paints the object within the game window,
 *  duplicating it for any wrapping necessary at the window edges.
 ***************************************************/
void GameObject::drawInGame() const{
	//draw in default location first
	glPushMatrix();
		glTranslatef(position.x, position.y, 0);
		draw();
	glPopMatrix();
	
	//handle wrapping on the left side
	if(position.x < radius){
		glPushMatrix();
			glTranslatef(position.x + FRAMEWIDTH, position.y, 0); //draw another asteroid just to the right of the rightmost screen border
			draw();
		glPopMatrix();
		
	}
	//handle wrapping on the right side
	else if(FRAMEWIDTH - position.x < radius){
		glPushMatrix();
			glTranslatef(0 - (FRAMEWIDTH - position.x), position.y, 0);
			draw();
		glPopMatrix();
		
	}
	//handle wrapping on the bottom
	if(position.y < radius){
		glPushMatrix();
			glTranslatef(position.x, position.y + FRAMEHEIGHT, 0);
			draw();
		glPopMatrix();
		
	}
	//handle wrapping on the top
	else if(FRAMEHEIGHT - position.y < radius){
		glPushMatrix();
			glTranslatef(position.x, 0 - (FRAMEHEIGHT - position.y), 0);
			draw();
		glPopMatrix();
		
	}
}

/****************************************************
 * adjustCoordinate
 *	modifies coordPtr by the amount of the directionPtr
 *  wraps a point around the window
 ***************************************************/
void GameObject::adjustCoordinate(float movementScaleFactor){
	position.x += direction.x * movementScaleFactor;
	position.y += direction.y * movementScaleFactor;
	
	position.x = fmod( fmod(position.x,FRAMEWIDTH)+FRAMEWIDTH, FRAMEWIDTH );
	position.y = fmod( fmod(position.y,FRAMEHEIGHT)+FRAMEHEIGHT, FRAMEHEIGHT );
}

/****************************************************
 * intersectsWith
 *	returns true if this object is inside the radius of otherObject
 *  this object is shifted around the edges of the screen for testing
 *  if twoWayCompare is set to true, then the otherObject is also shifted around
 *  note that specifying twoWayCompare is optional; it defaults to true if not present
 ***************************************************/
bool GameObject::intersectsWith(const GameObject *otherObject, bool twoWayCompare) const{
	Coordinate shiftedPosition;
	float lethalDistance = this->radius + otherObject->radius;
	
	//try the basic distance first
	if( distanceBetween(this->position, otherObject->position) < lethalDistance )
		return true;
	
	//then handle wrapping on the left side
	if(this->position.x < lethalDistance){
		shiftedPosition.x = this->position.x + FRAMEWIDTH; //shifts a new test position to just beyond the right side of the screen
		shiftedPosition.y = this->position.y;
		if( distanceBetween(shiftedPosition, otherObject->position) < lethalDistance )
			return true;
	}
	//handle wrapping on the right side
	if(FRAMEWIDTH - this->position.x < lethalDistance){
		shiftedPosition.x = 0 - (FRAMEWIDTH - this->position.x); //shifts a new test position to just beyond the left side of the screen
		shiftedPosition.y = this->position.y;
		if( distanceBetween(shiftedPosition, otherObject->position) < lethalDistance )
			return true;
	}
	//handle wrapping on the bottom
	if(this->position.y < lethalDistance){
		shiftedPosition.x = this->position.x;
		shiftedPosition.y = this->position.y + FRAMEHEIGHT; //shifts a new test position to just above the top of the screen
		if( distanceBetween(shiftedPosition, otherObject->position) < lethalDistance )
			return true;
	}
	//handle wrapping on the top
	if(FRAMEHEIGHT - this->position.y < lethalDistance){
		shiftedPosition.x = this->position.x;
		shiftedPosition.y = 0 - (FRAMEHEIGHT - this->position.y); //shifts a new test position to just below the bottom of the screen
		if( distanceBetween(shiftedPosition, otherObject->position) < lethalDistance )
			return true;
	}
	
	if(twoWayCompare == true)
		return otherObject->intersectsWith(this, false); //runs the same tests the other direction.

	//and if all else fails,
	return false; //the objects don't intersect
}
