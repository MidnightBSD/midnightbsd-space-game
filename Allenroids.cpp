/*
 *  asteroids.c
 *  My very first bona-fide C program
 *
 *  Created by Allen Smith on Oct. 6, 2002.
 *
 *  Scoring System:
 *		Fire a shot:			-levelNumber
 *		Destroy Big Asteroid:	(1+playerMaxStrength-playerStrength) * levelNumber
 *		Destroy Med Asteroid:	(2+playerMaxStrength-playerStrength) * levelNumber
 *		Destroy Small Asteroid:	(3+playerMaxStrength-playerStrength) * levelNumber
 *		Clear Level:			24 * levelNumber
 *		Destroy Opponent:		(144+playerMaxStrength-playerStrength) * levelNumber
 *		Use reverse drive:		-(36 + levelNumber) per 30th of a second
 *
 */
#include "Allenroids.h"

#include "ConfigureScreen.h"
#include "sound.h"
#include "Starfield.h"
#include "TitleScreen.h"

/*****Global Variables*****/
extern SDL_AudioCVT     soundBuffer[];
extern SoundSample      channels[NUMBER_CHANNELS];
unsigned int            levelNumber                     = 0;		//0 indicates that the game has not yet started
boolean                 startingNewLevel                = false;	//this is a flag used while beginning a new level during gameplay. It must be set to false while initializing Level 1
LinkedList<GameObject>  shotList;
LinkedList<GameObject>  asteroidList;								// all the asteroids
LinkedList<GameObject>  kerblooieList;								// explosion particles
LinkedList<GameObject>  crystalList;								// special crystals
LinkedList<GameObject>  badGuyList;									// computer opponents
Starfield               starfield;
int                     millisecondsPerStandardFrame    = 1000/FRAMES_PER_SECOND;
int                     minimumMillisecondsPerFrame     = 1000/MAX_FRAMES_PER_SECOND;

GLubyte                 playerColors[2][3]              = {  {0, 255,   0},		//Player 1: Green
															 {0, 128, 255} };	//Player 2: Aqua
GLubyte                 badGuyColors[]                  = {224,   0,   0 };		//A reddish color


//Global Constants which are important
unsigned int *crystalReleaseSchedule[CRYSTAL_NUMBER_OF_SPECIMENS]; //how many crystals should be created; arranged into subarrays indicating the release schedule (see startLevel())
int crystalsReleased[CRYSTAL_NUMBER_OF_SPECIMENS]; //how many crystals have been created (the index of the subarrays above)
int *asteroidReleaseSchedule; //how many medium-sized asteroids to release
unsigned int unsmallAsteroidsDestroyedToDate; //how many unsmall asteroids have been hit so far this level
int bigAsteroidsDestroyedToDate; //how many big asteroids have been hit so far this level
int *badGuySchedule; //becomes an array containing the index at which to call a bad guy
int badGuysSummonedToDate; //how many bad guys have appeared in this level so far.

boolean paused; //true if the game is paused


//Players
unsigned short numberOfPlayers = 0;
short numberOfPlayersActive = 0;
Player playerObjects[MAX_NUMBER_PLAYERS];
extern PlayerKeys playerKeys[MAX_NUMBER_PLAYERS]; //defined in 'Configure Screen.c'


/****************************************************
 * gameInit
 *  set up the game and window
 ***************************************************/
void gameInit(int numberOfPlayersChosen){
	unsigned int counter;
	
	//set up glut function calls
	glutMouseFunc(NULL);
	glutKeyboardFunc(handleKeyboard);
	glutKeyboardUpFunc(handleKeyboardUp);
	glutSpecialFunc(handleSpecialKeyboard);
	glutSpecialUpFunc(handleSpecialKeyboardUp);
	glutIgnoreKeyRepeat(true);
	glutDisplayFunc(display);
#ifdef TIGHT_IDLE_DRAWING
	glutIdleFunc(idle);
#else
	glutTimerFunc(10, idleByTimer, 0);
#endif

	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0,0,0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glLoadIdentity();
//	gluOrtho2D(0, FRAMEWIDTH, 0, FRAMEHEIGHT);
	glOrtho(0,FRAMEWIDTH, 0, FRAMEHEIGHT,-640,640);
	
	soundInit();
	
	numberOfPlayers = numberOfPlayersChosen;
	numberOfPlayersActive = numberOfPlayersChosen;
	
	for(counter = 1; counter <= numberOfPlayers; counter++){
		playerObjects[P(counter)] = Player(counter, numberOfPlayers, playerColors[P(counter)], 3, 8); //3 lives, default strength 8
		//Player is created enabled, with score=0, highest level=0
	}
	
	//start a completely fresh game.
	levelNumber = 0;
	paused = false;

	//Blank lists must be wiped here, though they were constructed at variable elaboration
	//the reason is, if you start another game after finishing one,
	//the lists retain all their old data and must be wiped clean.
	asteroidList.clearList();
	kerblooieList.clearList();
	shotList.clearList();
	crystalList.clearList();
	badGuyList.clearList();
	
	//start level 1
	#ifdef RANDOM_ON
		srand( time(NULL) );
	#endif
	startLevel(1);
}

/****************************************************
 * display
 *  paints all the game elements
 ***************************************************/
void display(void) {
	char pausedText[] = "PAUSED"; //print this if the game is paused
	char *levelText;
	unsigned int counter;
	int stringWidth = 0;
	float scaleFactor = 0.75; //75% of original size
		
	glClear(GL_COLOR_BUFFER_BIT);
	drawStarfield();
	
	for(counter = 1; counter <= numberOfPlayers; counter++){
		if(playerObjects[P(counter)].isEnabled() == true)
			playerObjects[P(counter)].spaceship.drawInGame();
	}
	
	if(!shotList.isEmpty())
		drawList(&shotList);
	if(!asteroidList.isEmpty())
		drawList(&asteroidList);
	if( !kerblooieList.isEmpty() )
		drawList(&kerblooieList);
	if( !crystalList.isEmpty() )
		drawList(&crystalList);
	if( !badGuyList.isEmpty() )
		drawList(&badGuyList);
	
	drawStatisticsHeader();

/****	SOME VERY INTERESTING TESTS. IF SIMPLY gluOrtho2d() IN INIT WITH OUT glLoadIdentity() THESE HUGE SHAPES ARE FULLY VISIBLE!
	glColor3ub(255,255,255);
	glBegin(GL_POLYGON);
		glVertex2i(-9000, -9000);
		glVertex2i(-9000,  9000);
		glVertex2i( 9000,  9000);
		glVertex2i( 9000, -9000);
	glEnd();
	
	glColor3ub(255,0,0);
	glLineWidth(1.0);
	glBegin(GL_LINES);
		glVertex2i(  1000,  1000);
		glVertex2i( 90000, 90000);
	glEnd();
****/	
	if(paused == true){ //an overlay to indicate a paused game
		glColor4ub(128,128,128,128); //transparent grey
		glBegin(GL_POLYGON);
			glVertex2i( 0, 0);
			glVertex2i( FRAMEWIDTH, 0);
			glVertex2i( FRAMEWIDTH,  FRAMEHEIGHT);
			glVertex2i(0, FRAMEHEIGHT);
		glEnd();
		
		//Now print the word "PAUSED"
		glColor4ub(255,0,0,192);
		glLineWidth(4.0);
		glPushMatrix();
		for(counter = 0; counter < strlen(pausedText); counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , pausedText[counter]);
		}
		glTranslatef( FRAMEWIDTH/2 - (stringWidth/2)*scaleFactor, FRAMEHEIGHT/2, 0);
		glScalef(scaleFactor, scaleFactor, 0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < strlen(pausedText); counter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , pausedText[counter]);
		}
		glPopMatrix();
	} //end if(paused)
	
	//Draw big "Level X" if appropriate
	if(startingNewLevel == true && paused == false){ //"Between" levels
		//Print the new level number
		levelText = (char *) calloc( strlen( "LEVEL ")+levelNumber+1, sizeof(char) );
		sprintf(levelText, "LEVEL %d", levelNumber+1);
		glColor4ub(128, 0, 64,192); //Maroon with opacity
		glLineWidth(4.0);
		glPushMatrix();
		stringWidth = 0;
		for(counter = 0; counter < strlen(levelText); counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , levelText[counter]);
		}
		glTranslatef( FRAMEWIDTH/2 - (stringWidth/2)*scaleFactor, FRAMEHEIGHT/2, 0);
		glScalef(scaleFactor, scaleFactor, 0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < strlen(levelText); counter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , levelText[counter]);
		}
		glPopMatrix();
		free(levelText);
	}
	
	glutSwapBuffers();
}

/****************************************************
 * drawStarfield
 *  draws the stars
 ***************************************************/
void drawStarfield()
{
	starfield.Draw();
}


/****************************************************
 * drawStatisticsHeader
 *  Spits out score data and vital signs at the top of the window
 ***************************************************/
