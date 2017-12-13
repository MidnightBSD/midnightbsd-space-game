#ifndef CRYSTAL_H
#define CRYSTAL_H

/*
 *  Crystal.h
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "GameObject.h"


//Crystal Specimen Codes (can be array subscripts)
#define CRYSTAL_SHIELDPOWER 0

#define CRYSTAL_NUMBER_OF_SPECIMENS 1

#define CRYSTAL_NORMAL 0
#define CRYSTAL_IMPACTED 1
#define CRYSTAL_RETRIEVED 2


class Crystal : public GameObject {

	public:
		int timeToLive; //milliseconds of life this particle has
		float rotationAngle; //the current rotation angle of the crystal around <0,1,1>
		float rotationIncrement; //how much to increment the rotation each frame
		float thickness; //radius of crystal's arms
		short crystalFlavor; //what kind of crystal this is
		GLubyte color[4]; //original color and fade percentage, interpreted as alpha value
		HitData status; //filled with CRYSTAL_STATUS

	//functions
		Crystal();
		virtual void draw() const;
	private:
		void drawCrystalArm(float, float) const;
		void drawHalfSphere(float, int, int) const;


};

#endif