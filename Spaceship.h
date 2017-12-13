#ifndef SPACESHIP_H
#define SPACESHIP_H

/*
 *  Spaceship.h
 *  Allenroids
 *
 *  Created by Allen Smith on Sun Jun 20 2004.
 *
 */

#include "GameObject.h"

//Rotation symbols, used as flags to indicate how the ship is being rotated
#define COUNTERCLOCKWISE -1
#define NOT_ROTATING 0
#define CLOCKWISE 1

class Spaceship : public GameObject {

	public:
		int playerNumber;
		GLubyte color[3]; //RGB values range from 0 to 255 using GLubyte
		float rotationAngle; //in *degrees*
		
		float shieldCoveragePercentage; //i.e., 0.75 draws a shield bubble 3/4 the diameter of the ship. Anything outside this bubble is invulnerable
		int invinciblityTimeToLive; //how long until the ship will become vulnerable again
		int drawShieldsTimeToLive; //shields drawn for a little bit after their use; this is their expiration date
		int strength; //how strong is our ship?
		int maxStrength; //what is normal full-shield capacity (not overloaded)
		bool thrusting; //is our intrepid craft supposed to be accelerating?
		bool decelerating; //is our intrepid craft supposed to be stopping?
		int drawThrustUntil; //When the thrust fire will fade out
		short rotating; //use COUNTERCLOCKWISE, NOT_ROTATING, CLOCKWISE
		float rotationScaleFactor; //the percentage of the total rotation to move by per frame (see rotateShip)

	//functions
		Spaceship();
		virtual void draw() const;
		void drawHealthMeter(int , int hudHeight, bool growsRight) const;
		bool intersectsWith(const GameObject *, bool twoWayCompare = true); //overrides method in GameObject
};

#endif