void drawStatisticsHeader(){
	char hudText[64]; //"heads-up display?"
	const int hudWidth = 80;
	const int hudHeight = 12;
	const int hudOffsetX = 8;
	const int hudOffsetY = 8;
	int characterWidth; //width of one glut monospaced character
	unsigned int counter;
	
	//create the status bar for ship strength
	glPushMatrix();
		glTranslatef(hudOffsetX, FRAMEHEIGHT - hudOffsetY - hudHeight, 0);
		playerObjects[P(1)].spaceship.drawHealthMeter(hudWidth, hudHeight, true);
	glPopMatrix();

	//print player 1 vital signs
	sprintf(hudText, "Lives:%2d Score:%6d", playerObjects[P(1)].lives >= 0 ? playerObjects[P(1)].lives : 0, playerObjects[P(1)].score); //The evil conditional operator: if lives >= 0, print lives, else print '0'
	glRasterPos2i( 2*hudOffsetX + hudWidth, FRAMEHEIGHT - hudHeight - hudOffsetY);
	for(counter = 0; counter < strlen(hudText); counter++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, hudText[counter]);
	
	//print level number
	glColor3ub(128, 0, 64); //Maroon
	sprintf(hudText, "LEVEL%3d", levelNumber);
	characterWidth = glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'A');
	glRasterPos2i( FRAMEWIDTH/2 - strlen(hudText)*characterWidth/2, FRAMEHEIGHT - hudHeight - hudOffsetY);
	for(counter = 0; counter < strlen(hudText); counter++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, hudText[counter]);
		
	//Player 2
	if(numberOfPlayers >= 2){
		//create the status bar for ship strength. It changes color based on health
		glPushMatrix();
		glTranslatef(FRAMEWIDTH - hudOffsetX - hudWidth, FRAMEHEIGHT - hudOffsetY - hudHeight, 0);
		playerObjects[P(2)].spaceship.drawHealthMeter(hudWidth, hudHeight, false);
		glPopMatrix();
		
		//print player 2 vital signs
		sprintf(hudText, "Score:%6d Lives:%2d", playerObjects[P(2)].score, playerObjects[P(2)].lives >= 0 ? playerObjects[P(2)].lives : 0); //The evil conditional operator: if lives >= 0, print lives, else print '0'
		glRasterPos2i( FRAMEWIDTH - (2*hudOffsetX + hudWidth) - strlen(hudText)*characterWidth, FRAMEHEIGHT - hudHeight - hudOffsetY);
		for(counter = 0; counter < strlen(hudText); counter++)
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, hudText[counter]);	
	}//endif for player2
	
}


/****************************************************
 * drawList
 *  draws all elements in the specified linked list
 ***************************************************/
void drawList(LinkedList<GameObject> *list){
	GameObject *currentObject;
	Iterator<GameObject> listIterator(list);
	
	currentObject = listIterator.nextObject();
	while(listIterator.atEnd() == false){
		currentObject->drawInGame();
		currentObject = listIterator.nextObject();
	}
}


/****************************************************
 * moveSpaceShip
 *  adjust's a ship's location based on its direction vector
 ***************************************************/
void moveSpaceShip(Spaceship * shipPtr, float movementScaleFactor){
	unsigned int counter;
	int numberWarningDingsPlaying = 0;
	
#ifndef ETERNAL_INVINCIBILITY
//If NOT defined
	if(shipPtr->invinciblityTimeToLive > 0)
		shipPtr->invinciblityTimeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
	if(shipPtr->drawShieldsTimeToLive > 0)
		shipPtr->drawShieldsTimeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
#endif
	if(shipPtr->playerNumber > 0){
		//Do max strength computations
		if(shipPtr->strength == shipPtr->maxStrength*(1+OVERLOAD_FACTOR) ){
			for(counter = 0; counter < NUMBER_CHANNELS; counter++){
				if(channels[counter].soundID == WARNING_SOUND(shipPtr->playerNumber) && 
						channels[counter].dataPosition != channels[counter].dataLength)
					numberWarningDingsPlaying++; //count how many times the thrust sound effect is playing
			}
			if(numberWarningDingsPlaying < 1)
				playSound(soundBuffer, WARNING_SOUND(shipPtr->playerNumber) );
		}
	}
	shipPtr->adjustCoordinate(movementScaleFactor);
}

/****************************************************
 * moveShots
 *  adjust's all shots' locations based on their direction vectors
 ***************************************************/
void moveShots(LinkedList<GameObject> * shots, float movementScaleFactor){
	Shot *shotToMovePtr;
	Iterator<GameObject> shotListIterator(shots);
		
	shotToMovePtr = static_cast<Shot *>(shotListIterator.nextObject());
	while(shotListIterator.atEnd() == false){
		shotToMovePtr->timeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
		if(shotToMovePtr->timeToLive > 0 &&
			shotToMovePtr->impacted == false){
			shotToMovePtr->adjustCoordinate(movementScaleFactor);
		}
		else{ //destroy the shot; it has expired
			#ifdef DEBUG_SHOTS
				printf("At time %d shot %d > 8\n",time(NULL),shotToMovePtr->timeFired);
			#endif
			//we could create an explosion for every shot that vanishes, but it would be pretty annoying
			//instead, we only create explosions for shots that hit each other.
			//see findShotCollisions()
			//spawnKerblooie(KERBLOOIE_INFINITESIMAL, shotToMovePtr->radius, shotToMovePtr->color, shotToMovePtr->position, shotToMovePtr->direction, &kerblooieList);
			shotListIterator.removeCurrentPosition();
		}
		shotToMovePtr = static_cast<Shot *>(shotListIterator.nextObject());
	}
}

/****************************************************
 * moveAsteroids
 *  adjusts all asteroids' locations based on their direction vectors
 *  If an asteroid has been hit by a shot, appropriate action is taken
 ***************************************************/
void moveAsteroids(LinkedList<GameObject> * asteroids, float movementScaleFactor){
	Asteroid *asteroidToMovePtr;
	Iterator<GameObject> asteroidListIterator(asteroids);
	
	unsigned short asteroidsToRelease;
	int oldAsteroidSize;
	unsigned int counter;
	Asteroid *newAsteroid;
		
	asteroidToMovePtr = static_cast<Asteroid*>(asteroidListIterator.nextObject());
	while(asteroidListIterator.atEnd() == false){
		asteroidToMovePtr->adjustCoordinate(movementScaleFactor);
		oldAsteroidSize = asteroidToMovePtr->size;
		isAsteroidHit(asteroidToMovePtr, &shotList); //if it is, an impacted flag is activated
		
		if(asteroidToMovePtr->impacted.hitCode == true){
			if(oldAsteroidSize > ASTEROID_SMALL){
				//Do we release crystals?
				for(counter = 0; counter < CRYSTAL_NUMBER_OF_SPECIMENS; counter++){
					if(crystalReleaseSchedule[counter][ crystalsReleased[counter] ] == unsmallAsteroidsDestroyedToDate ){
						spawnCrystal(
							counter, //What kind of crystal
							asteroidToMovePtr->position,
							&crystalList );
						crystalsReleased[counter]++;
						#ifdef DEBUG_CRYSTALS
							printf("C%d: %d\n",counter, crystalsReleased[counter]);
						#endif
					}
				}
				
				//Do we summon a bad guy?
				if(badGuySchedule[badGuysSummonedToDate] == unsmallAsteroidsDestroyedToDate){
					playSound(soundBuffer, WARNING_ALIEN_SOUND );
					badGuysSummonedToDate++; //do not move this line to summonBadGuy(). It doesn't always work in there.
					glutTimerFunc(0, summonBadGuy, smartiness(levelNumber) );
				}
					
				
				//Create new, smaller asteroids
				if(oldAsteroidSize == ASTEROID_BIG){
					asteroidsToRelease = asteroidReleaseSchedule[bigAsteroidsDestroyedToDate]; //schedule defined in startLevel()
					//printf("Number New: %d\n",asteroidReleaseSchedule[bigAsteroidsDestroyedToDate]);
					bigAsteroidsDestroyedToDate++;
				}
				else //This will happen on ASTEROID_MEDIUM
					asteroidsToRelease = rand()%2+2; //just release a random number. No need to schedule these.
				for(counter = 0; counter < asteroidsToRelease; counter++){ 
					newAsteroid = spawnAsteroid(
						oldAsteroidSize - 1,
						asteroidToMovePtr->position.x + ( rand()%10 *pow(-1,rand()) ),
						asteroidToMovePtr->position.y + ( rand()%10 *pow(-1,rand()) ),
						asteroidToMovePtr->direction
					);
					asteroids->insertObject(newAsteroid);
				}
				#ifdef DEBUG_CRYSTALS
					printf("Impacts: %d\n",unsmallAsteroidsDestroyedToDate);
				#endif
				unsmallAsteroidsDestroyedToDate++; //just a counter for unsmall (potentially crystal-causing) impacts
			}
			spawnKerblooie(
				oldAsteroidSize + KERBLOOIE_SMALL, //so it sizes up from small, allowing us to have kerblooies smaller than 'small'
				Asteroid::asteroidSizes[oldAsteroidSize]/2,
				asteroidToMovePtr->color,
				asteroidToMovePtr->position,
				asteroidToMovePtr->direction,
				&kerblooieList
			);
			asteroidListIterator.removeCurrentPosition(); //since it has been hit by a shot!
		}
		asteroidToMovePtr = static_cast<Asteroid*>(asteroidListIterator.nextObject());
	}//end while
}


