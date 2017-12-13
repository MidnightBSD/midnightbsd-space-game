/*
 *  Configure Screen.h
 *  Asteroids
 *
 *  Created by Allen Smith on Sat Oct 26 2002.
 *
 */

#include "TitleScreen.h"

#define NUMBER_ACTIONS 5

/****************************************************
 * Structures
 ***************************************************/
//key preferences structure. values are GLUT keycodes.
typedef struct playerKeys {
	int thrust;
	int rotateCounterClockwise;
	int rotateClockwise;
	int fire;
	int reverse;
} PlayerKeys;

/****************************************************
 * Macros
 ***************************************************/
//For translating key indices into english
#define THRUST(player)		(playerKeys[(player)].thrust)
#define ROTATE_CCW(player)  (playerKeys[(player)].rotateCounterClockwise)
#define ROTATE_CW(player)   (playerKeys[(player)].rotateClockwise)
#define FIRE(player)		(playerKeys[(player)].fire)
#define REVERSE(player)		(playerKeys[(player)].reverse)
 
/****************************************************
 * Prototypes
 ***************************************************/
extern void configureInit(void);
extern void configureDisplay(void);
extern void configurePrintTitle();
extern void configurePrintText();
extern void configurePrintButtons(MyButton *, unsigned int, GLenum, int);
extern void writeConfiguration();
extern void readConfiguration();
extern void restoreDefaults();
extern char* convertASCIItoText(int);
extern void configureHandleMouse(int, int, int, int);
extern void configureProcessHits(unsigned int, GLuint *, int);
extern void configureHandleKeyboard(unsigned char, int, int);
extern void configureHandleSpecialKeys(int, int, int);
extern void assignKey(int);
