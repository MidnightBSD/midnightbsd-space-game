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
		int playerNumber = 0;
		GLubyte color[3]{}; //RGB values range from 0 to 255 using GLubyte
		float rotationAngle = 0.0F; //in *degrees*
		
		float shieldCoveragePercentage = 0.75F; //i.e., 0.75 draws a shield bubble 3/4 the diameter of the ship. Anything outside this bubble is invulnerable
		int invinciblityTimeToLive = 0; //how long until the ship will become vulnerable again
		int drawShieldsTimeToLive = 0; //shields drawn for a little bit after their use; this is their expiration date
		int strength = 0; //how strong is our ship?
		int maxStrength = 0; //what is normal full-shield capacity (not overloaded)
		bool thrusting = false; //is our intrepid craft supposed to be accelerating?
		bool decelerating = false; //is our intrepid craft supposed to be stopping?
		int drawThrustUntil = 0; //When the thrust fire will fade out
		short rotating = NOT_ROTATING; //use COUNTERCLOCKWISE, NOT_ROTATING, CLOCKWISE
		float rotationScaleFactor = 0.0F; //the percentage of the total rotation to move by per frame (see rotateShip)

	//functions
		Spaceship() = default;
		void draw() const override;
		void drawHealthMeter(int , int hudHeight, bool growsRight) const;

	protected:
		float collisionRadius() const override;
};

#endif