/****************************************************
 * moveKerblooies
 *  adjusts all explosion particles appropriately
 ***************************************************/
void moveKerblooies(LinkedList<GameObject> * kerblooies, float movementScaleFactor){
	Kerblooie *kerblooieToMovePtr;
	float colorFade;
	Iterator<GameObject> kerblooieListIterator(kerblooies);
		
	kerblooieToMovePtr = static_cast<Kerblooie*>(kerblooieListIterator.nextObject());
	while(kerblooieListIterator.atEnd() == false){
		kerblooieToMovePtr->timeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
		if(kerblooieToMovePtr->timeToLive > 0){
			kerblooieToMovePtr->adjustCoordinate(movementScaleFactor);
			colorFade =  kerblooieToMovePtr->timeToLive / (float) kerblooieToMovePtr->lifespan;
			kerblooieToMovePtr->color[3] = (GLubyte) floor( 255 * colorFade); //adjust alpha, which starts at 255
		}
		else{
			kerblooieListIterator.removeCurrentPosition();
		}
		kerblooieToMovePtr = static_cast<Kerblooie*>(kerblooieListIterator.nextObject());
	}
}


/****************************************************
 * moveCrystals
 *  adjusts all crystals appropriately, killing those that have expired
 ***************************************************/
void moveCrystals(LinkedList<GameObject> * crystals, float movementScaleFactor){
	Crystal *crystalToMovePtr;
	Iterator<GameObject> crystalListIterator(crystals);
		
	crystalToMovePtr = static_cast<Crystal *>(crystalListIterator.nextObject());
	while(crystalListIterator.atEnd() == false){
		crystalToMovePtr->timeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
		isCrystalHit(crystalToMovePtr, &shotList); //First determine if the crystal is still alive
		if(crystalToMovePtr->timeToLive > 0 && crystalToMovePtr->status.hitCode == CRYSTAL_NORMAL){
			crystalToMovePtr->adjustCoordinate(movementScaleFactor);
			crystalToMovePtr->rotationAngle = fmod(crystalToMovePtr->rotationAngle + crystalToMovePtr->rotationIncrement * movementScaleFactor, 360);
		}
		else{
			if(crystalToMovePtr->status.hitCode == CRYSTAL_RETRIEVED)
				playSound(soundBuffer, RETRIEVE_CRYSTAL_SOUND);
			else{
				if(crystalToMovePtr->status.hitCode == CRYSTAL_IMPACTED) //crystal shot
					playSound(soundBuffer, CRYSTAL_SPLATTER_SOUND);
				//else if(crystalToMovePtr->timeToLive <= 0)
				spawnKerblooie(
					KERBLOOIE_SMALL,
					crystalToMovePtr->radius/2,
					crystalToMovePtr->color,
					crystalToMovePtr->position,
					crystalToMovePtr->direction,
					&kerblooieList );
			}
			crystalListIterator.removeCurrentPosition();
		}
		crystalToMovePtr = static_cast<Crystal *>(crystalListIterator.nextObject());
	}	
}

/****************************************************
 * collideObjects
 *	an actual physics equation! (corrected as of version 1.2, Jul. 27, 2004)
 *	Note: 55% of velocity is lost in a collision. (not as in the physics equation!)
 ***************************************************/
void collideObjects(GameObject *hitter, GameObject *hittee){
	Vector hitterInitialVelocity = hitter->direction; //initial values must be preserved, because they will be referenced in both equations
	Vector hitteeInitialVelocity = hittee->direction;
	
	static float dampendingFactor = 0.45;

	hitter->direction.x = dampendingFactor * ( (hitter->mass - hittee->mass)*hitterInitialVelocity.x + 2 * hittee->mass * hitteeInitialVelocity.x) / (hitter->mass + hittee->mass);
	hitter->direction.y = dampendingFactor * ( (hitter->mass - hittee->mass)*hitterInitialVelocity.y + 2 * hittee->mass * hitteeInitialVelocity.y) / (hitter->mass + hittee->mass);
	
	hittee->direction.x = dampendingFactor * (2 * hitter->mass * hitterInitialVelocity.x + (hittee->mass - hitter->mass) * hitteeInitialVelocity.x) / (hitter->mass + hittee->mass);
	hittee->direction.y = dampendingFactor * (2 * hitter->mass * hitterInitialVelocity.y + (hittee->mass - hitter->mass) * hitteeInitialVelocity.y) / (hitter->mass + hittee->mass);
}


/****************************************************
 * idleByTimer
 *  controls animation on a periodic basis
 *	There are two ways to do animation:
 *		1) in a tight idle loop--squeeze in as many 
 *		   frames as the processor can do.
 *		2) on a periodic timer, so there is a maximum
 *		   number of frames.
 *
 *	This function allows for #2, since #1 consumes 
 *	way more of the processor than is warranted for 
 *	this simple game.
 *
 *	Note: I tried to avoid this by using 
 *	SLD_Delay/nanosleep, but it caused everything to 
 *	become HUGELY unresponsive.
 *
 *	controlled by TIGHT_IDLE_DRAWING
 ***************************************************/
void idleByTimer(int delayForThisTimer){

	static int previousStartTime = 0; //time at beginning of previous calculation/draw cycle
	int previousFrameCalcDrawTime;
	int millisecondsTillNextFrame = 0;
	
	//schedule delay until next frame based on *previous* frame's 
	//drawing/calculation time. (It's probably a pretty good estimate. 
	//We can't do the current frame because we don't know how long 
	//drawing is going to take.)
						   // = calcTime + drawTime
	previousFrameCalcDrawTime = glutGet(GLUT_ELAPSED_TIME) - previousStartTime - delayForThisTimer;
	if(previousFrameCalcDrawTime < minimumMillisecondsPerFrame){
		millisecondsTillNextFrame = minimumMillisecondsPerFrame - previousFrameCalcDrawTime;
	}
	
	//mark the new time just before doing the calculations and drawing for the next frame.
	previousStartTime = glutGet(GLUT_ELAPSED_TIME);
	
	//Go!
	if(paused == false){
		idle(); //do all update calculations
		glutTimerFunc(millisecondsTillNextFrame, idleByTimer, millisecondsTillNextFrame); //schedule the next frame
	}
	
}//end idleByTimer

/****************************************************
 * idle
 *  controls animation
 ***************************************************/
void idle(void){
	static int previousSystemTime;
	int timeForPreviousFrame;
	unsigned int counter;
	float movementScaleFactor;
	
	timeForPreviousFrame = glutGet(GLUT_ELAPSED_TIME) - previousSystemTime;
	movementScaleFactor = (float)timeForPreviousFrame/millisecondsPerStandardFrame;
	movementScaleFactor = fmod(movementScaleFactor, 1);
	
	//Record the new "previous system time" before doing all the movement calculations
	previousSystemTime = glutGet(GLUT_ELAPSED_TIME);

	//generic objects
	if(shotList.isEmpty() == false){
		moveShots(&shotList, movementScaleFactor);
		findShotCollisions(&shotList);
	}
	if(asteroidList.isEmpty() == false)
		moveAsteroids(&asteroidList, movementScaleFactor);
	if( kerblooieList.isEmpty() == false )
		moveKerblooies(&kerblooieList, movementScaleFactor);
	if( crystalList.isEmpty() == false )
		moveCrystals(&crystalList, movementScaleFactor);
		
	//player operations
	for(counter = 1; counter <= numberOfPlayers; counter++){
		if(playerObjects[P(counter)].isEnabled() == true){
			if(playerObjects[P(counter)].spaceship.rotating != NOT_ROTATING)
				rotateShip(&(playerObjects[P(counter)].spaceship), movementScaleFactor);
			if(playerObjects[P(counter)].spaceship.thrusting == true)
				accelerateShip(&(playerObjects[P(counter)].spaceship), movementScaleFactor);
			if(playerObjects[P(counter)].spaceship.decelerating == true)
				decelerateShip(&(playerObjects[P(counter)].spaceship), movementScaleFactor);
			
			moveSpaceShip(&(playerObjects[P(counter)].spaceship), movementScaleFactor);
		}
	}
	
	//bad guy operations (also assesses damage, so must happen after shot updates, etc.)
	updateBadGuys(&badGuyList, movementScaleFactor);
	
	//assess damage to players (must be done after shots are moved)
	for(counter = 1; counter <= numberOfPlayers; counter++){
		if(playerObjects[P(counter)].isEnabled() == true){
			impactShip(&playerObjects[P(counter)].spaceship, isShipHit(&(playerObjects[P(counter)].spaceship), &asteroidList, &shotList, &crystalList) );
		}
	}
	
	//starts new level if needs be
	if(asteroidList.isEmpty() && startingNewLevel == false){
		glutTimerFunc(0, startLevel, levelNumber+1); //start new level after four seconds (handled inside function call)
		startingNewLevel = true;
	}
	
	//windowing
	glutPostRedisplay();
	
}


