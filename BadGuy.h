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
		int smartiness = 0; //times per second to update course?
		int timeToNextUpdate = 0;
		int rotationTimeToLive = 0;
		int thrustTimeToLive = 0;

	//functions
		BadGuy() = default;
		
};

#endif
