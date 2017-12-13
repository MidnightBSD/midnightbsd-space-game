#ifndef BADGUY_H
#define BADGUY_H

/*
 *  BadGuy.h
 *  Allenroids
 *
 *  Created by Allen Smith on Sun Jun 20 2004.
 *
 */

#include "Spaceship.h"

class BadGuy : public Spaceship {
	
	public:
		int smartiness; //times per second to update course?
		int timeToNextUpdate;
		int rotationTimeToLive;
		int thrustTimeToLive;

	//functions
		BadGuy();
		
};

#endif