/****************************************************
 * updateBadGuys
 *  moves badGuys and calls AI if necessary
 ***************************************************/
void updateBadGuys(LinkedList<GameObject> *badGuys, float movementScaleFactor){
	BadGuy *currentBadGuy;
	Iterator<GameObject> badGuyIterator(badGuys);
	
	currentBadGuy = static_cast<BadGuy*>(badGuyIterator.nextObject());
	while(badGuyIterator.atEnd() == false){
		//First do all updating
		if(currentBadGuy->rotating != NOT_ROTATING) //because rotation can be +1 or -1, *not* a boolean
			rotateShip(currentBadGuy, movementScaleFactor);
		if(currentBadGuy->thrusting == true)
			accelerateShip(currentBadGuy, movementScaleFactor);
		if(currentBadGuy->decelerating == true)
			decelerateShip(currentBadGuy, movementScaleFactor);
		
		//Rotation
		currentBadGuy->rotationTimeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
		if(currentBadGuy->rotationTimeToLive <= 0)
			currentBadGuy->rotating = NOT_ROTATING;
		//Thrust
		currentBadGuy->thrustTimeToLive -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
		if(currentBadGuy->thrustTimeToLive <= 0 && currentBadGuy->thrusting == true){
			currentBadGuy->thrusting = false;
			currentBadGuy->drawThrustUntil = glutGet( GLUT_ELAPSED_TIME ) + FADEOUTS;
			playSound(soundBuffer, STOP_THRUSTING_SOUND);

		}
		
		moveSpaceShip(currentBadGuy, movementScaleFactor);
		impactShip(currentBadGuy, isShipHit(currentBadGuy, &asteroidList, &shotList, &crystalList) );
		
		//wipe him out if necessary
		if(currentBadGuy->strength <= 0)
			badGuyIterator.removeCurrentPosition();
		else{
			//Do AI, if necessary
			currentBadGuy->timeToNextUpdate -= (int) (millisecondsPerStandardFrame * movementScaleFactor);
			if(currentBadGuy->timeToNextUpdate <= 0){
				doAI(currentBadGuy);
				currentBadGuy->timeToNextUpdate = (FRAMES_PER_SECOND * millisecondsPerStandardFrame) / (currentBadGuy->smartiness);
			}
			
		}
		
		//get the next bad guy in the badGuys list
			currentBadGuy = static_cast<BadGuy*>(badGuyIterator.nextObject()); //gets the next bad guy in the badGuys list
	}//end while
}


/****************************************************
 * doAI
 *  calculates so-called "intelligence" for a bad guy
 *  the Idea:
 *
 *  find closest player
 *  rotate ship to align with player
 *  	get dot product
 *  	set rotationScaleFactor = 1
 *  thrust to player
 *  player in range?
 *  fire shot
 *
 ***************************************************/
void doAI(BadGuy *badGuy){
	int counter;
	float distance;
	float distanceToPlayer = FRAMEHEIGHT*FRAMEWIDTH; //highball the number so that the actual distance will be smaller
	int playerToAttack = 0; //an invalid player number
	Vector vectorToPlayer;
	Vector badGuyNoseVector; //the direction the nose is pointing in
	double angleBetweenBadGuyNoseAndDirection;
	double angleToPlayer;
	float shotSpeed; //in pixels per frame
	float shotRange;
	
	
//find closest player
	for(counter = 1; counter <= numberOfPlayers; counter++){
		if(playerObjects[P(counter)].isEnabled() == true){
			distance = distanceBetween(badGuy->position, playerObjects[P(counter)].spaceship.position);
			if(distance < distanceToPlayer){
				distanceToPlayer = distance;
				playerToAttack = counter;
			}
		}
	}//end minimum distance calculation
	
if(playerToAttack != 0){ //then a valid player does exist. Note that there may not always be a player on the screen!
//Get Angle Between ships
	vectorToPlayer.x = playerObjects[P(playerToAttack)].spaceship.position.x - badGuy->position.x;
	vectorToPlayer.y = playerObjects[P(playerToAttack)].spaceship.position.y - badGuy->position.y;
	badGuyNoseVector.x = badGuy->radius * cos( radians(badGuy->rotationAngle) );
	badGuyNoseVector.y = badGuy->radius * sin( radians(badGuy->rotationAngle) );
	angleToPlayer = degrees( angleBetweenVectors(badGuyNoseVector, vectorToPlayer) );
	angleBetweenBadGuyNoseAndDirection = degrees(angleBetweenVectors(badGuyNoseVector, badGuy->direction) );
	
//Rotate into alignment
	if(angleToPlayer != 0){
		badGuy->rotating = (short) -(badGuyNoseVector.x * vectorToPlayer.y - badGuyNoseVector.y * vectorToPlayer.x); //this is the k-term of a cross product. The direction it points is the opposite of my clockwise/counterclockwise convention!
		if(badGuy->rotating != NOT_ROTATING)
			badGuy->rotating = badGuy->rotating/abs(badGuy->rotating); //normalize to either 1 or -1
	}
	badGuy->rotationScaleFactor = 1 + levelNumber/100.0;
	badGuy->rotationTimeToLive = (int) (angleToPlayer/(ROTATION_ANGLE_PER_FRAME*badGuy->rotationScaleFactor) * millisecondsPerStandardFrame);
	//printf("Doing AI: angle to player %d: %f; rotating: %d, timeToRotate: %d\n", playerToAttack, angleToPlayer, badGuy->rotating, badGuy->rotationTimeToLive);	

//Calculate shot range
	shotSpeed = sqrt( 2*pow(badGuy->strength,2) );
	shotRange = shotSpeed * FRAMES_PER_SECOND * SHOT_LIFESPAN / 1000; //the 1000 is because SHOT_LIFESPAN is in milliseconds

//Thrust to Player
	if(distanceToPlayer > shotRange &&
	   angleToPlayer < 20 &&
	   (angleBetweenBadGuyNoseAndDirection > 10 ||
	    magnitudeOfVector(badGuy->direction) < 0.25*shotSpeed )
	  ){
		badGuy->thrustTimeToLive = 62 + rand()%62;
		badGuy->thrusting = true;
	}
	
//ATTACK!
	if(1.25*shotRange > distanceToPlayer && angleToPlayer < 10){
		fireShot(badGuy);
	}
}//end valid player check
}//end AI function


/****************************************************
 * fireShot
 *  fires a shot by the designated spaceship
 ***************************************************/
void fireShot(Spaceship * player){
	Shot *newshot;
	
	newshot = new Shot;
	if(player->playerNumber > 0)
		playSound(soundBuffer, POP_SOUND);
	else
		playSound(soundBuffer, POP_ALIEN_SOUND);
		
	newshot->owner = player->playerNumber;
	newshot->timeToLive =  SHOT_LIFESPAN; //1 second
	newshot->impacted = false;
	
	newshot->position.x = player->position.x +
						player->radius*cos(player->rotationAngle * PI/180 ) *1.125; //move the shot away from the ship
	newshot->position.y = player->position.y +
						player->radius*sin(player->rotationAngle * PI/180 ) *1.125;
						
	newshot->direction.x = player->direction.x +
						player->strength* cos(player->rotationAngle * PI/180);
	newshot->direction.y = player->direction.y +
						player->strength* sin(player->rotationAngle * PI/180);
	
	newshot->mass = 1;
	newshot->radius = SHOT_RADIUS;
	newshot->color[0] = player->color[0];
	newshot->color[1] = player->color[1];
	newshot->color[2] = player->color[2];
	
	if(player->playerNumber > 0)
		playerObjects[P(player->playerNumber)].score -= levelNumber; //it costs you to shoot!
	shotList.insertObject(newshot);
}

/****************************************************
 * findShotCollisions
 *  test all shots for collisions with each other
 *  if your shot is hit, you get the points for firing it back
 *  creates explosions for impacted shots here
 ***************************************************/
void findShotCollisions(LinkedList<GameObject> *shots){
	Shot *shotToTest;
	Iterator<GameObject> shotListIterator(shots);
	shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	while(shotListIterator.atEnd() == false){
		if(shotToTest->impacted == false)
			isShotHitByShots(shotToTest, new Iterator<GameObject>(shots, shotListIterator.getCurrentNode()) ); //only tests this shot against shots that haven't been tested yet
		if(shotToTest->impacted == true){
			playSound(soundBuffer, SHOT_TUNK_SOUND);
			spawnKerblooie(KERBLOOIE_INFINITESIMAL, shotToTest->radius, shotToTest->color, shotToTest->position, shotToTest->direction, &kerblooieList);
			if(shotToTest->owner > 0)
				playerObjects[P(shotToTest->owner)].score += levelNumber; //you get your deposit back
		}
		shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	}
	
}


/****************************************************
 * isShotHitByShots
 *  test this shot for collisions all the other shots in this iterator
 *  sets the impacted flag if it is hit
 *  also returns flag specifying a hit or not
 ***************************************************/
