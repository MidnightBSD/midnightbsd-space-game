/*
 *  Spaceship.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Sun Jun 20 2004.
 *
 */

#include "Spaceship.h"

/****************************************************
 * constructor
 ***************************************************/
Spaceship::Spaceship(){
	shieldCoveragePercentage = 0.75; // 3/4 is the default.
};


/****************************************************
 * draw
 *  paints the spaceship, the shield bubble (if any),
 *  and the rocket thrust
 ***************************************************/
void Spaceship::draw() const{
	float angle; //for creating a circle for shields
	int spriteTimeRemaining; //how much longer fading effects like shields and thrust have left
	float spriteAlpha; //the *percentage* alpha (transparency) for the thrust and shields
	int currentTime = glutGet( GLUT_ELAPSED_TIME );
	bool shieldsOverloading;
	
	glRotatef(rotationAngle, 0,0,1);

	/* Basic Ship Model:
		glVertex2i( x, y+20);
		glVertex2i( x-10, y-10*sqrt(3) );
		glVertex2i( x, y-15);
		glVertex2i( x+10, y-10*sqrt(3) );
	*/
	
//rocket thrust; drawn first so it appears behind the ship.
	spriteTimeRemaining = drawThrustUntil - currentTime;
	if(thrusting == true || spriteTimeRemaining > 0){
		if(thrusting == true)
			spriteAlpha = 1.0; //100%
		else
			spriteAlpha = spriteTimeRemaining / (float) FADEOUTS;
		
		glBegin(GL_POLYGON);
			if(strength <= maxStrength)
				glColor4ub( 255 * strength/maxStrength,
							0, 0,
							(GLubyte) (255*spriteAlpha) );
			else //overloading
				glColor4ub( 255,
							255*(strength%maxStrength)/maxStrength,
							255*(strength%maxStrength)/maxStrength,
							(GLubyte) (255*spriteAlpha) );
			glVertex2i( 0, 0); //centerpoint; to complete the filled-in appearance
			
			glColor4ub( 128, 0, 0, (GLubyte) (255*spriteAlpha) );
			glVertex2f( (radius*4/5)*cos(5*PI/6), //port blast
						(radius*4/5)*sin(5*PI/6));
						
			if(strength <= maxStrength)
				glColor4ub( 255 * strength/maxStrength,
							0,
							0,
							(GLubyte) (255*spriteAlpha) );
			else //overloading
				glColor4ub( 255,
							255*(strength % maxStrength)/maxStrength,
							255*(strength % maxStrength)/maxStrength,
							(GLubyte) (255*spriteAlpha));
			glVertex2f( (radius*1.5)*cos(1*PI/1), //center blast
						(radius*1.5)*sin(1*PI/1));
						
			glColor4ub( 128, 0, 0, (GLubyte) (255*spriteAlpha) );
			glVertex2f( (radius*4/5)*cos(7*PI/6), //starboard blast
						(radius*4/5)*sin(7*PI/6));
						
		glEnd();
	}//end if

//the spaceship
	glColor3ubv(color);
	glBegin(GL_POLYGON);
		glVertex2f( radius, //nose point
					0);
					
		glVertex2f( radius*cos(5*PI/6), //port fin
					radius*sin(5*PI/6));
					
		glVertex2f( (radius*3/5)*cos(1*PI/1), //center point
					(radius*3/5)*sin(1*PI/1));
					
		glVertex2f( radius*cos(7*PI/6), //starboard fin
					radius*sin(7*PI/6));
	glEnd();
	
	//A Border for the ship
	glColor3ub( (GLubyte) (color[0]*0.25),
				(GLubyte) (color[1]*0.25),
				(GLubyte) (color[2]*0.25)  ); //darker color
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f( radius, //nose point
					0);
					
		glVertex2f( radius*cos(5*PI/6), //port fin
					radius*sin(5*PI/6));
					
		glVertex2f( (radius*3/5)*cos(1*PI/1), //center point
					(radius*3/5)*sin(1*PI/1));
					
		glVertex2f( radius*cos(7*PI/6), //starboard fin
					radius*sin(7*PI/6));
					
	glEnd();
	
//Draw Shields
	shieldsOverloading = false; //assume they aren't overloading; then check the assumption. Computer players can't overload
	if(playerNumber > 0){ //not a computer player
		if(strength > maxStrength)
			shieldsOverloading = true;
	}
	if(drawShieldsTimeToLive > 0 || shieldsOverloading){
		if(invinciblityTimeToLive > 0 )
			spriteAlpha = 0.3125; // 5/16ths
		else if(shieldsOverloading)
			spriteAlpha = (float)( strength % maxStrength ) / (maxStrength*OVERLOAD_FACTOR);
		else
			spriteAlpha = drawShieldsTimeToLive / (float) FADEOUTS;
		
		//Outer Ring
		glLineWidth(2.0);
		if(shieldsOverloading)
			glColor4ub(	(GLubyte) (255*spriteAlpha),
									0,
									0,
						(GLubyte) (128*spriteAlpha) );
		else if(playerNumber < 0)
			glColor4ub(	102, 255, 102, (GLubyte) (128*spriteAlpha) );
		else
			glColor4ub(	0, 128, 255, (GLubyte) (128*spriteAlpha) );
		glBegin(GL_LINE_LOOP);
			for(angle = 0; angle < 2*PI; angle += 2*PI/12){ //twelve-sided shield bubble
				glVertex2f( (radius*shieldCoveragePercentage +1)*cos(angle), (radius*shieldCoveragePercentage +1)*sin(angle) );
			}
		glEnd();
		
		//Central Shild Line!
		glLineWidth(3.0);
		if(shieldsOverloading)
			glColor4ub(	(GLubyte) (255*spriteAlpha),
									0,
									0,
						(GLubyte) (255*spriteAlpha)  );
		else if(playerNumber < 0)
			glColor4ub(	102, 255, 102, (GLubyte) (255*spriteAlpha));
		else
			glColor4ub(	0, 128, 255, (GLubyte) (255*spriteAlpha));
		glBegin(GL_LINE_LOOP);
			for(angle = 0; angle < 2*PI; angle += 2*PI/12){
				glVertex2f( (radius*shieldCoveragePercentage)*cos(angle), (radius*shieldCoveragePercentage)*sin(angle) );
			}
		glEnd();
		
		//Inner Ring
		glLineWidth(2.0);
		if(shieldsOverloading)
			glColor4ub(	(GLubyte) (255*spriteAlpha),
									0,
									0,
						(GLubyte) (128*spriteAlpha)  );
		else if(playerNumber < 0)
			glColor4ub(	102, 255, 102, (GLubyte) (128*spriteAlpha) );
		else
			glColor4ub(	0, 128, 255, (GLubyte) (128*spriteAlpha) );
		glBegin(GL_LINE_LOOP);
			for(angle = 0; angle < 2*PI; angle += 2*PI/12){
				glVertex2f( (radius*shieldCoveragePercentage -1)*cos(angle), (radius*shieldCoveragePercentage -1)*sin(angle) );
			}
		glEnd();

	}

}


