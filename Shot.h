#ifndef SHOT_H
#define SHOT_H

/*
 *  Shot.h
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "GameObject.h"

#define SHOT_RADIUS 2

class Shot : public GameObject {

	public:
		int owner; //who fired this bullet?
		int timeToLive; //when this bullet was shot, from GLUT_ELAPSED_TIME
		GLubyte color[3];
		bool impacted; //has this bullet hit anything yet? If so, it gets tossed.
		
	//functions
		Shot();
		virtual void draw() const;

};

#endif