bool isShotHitByShots(Shot *shotToTest, Iterator<GameObject> *shotComparisonIterator){
	Shot *shotToCompare;
	bool isHit = false;
	shotToCompare = static_cast<Shot *>(shotComparisonIterator->nextObject());
	while(shotComparisonIterator->atEnd() == false){
//		if(distanceBetween(shotToTest->position, shotToCompare->position) <= shotToCompare->radius*1.75){ //I let the shots enter each other a little bit for visual effect
		if(shotToTest->intersectsWith(shotToCompare)){ //we could do it the old way above, but this is less apt to "miss"
			shotToTest->impacted = true;
			shotToCompare->impacted = true; //these two shots have collided
			collideObjects(shotToTest, shotToCompare);
		}
		shotToCompare = static_cast<Shot *>(shotComparisonIterator->nextObject());
	}
	return isHit;
}


/****************************************************
 * isCrystalHit
 *  returns true if the crystal has been hit by a shot
 *  the shot that hit the crystal is flagged for deletion from the list
 ***************************************************/
HitData isCrystalHit(Crystal *crystal, LinkedList<GameObject> *shots){
	Shot *shotToTest;
	HitData isHit;
	Iterator<GameObject> shotListIterator(shots);
	
	isHit.hitCode = false;
	shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	while(shotListIterator.atEnd() == false  && isHit.hitCode == false){
		if(crystal->intersectsWith(shotToTest) && shotToTest->impacted == false){
			isHit.hitCode = CRYSTAL_IMPACTED;
			isHit.mass = shotToTest->mass;
			isHit.direction = shotToTest->direction;
			shotToTest->impacted = true;
			crystal->status = isHit;
			collideObjects(shotToTest, crystal);
		}
		else
			shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	}
	
	return isHit;
}


/****************************************************
 * isAsteroidHit
 *  returns true if the asteroid has been hit by a shot
 *  the shot that hit the asteroid is flagged for deletion from the list
 ***************************************************/
HitData isAsteroidHit(Asteroid *asteroid, LinkedList<GameObject> *shots){
	Shot *shotToTest;
	HitData isHit;
	int shotOwner;
	Iterator<GameObject> shotListIterator(shots);

	
	isHit.hitCode = false;
	
	shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	while(shotListIterator.atEnd() == false  && isHit.hitCode == false){
		if( asteroid->intersectsWith(shotToTest) ){ //a hit!
			isHit.hitCode = true;
			isHit.mass = shotToTest->mass;
			isHit.direction = shotToTest->direction;
			shotToTest->impacted = true;
			asteroid->impacted = isHit;
			
			#ifdef DEBUG_SHOTS
				printf("Shot before impact: <%f, %f>\n", shotToTest->direction.x, shotToTest->direction.y);
			#endif
			collideObjects( shotToTest, asteroid );
			#ifdef DEBUG_SHOTS
				printf("Shot after impact: <%f, %f>\n", shotToTest->direction.x, shotToTest->direction.y);
			#endif
			
			//Scoring:
			shotOwner = shotToTest->owner;
			
			if(shotOwner > 0){
				if(playerObjects[P(shotOwner)].spaceship.strength > 0)
					playerObjects[P(shotOwner)].score += 
						( abs(asteroid->size - 3) +
							playerObjects[P(shotOwner)].spaceship.maxStrength /
							playerObjects[P(shotOwner)].spaceship.strength )
						* levelNumber;
				else
					playerObjects[P(shotOwner)].score += 
						( abs(asteroid->size - 3) +
							playerObjects[P(shotOwner)].spaceship.maxStrength )
						* levelNumber;
			}
		}
		else
			shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	}//end while

	return isHit;
}

/****************************************************
 * isShipHit
 *  returns int for amount by which to decrease ship strength
 *   -> 1 for a shot, otherwise dependent on asteroid size
 *  the shot that hit the ship is flagged for deletion from the list
 ***************************************************/
HitData isShipHit(Spaceship *shipPtr, LinkedList<GameObject> *asteroids, LinkedList<GameObject> *shots, LinkedList<GameObject> *crystals){
	Crystal *crystalToTest;
	Shot *shotToTest;
	Asteroid *asteroidToTest;
	HitData isHit; //how much impact did the ship absorb?
	int shotOwner;
	int playerNumber = shipPtr->playerNumber;
	int scoreAssessed = 0;
	Iterator<GameObject> shotListIterator(shots);
	Iterator<GameObject> crystalListIterator(crystals);
	Iterator<GameObject> asteroidListIterator(asteroids);
	
	isHit.hitCode = 0; //Stores the number of impacts sustained; no hits yet
	isHit.mass = 0;
	isHit.direction.x = 0;
	isHit.direction.y = 0;
	
//Test crystals for intersection
	if( shipPtr->invinciblityTimeToLive <= 0 ){ //non-invincible players
		crystalToTest = static_cast<Crystal *>(crystalListIterator.nextObject());
		while(crystalListIterator.atEnd() == false  && isHit.hitCode == false){
			if(shipPtr->intersectsWith(crystalToTest)){
				crystalToTest->status.hitCode = CRYSTAL_RETRIEVED;
				//Assess result of crystal collection
				switch(crystalToTest->crystalFlavor){
					case CRYSTAL_SHIELDPOWER:
						playSound(soundBuffer, RETRIEVE_CRYSTAL_SOUND);
						shipPtr->strength++;
						if(playerNumber > 0){
							/*Code required to give extra life after exceeding max strength (not used)
							if(playerObjects[P(playerNumber)].spaceship.strength > playerMaxStrength[P(playerNumber)]){
								playerObjects[P(playerNumber)].spaceship.strength %= playerMaxStrength[P(playerNumber)];
								playerObjects[P(playerNumber)].lives++;
							}*/
							if(playerObjects[P(playerNumber)].spaceship.strength > playerObjects[P(playerNumber)].spaceship.maxStrength*(1+OVERLOAD_FACTOR) )
								isHit.hitCode = playerObjects[P(playerNumber)].spaceship.strength; //destroy players who overload their shields
						}
						break;
				}
			}
			crystalToTest = static_cast<Crystal *>(crystalListIterator.nextObject());
		}//end while
	}//end crystal search
	

//Test shots for impact
	shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	while(shotListIterator.atEnd() == false  && isHit.hitCode == false){
		if(shipPtr->intersectsWith(shotToTest)){
			collideObjects(shotToTest, shipPtr);
			isHit.mass += shotToTest->mass;
			isHit.hitCode++;
			shotToTest->impacted = true;
			
			//Scoring:
			shotOwner = shotToTest->owner;
			//Don't award points for shooting oneself.
			//Potential Computer Players might be given negative numbers, hence the second condition
			if(shotOwner != playerNumber && shotOwner >= 1 ){
				if(playerObjects[P(shotOwner)].spaceship.strength > 0)
					scoreAssessed += 
						(144 +
							playerObjects[P(shotOwner)].spaceship.maxStrength / 
							playerObjects[P(shotOwner)].spaceship.strength
						) * levelNumber;
				else
					scoreAssessed +=
						(144 +
							playerObjects[P(shotOwner)].spaceship.maxStrength
						) * levelNumber;
				if(playerNumber < 0)
					scoreAssessed /= 2;
				playerObjects[P(shotOwner)].score += scoreAssessed;
			}
		}//end impact test
		shotToTest = static_cast<Shot *>(shotListIterator.nextObject());
	}//end shot testing
	

//Test asteroids for impact
	asteroidToTest = static_cast<Asteroid *>(asteroidListIterator.nextObject());
	while(asteroidListIterator.atEnd() == false){
		if(shipPtr->intersectsWith(asteroidToTest) ){
			collideObjects(asteroidToTest, shipPtr);
			isHit.mass += asteroidToTest->mass;
			if(playerNumber > 0){
				isHit.hitCode += (asteroidToTest->size*2 +1);
			}
			asteroidToTest->impacted.hitCode = true;
			asteroidToTest->impacted.mass = shipPtr->mass;
			asteroidToTest->impacted.direction = shipPtr->direction;
			
			if(playerNumber > 0){ //no score for computer players
				//Scoring (yes, you get points for ramming the asteroids!):
				if(playerObjects[P(playerNumber)].spaceship.strength > 0)
					scoreAssessed = (int)
						( abs(asteroidToTest->size - 3) +
							(float) playerObjects[P(playerNumber)].spaceship.maxStrength / 
							playerObjects[P(playerNumber)].spaceship.strength
						) * levelNumber;
				else
					scoreAssessed =
						abs(asteroidToTest->size - 3) * levelNumber;
			//	printf("Strength: %d; factor: %f; score assessed: %d\n",playerObjects[P(playerNumber)].spaceship.strength, (float) playerMaxStrength[P(playerNumber)]/playerObjects[P(playerNumber)].spaceship.strength, scoreAssessed);	
				if(shipPtr->invinciblityTimeToLive > 0)
					scoreAssessed /= 2;
				playerObjects[P(playerNumber)].score += scoreAssessed;
			}//end computer-player point guard

		}//end impact
		
		asteroidToTest = static_cast<Asteroid *>(asteroidListIterator.nextObject());
	}//end asteroid testing

	return isHit;
}

/****************************************************
 * accelerateShip
 *  move a ship
 ***************************************************/