/****************************************************
 * drawHealthMeter
 *  a little rectangle indicating a player's health
 ***************************************************/
void Spaceship::drawHealthMeter(int hudWidth, int hudHeight, bool growsRight) const{
	float overloadFactor;

	//create the status bar for ship strength. It changes color based on health
	if(	strength > maxStrength ){
		overloadFactor = (float)( strength % maxStrength ) / (maxStrength*OVERLOAD_FACTOR);
		glColor4ub( (GLubyte) (255*overloadFactor     ),
					(GLubyte) (255*(1-overloadFactor) ),
					0,
					128); //Overload Red
	}
	else if(strength > maxStrength*3/4)
		glColor4ub(0, 255, 0, 128); //Green
	else if(strength > maxStrength*1/2)
		glColor4ub(128, 255, 0, 128); //Lime
	else if(strength > maxStrength*1/4)
		glColor4ub(255, 255, 0, 128); //Yellow
	else // strength > 0
		glColor4ub(255, 0, 0, 128); //Red
	if(growsRight){
		glBegin(GL_POLYGON); //draw it counterclockwise for avoiding back-face culling
			glVertex2i(0, 0	 );
			glVertex2i(strength * hudWidth/maxStrength, 0 );
			glVertex2i(strength * hudWidth/maxStrength, hudHeight);
			glVertex2i(0, hudHeight);
		glEnd();
	}
	else{
		//right-justified meter
		glBegin(GL_POLYGON); //draw it counterclockwise for avoiding back-face culling
			glVertex2i(hudWidth, 0);
			glVertex2i(hudWidth, hudHeight);
			glVertex2i(hudWidth - strength * hudWidth/maxStrength, hudHeight);
			glVertex2i(hudWidth - strength * hudWidth/maxStrength, 0);
		glEnd();
	}
	
	//Create an outline around the status bar, which identifies it by color.
	glColor3ubv(color);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(0, 0 );
		glVertex2i(hudWidth, 0 );
		glVertex2i(hudWidth, hudHeight);
		glVertex2i(0, hudHeight);
	glEnd();
	
}

/****************************************************
 * intersectsWith
 *	overrides GameObject function so as to shrink the impact radius for this spaceship
 ***************************************************/
bool Spaceship::intersectsWith(const GameObject *otherObject, bool twoWayCompare){
	bool intersects;
	float originalRadius = radius;
	radius *= shieldCoveragePercentage; //shrink down the spaceship radius for the comparison
	intersects = GameObject::intersectsWith(otherObject); //now do the regular calculations, which will be affected by the shrunken radius
	radius = originalRadius; //now restore the radius to its proper size
	return intersects;
}