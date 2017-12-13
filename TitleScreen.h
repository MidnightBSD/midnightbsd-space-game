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
 
#include "Player.h"

#define NUMBER_HIGH_SCORES 12


typedef struct highScore{
	char name[32];
	short level; //maximum level reached
	int score;
	time_t date;
} Score;


typedef struct mybutton { //a clickable button
	Coordinate position; //the center point
	GLubyte color[4];
	char text[32];
} MyButton;



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