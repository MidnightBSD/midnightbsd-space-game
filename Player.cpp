/*
 *  Player.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on Mon Jun 21 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include "Player.h"

/****************************************************
 * default constructor
 ***************************************************/
Player::Player(){
}
/****************************************************
 * constructor
 ***************************************************/
Player::Player(int numberArg, int numberOfPlayers, GLubyte *colorsArg, int livesArg, int maxStrength){
	number = numberArg;
	totalNumberOfPlayers = numberOfPlayers;
	colors = colorsArg;
	lives = livesArg;
	defaultMaxStrength = maxStrength;
	spawnSpaceship(); //ship must be generated after maxStrength is assigned
	score = 0;
	highestLevelAttained = 0;
	enabled = true;
};


/****************************************************
 * spawnSpaceship
 *  creates a spaceship for this player
 ***************************************************/
void Player::spawnSpaceship() {
	Spaceship alevin; //"alevin" is the term for a newly-hatched salmon. Hey, "spawned," "salmon," "alevin"--yeah...
	
	alevin.playerNumber = number;
	alevin.radius = 20;
	alevin.mass = 16;
	
	alevin.color[0] = colors[0];
	alevin.color[1] = colors[1];
	alevin.color[2] = colors[2];
	
	alevin.rotationAngle = 0;
	alevin.position.x = FRAMEWIDTH  * number/(totalNumberOfPlayers + 1);
	alevin.position.y = FRAMEHEIGHT * number/(totalNumberOfPlayers + 1);
	alevin.direction.x = 0;
	alevin.direction.y = 0;
	alevin.invinciblityTimeToLive = INVINCIBLE_LENGTH;
	alevin.drawShieldsTimeToLive = alevin.invinciblityTimeToLive + FADEOUTS/2;
	alevin.strength = defaultMaxStrength;
	alevin.maxStrength = defaultMaxStrength;
	alevin.thrusting = false;
	alevin.decelerating = false;
	alevin.drawThrustUntil = 0;
	alevin.rotating = NOT_ROTATING;
	
	spaceship = alevin;
}


/****************************************************
 * isEnabled
 *  returns true if the player is currently able to play
 ***************************************************/
bool Player::isEnabled() const{
	return enabled;
}

/****************************************************
 * getColors
 *  returns the RGB triplet for this player
 ***************************************************/
GLubyte * Player::getColors() const{
	return colors;
}

/****************************************************
 * setEnabled
 *  sets the enabled flag to the specified value
 ***************************************************/
void Player::setEnabled(bool newSetting){
	enabled = newSetting;
}