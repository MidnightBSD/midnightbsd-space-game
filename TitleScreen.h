/*
 *  Title Screen.h
 *  Supplement to Allenroids
 *	Stuff to create a beautiful opening screen.
 *
 *  Created by Allen Smith on Mon Oct 21 2002.
 *  
 *
 */
#ifndef _TitleScreen_
#define _TitleScreen_ 
 
#include <algorithm>
#include <array>
#include <string_view>

#include "Player.h"

#define NUMBER_HIGH_SCORES 12


struct highScore {
	char name[32];
	short level; //maximum level reached
	int score;
	time_t date;
};

using Score = highScore;


struct MyButton { //a clickable button
	Coordinate position{}; //the center point
	std::array<GLubyte, 4> color{};
	std::array<char, 32> text{};
};

inline void setButtonText(MyButton &button, std::string_view value) {
	button.text.fill('\0');
	const auto length = std::min(value.size(), button.text.size() - 1);
	std::copy_n(value.begin(), length, button.text.begin());
}



//Prototypes
extern void titleInit();
extern void titleDisplay(void);
extern void printTitle();
extern void printHighScores();
extern void writeHighScores();
extern void readHighScores();
extern void newHighScore(int);
extern void titleIdle(void);
extern void titleHandleMouse(int, int, int, int);
extern void processHits(unsigned int, GLuint *,int);
extern void titleHandleKeyboard(unsigned char, int, int);
extern void reshape(int, int);

#endif
