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
		int number = 0;
		int totalNumberOfPlayers = 0;
		bool enabled = false; //is this player even active?
		GLubyte *colors = nullptr;
	
	public:
		Spaceship spaceship;
		int lives = 0; //how many lives this player has remaining
		int defaultMaxStrength = 0; //the strength required for full health
		int score = 0;
		int highestLevelAttained = 0;
		
	//functions
		Player() = default;
		Player(int numberArg, int numberOfPlayers, GLubyte *colorsArg, int livesArg, int maxStrength);
		void spawnSpaceship();
		bool isEnabled() const;
		GLubyte * getColors() const;
		void setEnabled(bool);
		

};


#endif
