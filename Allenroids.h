/*
 *  Allenroids.h
 *  
 *  Main header for Allenroids
 *  
 *
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Iterator.h"

#include "Player.h"
#include "Asteroid.h"
#include "Crystal.h"
#include "Kerblooie.h"
#include "Shot.h"
#include "Spaceship.h"
#include "BadGuy.h"

#define ALLENROIDS_VERSION			"1.4"
#define ALLENROIDS_VERSION_DATE		2009


#define NUMBERSTARS					512
#define MAX_NUMBER_PLAYERS			2

//essentially the gamespeed
	//note: this is *not* the number of frames to draw each second. 
	// All of the movement constants in this game are scaled so that 
	// they should be understood *as if* the game were only drawing 
	// this many frames per second. The game may draw more or less, 
	// and it will scale all the constants accordingly.
#define FRAMES_PER_SECOND			30
//how many degrees the ship will rotate each frame, if it is rotating
#define ROTATION_ANGLE_PER_FRAME	10

//the highest number of frames the game is allowed to draw.
// (This is for power conservation.)
#define MAX_FRAMES_PER_SECOND		60

//don't insert any delay between fromes; draw as many as possible.
// (ignore MAX_FRAMES_PER_SECOND)
//#define TIGHT_IDLE_DRAWING

//print timestamps on shots
//#define DEBUG_SHOTS

//print crystal information
//#define DEBUG_CRYSTALS

//eternal invincibility; for debugging, of course
//#define ETERNAL_INVINCIBILITY

//are random numbers actually sorta random?
#define RANDOM_ON true


#define true 1
#define false 0

typedef unsigned boolean;

/****************************************************
 *
 * Data Structures
 *
 ***************************************************/
//There aren't any in here anymore; they all 
//got turned into classes or moved to StandardElements.h


/****************************************************
 *
 * Macros (General Purpose)
 *
 ***************************************************/

//Index of player number in array
#define P(x) ( (x)-1 )

//Smartiness for a level
//the number of times per second that the aliens are updated
//It's 1 for Level 1, and then follows this nifty function for the rest
#define smartiness(level) (	(int) (													\
	( (level) < 2 ) ? 9:															\
		floor(																		\
			8.774 * log( 0.118*(level) + 0.954 ) - 0.525							\
		)																			\
))



/****************************************************
 *
 * Function Prototypes
 *
 ***************************************************/
extern void gameInit(int);
extern void display(void);
extern void drawStarfield();
extern void drawStatisticsHeader();
extern void drawList(LinkedList<GameObject> *);
extern void moveSpaceShip(Spaceship *, float);
extern void moveShots(LinkedList<GameObject> *, float);
extern void moveAsteroids(LinkedList<GameObject> *, float);
extern void moveKerblooies(LinkedList<GameObject> *, float);
extern void moveCrystals(LinkedList<GameObject> *, float);
extern void collideObjects(GameObject *, GameObject *);
extern void idleByTimer(int parameter);
extern void idle(void);
void doAI(BadGuy *);
extern void updateBadGuys(LinkedList<GameObject> *, float);
extern void fireShot(Spaceship *);
extern void findShotCollisions(LinkedList<GameObject> *);
extern bool isShotHitByShots(Shot *, Iterator<GameObject> *);
extern HitData isCrystalHit(Crystal *, LinkedList<GameObject> *);
extern HitData isAsteroidHit(Asteroid *, LinkedList<GameObject> *);
extern HitData isShipHit(Spaceship *, LinkedList<GameObject> *, LinkedList<GameObject> *, LinkedList<GameObject> *);
extern void accelerateShip(Spaceship *, float);
extern void decelerateShip(Spaceship *, float);
extern void rotateShip(Spaceship *, float);
extern void impactShip(Spaceship *, HitData);
extern void resurrectShip(int);
extern void spawnStarfield();
extern void summonBadGuy(int);
extern Asteroid *spawnAsteroid(int,float,float,Vector);
extern void spawnKerblooie(int, float, GLubyte [],Coordinate, Vector, LinkedList<GameObject> *);
extern void spawnCrystal(int, Coordinate, LinkedList<GameObject> *);
extern void startLevel(int);
extern void endGame(int);
extern void handleKeyboard(unsigned char, int, int);
extern void handleSpecialKeyboard(int, int, int);
extern void handleKey(int);
extern void handleKeyboardUp(unsigned char, int, int);
extern void handleSpecialKeyboardUp(int, int, int);
extern void handleKeyUp(int);