void accelerateShip(Spaceship * player, float movementScaleFactor){
	float acceleration = 0.625; //magnitude to increase the direction (velocity) vector
	unsigned int counter, numberThrustSounds = 0;
	
	acceleration *= movementScaleFactor;
	if(player->thrusting == true){
		for(counter = 0; counter < NUMBER_CHANNELS; counter++){
			if(channels[counter].soundID == THRUST_SOUND && 
					channels[counter].dataPosition != channels[counter].dataLength)
				numberThrustSounds++; //count how many times the thrust sound effect is playing
		}
		if(numberThrustSounds < numberOfPlayers)
			playSound(soundBuffer, THRUST_SOUND);

		player->direction.x += acceleration*(player->strength/ (float) player->maxStrength)*cos(player->rotationAngle * PI/180);
		player->direction.y += acceleration*(player->strength/ (float) player->maxStrength)*sin(player->rotationAngle * PI/180);
	}
}

/****************************************************
 * decelerateShip
 *  stops a ship, via the mystical reverse.
 ***************************************************/
void decelerateShip(Spaceship * player, float movementScaleFactor){
	float acceleration = 0.5; //factor by which to decrease the direction (velocity) vector
	
	if( fabs(player->direction.x) > 0.1 || fabs(player->direction.y) > 0.1 ){
		acceleration *= movementScaleFactor;
		if(player->decelerating == true){
			player->direction.x *= 1 - acceleration*(player->strength/ (float) player->maxStrength);
			player->direction.y *= 1 - acceleration*(player->strength/ (float) player->maxStrength);
		}
		if(player->playerNumber > 0)
			playerObjects[P(player->playerNumber)].score -= (int) floor( (24 + levelNumber)*movementScaleFactor);
	}
	else{ //the ship is barely moving, so just stop it.
		player->direction.x = 0;
		player->direction.y = 0;
	}
}


/****************************************************
 * rotateShip
 *  rotates a ship based on the value of its rotating flag
 ***************************************************/
void rotateShip(Spaceship * player, float movementScaleFactor){
	float rotationValue; //how many degrees to rotate the ship
	//Standard is 10 degrees, defined in Allenroids.h
	
	rotationValue = movementScaleFactor*ROTATION_ANGLE_PER_FRAME;
	if(player->rotating != NOT_ROTATING){
		if(player->rotationScaleFactor < 1 + levelNumber/30.0){ //to start the ship rotataing slowly for precision guidance
			//slightly increase it as level number increases, so that it can turn faster
			player->rotationScaleFactor = player->rotationScaleFactor+0.20*movementScaleFactor;
		}
		if(player->rotating == COUNTERCLOCKWISE) //counterclockwise
			player->rotationAngle += rotationValue*player->rotationScaleFactor;
		else //clockwise
			player->rotationAngle -= rotationValue*player->rotationScaleFactor;
			
		player->rotationAngle = fmod(player->rotationAngle, 360);
	}
}

/****************************************************
 * impactShip
 *  registers an impact; blows up a ship
 *  and slates it for rebuilding if necessary
 ***************************************************/
void impactShip(Spaceship *ship, HitData impactData){
	int playerNumber = ship->playerNumber;
	
	if(ship->invinciblityTimeToLive <= 0){ //that is, not incinvible
		ship->strength -= impactData.hitCode; //hitCode holds the strength of the impact
		if(ship->strength < 0)
			ship->strength = 0;
		if(impactData.hitCode != 0){
			playSound(soundBuffer, SHIELDS_SOUND);
			ship->drawShieldsTimeToLive = 0 + FADEOUTS; //only draw a bubble for the fadeout time
		}
		if(ship->strength <= 0){
			if(playerNumber > 0){ //this code is specific to actual players, not to *potential* enemies. Enemies have numbers less than 0
				//disable player if shields are gone
				playerObjects[P(playerNumber)].setEnabled(false);
				playerObjects[P(playerNumber)].lives--;
				if(playerObjects[P(playerNumber)].lives < 0){
					numberOfPlayersActive--;
					playerObjects[P(playerNumber)].highestLevelAttained = levelNumber;
					if(numberOfPlayersActive <= 0)
						glutTimerFunc(3000, endGame, 0); //end the game after 3 seconds; after all; no one is playing!
				}
				else{
					glutTimerFunc(0, resurrectShip, playerNumber); //rebuild after 3 seconds
				}
			}
			spawnKerblooie(
				KERBLOOIE_HUGE,
				ship->radius/2,
				ship->color,
				ship->position,
				ship->direction, //which has already been collided at the top of the function
				&kerblooieList );
		}
	}
	
}


/****************************************************
 * resurrectShip
 *  rebuilds a spaceship; always called by a timer function!
 ***************************************************/
void resurrectShip(int playerNumber){
	static boolean firstCall = true; //this is only initialized upon variable creation
	if(firstCall == true){
		glutTimerFunc(3000, resurrectShip, playerNumber); //rebuild after 3 seconds
		firstCall = false;
		//Why this is necessary:
		//For some reason, calling glutTimerFunc (within the process of the idle function?) causes it to only time the first call. Subsequent calls execute immediately. Hence the "double call" here.
		//Inexplicably, the second timer call works just fine.
	}
	else{
		playerObjects[P(playerNumber)].spawnSpaceship();
		playerObjects[P(playerNumber)].setEnabled(true);
		firstCall = true;
	}
}



/****************************************************
 * spawnStarfield
 *  creates a pretty starfield for the background
 ***************************************************/
void spawnStarfield()
{
	starfield.SetStarCount(NUMBERSTARS);
}


/****************************************************
 * summonBadGuy
 *  creates an opponent spaceship
 *	different from other spawn methods,
 *	in that it adds the alien into the list 'badGuys'
 ***************************************************/
void summonBadGuy(int smartinessValue) {
	static boolean firstCall = true;
	if(firstCall == true){
		glutTimerFunc(2000, summonBadGuy, smartinessValue);
		firstCall = false;
	}
	else{
		BadGuy *gremlin = new BadGuy; //"gremlin" because he's not nice--yeah...
		boolean alongX;
		
		gremlin->playerNumber = -1;
		gremlin->radius = 20;
		gremlin->mass = 16;
		gremlin->color[0] = badGuyColors[0];
		gremlin->color[1] = badGuyColors[1];
		gremlin->color[2] = badGuyColors[2];
	
		gremlin->rotationAngle = rand()%360;
		
		//Create the position somewhere along the edge of the screen
		alongX = rand()%2; //true or false
		if(alongX == true){
			gremlin->position.x = rand() % FRAMEWIDTH; //create a point somewhere along x
			gremlin->position.y = rand()%2 * FRAMEHEIGHT; //put it either on the top of the screen or the bottom
		}
		else{ //along y
			gremlin->position.x = rand()%2 * FRAMEWIDTH; //create a point somewhere along x
			gremlin->position.y = rand() % FRAMEHEIGHT; //put it either on the top of the screen or the bottom
		}
		
		gremlin->direction.x = 0; //Start out at a "stop," but give it a random direction and thrust time
		gremlin->direction.y = 0;
		gremlin->invinciblityTimeToLive = 0;
		gremlin->drawShieldsTimeToLive = 0;
		gremlin->strength = 4 + smartinessValue/4;
		gremlin->maxStrength = 4 + smartinessValue/4;
		gremlin->thrusting = true; //start it out moving!
		gremlin->decelerating = false; //no mystical reverse
		gremlin->drawThrustUntil = 0;
		gremlin->rotating = NOT_ROTATING;
		
		//Special Bad Guy Code:
		gremlin->smartiness = smartinessValue;
		gremlin->timeToNextUpdate = rand()%500;
		gremlin->rotationTimeToLive = 0;
		gremlin->thrustTimeToLive = rand()%500;
		
		badGuyList.insertObject(gremlin);
		firstCall = true;
	}
	//return gremlin;
}


/****************************************************
 * spawnAsteroid
 *  creates an asteroid of specified size moving in a random direction
 ***************************************************/
Asteroid *spawnAsteroid(int sizeIn, float positionX, float positionY, Vector hint) {
	Asteroid *asteroid;
	int asteroidAngle;
	float magnitude;
	
	asteroid = new Asteroid(sizeIn, positionX, positionY);
	
	asteroidAngle = rand()%360;
	//the Magnitude: a Random Number between 0 and 1, multiplied by a speed for the level
	
	if(sizeIn == ASTEROID_BIG){
		do{
			magnitude = (1+levelNumber/5)*((float) rand()/RAND_MAX + abs(sizeIn/sizeof(Asteroid::asteroidSizes)/sizeof(int)) - 1.5 );
		}while(magnitude == 0); //an asteroid must be moving!
	}
	else
		magnitude = magnitudeOfVector(hint) *(1 + fmod((float) rand()/RAND_MAX, 0.25) ); //because smaller rocks should go a little faster, we bump up the speed from the hint
	asteroid->direction.x = magnitude * cos(asteroidAngle) + hint.x;
	asteroid->direction.y = magnitude * sin(asteroidAngle) + hint.y;
	
	return asteroid;
}


