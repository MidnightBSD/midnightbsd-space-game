/*
 *  Shot.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shot.h"

/****************************************************
 * constructor
 ***************************************************/
Shot::Shot(){};


/****************************************************
 * draw
 *  paints the shot as a dodecahedron
 ***************************************************/
void Shot::draw() const{
	float angle;
	glColor3ubv( color );
	
	//one could just draw points for these. However, drawing circles looks better, especially on resize
	glBegin(GL_TRIANGLE_FAN);
		for(angle = 0; angle < 2*PI; angle += 2*PI/12){
			glVertex2f( radius*cos(angle), 
						radius*sin(angle) );
		}
	glEnd();
}