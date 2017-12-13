#ifndef PLAYER_H
#define PLAYER_H

/*
 *  Player.h
 *  Allenroids
 *
 *  Maintains all data for one player, including their
 *  spaceship, their score, and status flags
 *
 *  Created by Allen Smith on Mon Jun 21 2004.
 *
 */

#include "StandardElements.h"
#include "Spaceship.h"

class Player {
	private:
		int number;
		int totalNumberOfPlayers;
		bool enabled; //is this player even active?
		GLubyte *colors;
	
	public:
		Spaceship spaceship;
		int lives; //how many lives this player has remaining
		int defaultMaxStrength; //the strength required for full health
		int score;
		int highestLevelAttained;
		
	//functions
		Player();
		Player(int numberArg, int numberOfPlayers, GLubyte *colorsArg, int livesArg, int maxStrength);
		void spawnSpaceship();
		bool isEnabled() const;
		GLubyte * getColors() const;
		void setEnabled(bool);
		

};


#endif