/****************************************************
 * spawnKerblooie
 *  creates an explosion of specified KERBLOOIE_SIZE at specified point,
 *  with specified directional hint
 *	put particles in specified list
 ***************************************************/
void spawnKerblooie(int sizeIn, float radius, GLubyte *colorHint, Coordinate position, Vector hint, LinkedList<GameObject> *list) {
	int counter;
	int kerblooieSize;
	Coordinate particlePosition;
	Vector particleDirection;
	float particleAngle; //prior to version 1.1.1, this was an int not a float. Consequently, explosions started out as neat little outward-thrusting spirals (like a camera lens). Now they are more like hypnotic disk spirals, and are undetectible to the naked eye.
	float magnitude;
	int particleLifespan;
	float particleRadius;

	
	switch(sizeIn){
		case KERBLOOIE_HUGE:
			playSound(soundBuffer, EXPLOSION_HUGE_SOUND);
			break;
		case KERBLOOIE_BIG:
			playSound(soundBuffer, EXPLOSION_LARGE_SOUND);
			break;
		case KERBLOOIE_MEDIUM:
			playSound(soundBuffer, EXPLOSION_MEDIUM_SOUND);
			break;
		case KERBLOOIE_SMALL:
			playSound(soundBuffer, EXPLOSION_SMALL_SOUND);
			break;
		case KERBLOOIE_TINY:
			break;
	}
	kerblooieSize = Kerblooie::kerblooieSizes[sizeIn];
	
	for(counter = 0; counter < kerblooieSize; counter++){
		particleAngle = counter*4*2*PI/kerblooieSize;
		//Basic position function: r = kerblooieRadius*(theta/numberSpirals*2*PI), 0 <= theta <= numberSpirals*2*PI
		//Converted to cartesian ( x = r*cos(theta) ):
		particlePosition.x = radius*particleAngle/(4*2*PI)*cos(particleAngle) + position.x;
		particlePosition.y = radius*particleAngle/(4*2*PI)*sin(particleAngle) + position.y;

		//the Magnitude: a Random Number between 0 and 1, multiplied by a speed. 16 is the max speed; only achieveable on the biggest explosion
		do{
			magnitude = (float) rand()/RAND_MAX * ((float) sizeIn/KERBLOOIE_HUGE +1); //cast random to float beforehand; Integer/(float) Integer does not do a float division.
			//printf("Magnitude: %f; Rand: %d; RandMax = %d\n", magnitude, random, RAND_MAX);
		}while(magnitude == 0); //an particle must be moving!
		
		particleDirection.x = magnitude * cos(particleAngle) + hint.x;
		particleDirection.y = magnitude * sin(particleAngle) + hint.y;

		particleLifespan = (int) (
				(rand()%2000 + 250) * ((float) sizeIn/KERBLOOIE_HUGE +1)
											); // (0.25 < t < 2 seconds)*explosionSize
		particleRadius = (float) rand()/RAND_MAX * ( rand()%(sizeIn+1) ) + 2.25;
		
		Kerblooie *particle = new Kerblooie(particlePosition, particleDirection, particleLifespan, particleRadius, colorHint);
				
		list->insertObject(particle);
	}
}


/****************************************************
 * spawnCrystal
 *  creates a crystal of specified CRYSTAL_SPECIMEN at specified point
 ***************************************************/
void spawnCrystal(int specimen, Coordinate position, LinkedList<GameObject> *list) {
	Crystal *newCrystal;
	int crystalAngle; //the direction at which it goes zooming off
	float magnitude;
	
	newCrystal = new Crystal;
	playSound(soundBuffer, RELEASE_CRYSTAL_SOUND);
	newCrystal->position = position;
	newCrystal->mass = 2;
	
	crystalAngle = rand()%360;
	//the Magnitude: level Number plus a random adjustment.
	do{
		magnitude = levelNumber/4.0;
		//printf("Magnitude: %f; Rand: %d; RandMax = %d\n", magnitude, random, RAND_MAX);
	}while(magnitude == 0); //a crystal must be moving!
	
	newCrystal->direction.x = magnitude * cos(crystalAngle);
	newCrystal->direction.y = magnitude * sin(crystalAngle);
	
	newCrystal->crystalFlavor = specimen;
	newCrystal->timeToLive = 10000; //10 seconds
	newCrystal->rotationIncrement = (float) rand()/RAND_MAX + 30/FRAMES_PER_SECOND;
	if(specimen == CRYSTAL_SHIELDPOWER){
		newCrystal->radius = 8;
		newCrystal->thickness = 1;
	//  newCrystal->radius = 128;
	//  newCrystal->thickness = 16;
		newCrystal->color[0] = 0;
		newCrystal->color[1] = 128;
		newCrystal->color[2] = 128;
		newCrystal->color[3] = 192;
	}
	newCrystal->status.hitCode = CRYSTAL_NORMAL;
	newCrystal->status.mass = newCrystal->mass;
	newCrystal->status.direction = newCrystal->direction;
	
	list->insertObject(newCrystal);
}


/****************************************************
 * startLevel
 *  initiates a level
 ***************************************************/
void startLevel(int level) {
	static boolean firstCall = true; //this is only initialized upon variable creation
	unsigned int numberUnsmallAsteroids = 0;
	unsigned int totalNumberLargeAsteroids; //this will be the initial number of asteroids on the screen
	Vector zeroVector;
	unsigned int numberCrystalsToRelease[CRYSTAL_NUMBER_OF_SPECIMENS] = {0}; //Initialize to zero
	unsigned int numberBadGuys; //how many will haunt us this level
	int partitionWidth;
	unsigned int counter, scheduleCounter;
	
	#ifdef RANDOM_ON
		srand( time(NULL) );
	#endif
	zeroVector.x = 0;
	zeroVector.y = 0;
	
	if(firstCall == true && startingNewLevel == true){ //don't do this silly jerry-rig unless the method was actually called during gameplay
		glutTimerFunc(4000, startLevel, level); //rebuild after 4 seconds
		firstCall = false;
		//Why this is necessary:
		//For some reason, calling glutTimerFunc (within the process of the idle function?) causes it to only time the first call. Subsequent calls execute immediately. Hence the "double call" here.
		//Inexplicably, the second timer call works just fine.
	}
	else{
		//the actual startLevel code!

		//reset players to default positions
		for(counter = 1; counter <= numberOfPlayers; counter++){
			if(playerObjects[P(counter)].isEnabled() == true){
				playerObjects[P(counter)].score += 48 * (levelNumber); //the global level number is still the number of the old level just ended
				//stop the spaceship
				playerObjects[P(counter)].spaceship.rotationAngle = 0;
				playerObjects[P(counter)].spaceship.position.x = FRAMEWIDTH  * counter/(numberOfPlayers + 1);
				playerObjects[P(counter)].spaceship.position.y = FRAMEHEIGHT * counter/(numberOfPlayers + 1);
				playerObjects[P(counter)].spaceship.direction.x = 0;
				playerObjects[P(counter)].spaceship.direction.y = 0;
				playerObjects[P(counter)].spaceship.invinciblityTimeToLive = INVINCIBLE_LENGTH;
				playerObjects[P(counter)].spaceship.drawShieldsTimeToLive = playerObjects[P(counter)].spaceship.invinciblityTimeToLive + FADEOUTS/2;
				playerObjects[P(counter)].spaceship.thrusting = 0;
			}
		
		}
		
		//Wipe lists
		shotList.clearList();
		crystalList.clearList();
		badGuyList.clearList();
		
		//Change background
		spawnStarfield();
		
		//create Asteroids and asteroid schedule
		/*
		 *	Only unsmall asteroids (big and medium) are "scheduled"--that is,
		 *	there is a preordained total number of unsmall asteroids that can
		 *	be produced, and the order in which they appear is also preordained.
		 *	(Small asteroids are left to chance.) This system is used to more evenly
		 *	distribute crystals throughout a level.
		 */
		totalNumberLargeAsteroids = level+2;
		numberUnsmallAsteroids = totalNumberLargeAsteroids; //start out with at least that many unsmalls
		free(asteroidReleaseSchedule);
		asteroidReleaseSchedule = (int *)calloc(totalNumberLargeAsteroids, sizeof(int) ); //will hold the number of smaller asteroids to be produced from each *large* one
		for(counter = 0; counter < totalNumberLargeAsteroids; counter++){ //spawn large asteroids and ordain their number of divisions
			Asteroid *newAsteroid = spawnAsteroid(ASTEROID_BIG,rand() % FRAMEWIDTH, rand() % FRAMEHEIGHT , zeroVector);
			asteroidList.insertObject(newAsteroid);
			asteroidReleaseSchedule[counter] = rand()%2+2; //either 2 or three new asteroids to be released
			numberUnsmallAsteroids += asteroidReleaseSchedule[counter]; //the number of unsmalls has now been increased by the random number just generated
		}
		unsmallAsteroidsDestroyedToDate = 0;
		bigAsteroidsDestroyedToDate = 0; //keep track of these as the array index for the schedule!
		//printf("\n\n\nUnsmall: %d\n", numberUnsmallAsteroids);
		
		//create Crystal schedule
		#ifdef DEBUG_CRYSTALS
			printf("\n\n");
		#endif
		numberCrystalsToRelease[CRYSTAL_SHIELDPOWER] = level*3/4 + ( rand()%level )/2;
		
		for(counter = 0; counter < CRYSTAL_NUMBER_OF_SPECIMENS; counter++){
			free(crystalReleaseSchedule[counter]);
			crystalReleaseSchedule[counter] = (unsigned int *)calloc(numberCrystalsToRelease[counter],sizeof(int));
			for(scheduleCounter = 0; scheduleCounter < numberCrystalsToRelease[counter]; scheduleCounter++){
				do{
					crystalReleaseSchedule[counter][scheduleCounter] = rand()%numberUnsmallAsteroids; //try a random number somewhere amongst the unsmall explosions
				}while( scheduleCounter * numberUnsmallAsteroids / numberCrystalsToRelease[CRYSTAL_SHIELDPOWER] >= crystalReleaseSchedule[counter][scheduleCounter] || //these two lines specify the bracket conditions (lower and upper), which are vital to ensuring the crystals are *evenly* distributed
						crystalReleaseSchedule[counter][scheduleCounter] > (scheduleCounter+1)*numberUnsmallAsteroids/numberCrystalsToRelease[CRYSTAL_SHIELDPOWER] //value must be within the desired bracket
						|| (
							(scheduleCounter > 0) ? //conditional operator: if(scheduleCounter>0) then (blah) else true
								crystalReleaseSchedule[counter][scheduleCounter] <= crystalReleaseSchedule[counter][scheduleCounter-1] //make sure that we *keep looping* if the computer hasn't generated a number greater than the previous one it came up with
							:
								false //this only happens if we are still on the very first scheduled crystal, on which we don't need to consider the previous line
							)
					); //loop while conditions are wrong
				if(numberCrystalsToRelease[counter] == 0)
					//crystalReleaseSchedule[counter][scheduleCounter] = -1; //when there are no crystals to create, one schedule slot gets calloced and filled with 0 anyway. Fill it with a clearly invalid number instead.
					crystalReleaseSchedule[counter][scheduleCounter] = 0; //when there are no crystals to create, one schedule slot gets calloced and filled with 0 anyway. Fill it with a clearly invalid number instead.
				#ifdef DEBUG_CRYSTALS
					printf("%d, ",crystalReleaseSchedule[counter][scheduleCounter]);
				#endif
			}
			crystalsReleased[counter] = 0;
			#ifdef DEBUG_CRYSTALS
				printf("\nCyrstals (%d) to be released: %d\n", counter, numberCrystalsToRelease[counter]);
			#endif
		}
		
		
		/* Bad Guy Scheduling
		 *	Don't make a lot of them!
		 *	They are scheduled by the index of an unsmall asteroid destruction
		 */
		free(badGuySchedule);
		if(level > 1)
			numberBadGuys = level/5 + rand()%2;
		else
			numberBadGuys = 0; //first level is easy
		if(numberBadGuys > 0){
			partitionWidth = (int) ceil( (float)numberUnsmallAsteroids/numberBadGuys);
			badGuySchedule = (int*)calloc(numberBadGuys, sizeof(int) );
			for(scheduleCounter = 0; scheduleCounter < numberBadGuys; scheduleCounter++){
				badGuySchedule[scheduleCounter] = 1 + rand()%partitionWidth + scheduleCounter*partitionWidth;
				//printf("%d,", badGuySchedule[scheduleCounter]);
			}
		}
		else{
			badGuySchedule = (int*)calloc(1, sizeof(int) );
			badGuySchedule[0] = -1; //bogus value prevents it from ever being considered
		}
		badGuysSummonedToDate = 0;
		
		//Finally, get moving!
 		startingNewLevel = false; //the new level has begun
		levelNumber = level;

		firstCall = true;
	}


}


