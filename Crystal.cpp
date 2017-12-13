/*
 *  Crystal.cpp
 *  Allenroids
 *
 *  Class for explosion particles
 *
 *  Created by Allen Smith on Sat Jun 19 2004.
 *
 */

#include "Crystal.h"

/****************************************************
 * constructor
 ***************************************************/
Crystal::Crystal(){
}

/****************************************************
 * draw
 *  paints the kerblooie particle
 ***************************************************/
void Crystal::draw() const{
	short counter;
	static short armVectors[3][3] = {	{1,0,0},
										{0,1,0},
										{0,0,1}
									};
		
	glLineWidth(1);
	glRotatef(rotationAngle, 0,1,1);
	for(counter = 0; counter < 3; counter++){ //draw the three struts/arms of the crystal
		glPushMatrix();
		glRotatef(90,	armVectors[counter][0],
						armVectors[counter][1],
						armVectors[counter][2]  );
		glColor4ubv( color );
		drawCrystalArm(radius, thickness);
		glColor4ub(	color[0],
					color[1],
					color[2],
					color[2]/2 );
		drawCrystalArm(	radius*(1 + 3/32),
						thickness + radius*3/32);
		glPopMatrix();	
	}//end for to draw arms
}


/****************************************************
 * drawCrystalArm
 *  generates a single arm for a crystal
 ***************************************************/
void Crystal::drawCrystalArm(float armHalfLength, float thickness) const{
	float angle;
	static int slices = 6;
	glBegin(GL_QUAD_STRIP);
		for(angle = 0; angle <= 2*PI; angle += 2*PI/slices){
			glVertex3f( thickness * cos(angle), 
						thickness * sin(angle),
						-armHalfLength );
			glVertex3f( thickness * cos(angle), 
						thickness * sin(angle),
						+armHalfLength );
		}
	glEnd();
	
	glPushMatrix();
	glTranslatef(0,0,-armHalfLength);
	glRotatef(180,1,0,0);
	drawHalfSphere(thickness,slices,5);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0,0,+armHalfLength);
	drawHalfSphere(thickness,slices,5);
	glPopMatrix();
}


/****************************************************
 * drawHalfSphere
 *  oriented based in x-y plane, rising in +z
 ***************************************************/
void Crystal::drawHalfSphere(float sphereRadius, int slices, int stacks) const{
	float angle;
	int stack;
	float radiusStack;
	for(stack = 0; stack < stacks; stack++){
		glBegin(GL_QUAD_STRIP);
			for(angle = 0; angle <= 2*PI; angle += 2*PI/slices){
				radiusStack = sphereRadius*sqrt(1-pow((float)stack/stacks,2));
				glVertex3f( radiusStack * cos(angle), 
							radiusStack * sin(angle),
							(float)(stack)*sphereRadius/stacks );
				
				radiusStack = sphereRadius*sqrt(1-pow( (float)(stack+1)/stacks, 2) );
				glVertex3f( radiusStack * cos(angle), 
							radiusStack * sin(angle),
							(float)(stack+1)*sphereRadius/stacks );
			}
		glEnd();
	}
}


