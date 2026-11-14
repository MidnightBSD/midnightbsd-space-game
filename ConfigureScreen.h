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
// Persisted legacy key values; SDL translates into this stable representation.
struct PlayerKeys {
	int thrust;
	int rotateCounterClockwise;
	int rotateClockwise;
	int fire;
	int reverse;
};

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
extern void writeConfiguration();
extern void readConfiguration();
extern void restoreDefaults();
extern std::string_view convertASCIItoText(int);
extern void configureHandleLogicalMouse(bool pressed, float x, float y);
extern void assignKey(int);