/****************************************************
 * endGame
 ***************************************************/
void endGame(int needlessParameter) {
	static boolean firstCall = true; //this is only initialized upon variable creation
	if(firstCall == true){
		glutTimerFunc(3000, endGame, needlessParameter); //rebuild after 3 seconds
		firstCall = false;
		//Why this is necessary:
		//For some reason, calling glutTimerFunc (within the process of the idle function?) causes it to only time the first call. Subsequent calls execute immediately. Hence the "double call" here.
		//Inexplicably, the second timer call works just fine.
	}
	else{
		//the actual endGame code!
		paused = true; //stops timer for timer-idle drawing.
		titleInit();
		newHighScore(1); //create highscore for player1. Player2's is called afterwards.
	
		glutPostRedisplay();
		firstCall = true;
	}
}

/****************************************************
 * handleKeyboard
 *  accepts regular keyboard input
 ***************************************************/
void handleKeyboard(unsigned char keypressed, int x, int y){
	handleKey(keypressed);
}

/****************************************************
 * handleSpecialKeyboard
 *  accepts special keys, i.e. arrows, etc.
 ***************************************************/
void handleSpecialKeyboard(int keypressed, int x, int y){
	handleKey(-keypressed); //by convention in my program, special keys are negative
}


/****************************************************
 * handleKey
 *  accepts regular keyboard input
 ***************************************************/
void handleKey(int keypressed){
	unsigned int counter;
	
	//Cheat Codes!
	if(glutGetModifiers() == GLUT_ACTIVE_ALT){
		switch(keypressed){
			case -GLUT_KEY_F1:
				//playerObjects[P(1)].lives++;
				levelNumber++;
				break;
			case -GLUT_KEY_F2:
				playerObjects[P(2)].lives++;
				break;
			case -GLUT_KEY_F5:	
				levelNumber++;
				break;
			case -GLUT_KEY_F6:	
				levelNumber--;
				break;
			
		} //end switch
	}//end if

	//pause on escape keypress
	if(keypressed == 27 && paused == false){
		glutSpecialFunc(NULL);
		glutIdleFunc(NULL);
		
		paused = true;
		glutPostRedisplay();
	}
	else if(keypressed == 27 && paused == true){ //*unpause only* on escape key
		glutKeyboardUpFunc(handleKeyboardUp);
		glutSpecialFunc(handleSpecialKeyboard);
		glutSpecialUpFunc(handleSpecialKeyboardUp);
#ifdef TIGHT_IDLE_DRAWING
		glutIdleFunc(idle);
#else
		glutTimerFunc(0, idleByTimer, 0);
#endif
		
		paused = false;
	}
	else if(paused == false){ //don't do key actions if paused
		for(counter = 1; counter <= numberOfPlayers; counter++){
	
			if(playerObjects[P(counter)].isEnabled() == true){
				if( FIRE(P(counter)) == keypressed )
					fireShot(&playerObjects[P(counter)].spaceship);
				else if( ROTATE_CCW(P(counter)) == keypressed ){
					playerObjects[P(counter)].spaceship.rotating = COUNTERCLOCKWISE;
					playerObjects[P(counter)].spaceship.rotationScaleFactor = 0; //this value is incremented up to one as the ship continues to rotate. This allows precision alignment at first.					
				}
				else if( ROTATE_CW(P(counter)) == keypressed){
					playerObjects[P(counter)].spaceship.rotating = CLOCKWISE;
					playerObjects[P(counter)].spaceship.rotationScaleFactor = 0;					
				}
				else if( THRUST(P(counter)) == keypressed )
					playerObjects[P(counter)].spaceship.thrusting = true;
				else if( REVERSE(P(counter)) == keypressed )
					playerObjects[P(counter)].spaceship.decelerating = true;
			}
		}//end for
	}
}


/****************************************************
 * handleKeyboardUp
 *  accepts regular keyboard input
 ***************************************************/
void handleKeyboardUp(unsigned char keypressed, int x, int y){
	handleKeyUp(keypressed);
}

/****************************************************
 * handleSpecialKeyboardUp
 *  accepts special keys, i.e. arrows, etc.
 ***************************************************/
void handleSpecialKeyboardUp(int keypressed, int x, int y){
	handleKeyUp(-keypressed); //by convention in my program, special keys are negative
}


/****************************************************
 * handleKeyUp
 *  when a key is released
 ***************************************************/
void handleKeyUp(int keyreleased){
	unsigned int counter;
	
	for(counter = 1; counter <= numberOfPlayers; counter++){

		if(playerObjects[P(counter)].isEnabled() == true){
			if( ROTATE_CCW(P(counter)) == keyreleased)
				playerObjects[P(counter)].spaceship.rotating = NOT_ROTATING;
			else if( ROTATE_CW(P(counter)) == keyreleased)
				playerObjects[P(counter)].spaceship.rotating = NOT_ROTATING;
			else if( THRUST(P(counter)) == keyreleased){
				playerObjects[P(counter)].spaceship.thrusting = false;
				playerObjects[P(counter)].spaceship.drawThrustUntil = glutGet( GLUT_ELAPSED_TIME ) + FADEOUTS;
				playSound(soundBuffer, STOP_THRUSTING_SOUND);
			}
			else if( REVERSE(P(counter)) == keyreleased )
				playerObjects[P(counter)].spaceship.decelerating = false;
		}
	
	}//end for
}
