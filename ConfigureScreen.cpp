/*
 *  Configure Screen.c
 *  Asteroids
 *
 *  Created by Allen Smith on Mon Oct 26 2002.
 *
 */

#include "Allenroids.h"
#include "TitleScreen.h"
#include "ConfigureScreen.h"

/*****Global Variables*****/
extern GLubyte playerColors[][3];

MyButton configureButtons[MAX_NUMBER_PLAYERS];
MyButton playerButtons[MAX_NUMBER_PLAYERS][NUMBER_ACTIONS];
int buttonWidth;
int buttonHeight;

//Players
PlayerKeys playerKeys[MAX_NUMBER_PLAYERS];
	
int activeButton = -1; //identifies the button that is being changed at the moment. -1 if not active.
int buttonBeingPressed = -1; //the button upon which the mouse is being held down on. -1 if not active.


/****************************************************
 * configureInit
 *  set up the configure screen
 ***************************************************/
void configureInit(void){
	unsigned int counter, playerCounter;
	float linePosition;
	float xPosition, yPosition;
	int actionKey;

	//glut Function setup
	glutKeyboardFunc(configureHandleKeyboard);
	glutSpecialFunc(configureHandleSpecialKeys);
	glutIgnoreKeyRepeat(true);
	glutMouseFunc(configureHandleMouse);
	glutDisplayFunc(configureDisplay);
	
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0,0,0, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glLoadIdentity();
	gluOrtho2D(0, FRAMEWIDTH, 0, FRAMEHEIGHT);
		
	activeButton = -1; //we aren't changing any keys yet
	buttonWidth = 128;
	buttonHeight = 24;
	
	linePosition = FRAMEHEIGHT*(float)3/8; //initial center position of first row of buttons
	
	//Create the buttons themselves, row by row for each action
	for(counter = 0; counter < NUMBER_ACTIONS; counter++){ //for loop creates buttons for player keys
		//Generate the button for the action for each player
		for(playerCounter = 1; playerCounter <= MAX_NUMBER_PLAYERS; playerCounter++){ //for loop creates buttons for player keys
			xPosition =   FRAMEWIDTH*(1/4.0)//first, we reserve 1/4 screen for the button labels
						+ playerCounter * FRAMEWIDTH*((float)3/4/MAX_NUMBER_PLAYERS) //now we divide the remaining 3/4 screen by the number of players, and shift that much more
						- FRAMEWIDTH*((float)3/4/MAX_NUMBER_PLAYERS)/2; //now center ourselves within the block
			yPosition = FRAMEHEIGHT - linePosition;
			
			//map the button index to the action it represents.
			switch(counter){
				case 0:	actionKey = playerKeys[P(playerCounter)].thrust;					break;
				case 1:	actionKey = playerKeys[P(playerCounter)].rotateCounterClockwise;	break;
				case 2:	actionKey = playerKeys[P(playerCounter)].rotateClockwise;			break;
				case 3:	actionKey = playerKeys[P(playerCounter)].fire;						break;
				case 4:	actionKey = playerKeys[P(playerCounter)].reverse;					break;
			}
			sprintf(playerButtons[P(playerCounter)][counter].text, "%s", convertASCIItoText(actionKey) );
			
			playerButtons[P(playerCounter)][counter].position.x = xPosition;
			playerButtons[P(playerCounter)][counter].position.y = yPosition;
				playerButtons[P(playerCounter)][counter].color[0] = playerColors[P(playerCounter)][0];
				playerButtons[P(playerCounter)][counter].color[1] = playerColors[P(playerCounter)][1];
				playerButtons[P(playerCounter)][counter].color[2] = playerColors[P(playerCounter)][2];
				playerButtons[P(playerCounter)][counter].color[3] = 128; //50%
		}
		linePosition += buttonHeight + buttonHeight/2; //ready to advance to the next row of buttons
	}
	
	//The control buttons
	
	sprintf(configureButtons[0].text, "Cancel");
	configureButtons[0].position.x = FRAMEWIDTH*1/3;
	configureButtons[0].position.y = FRAMEHEIGHT*1/8;
		configureButtons[0].color[0] = 255;
		configureButtons[0].color[1] = 0;
		configureButtons[0].color[2] = 0;
		configureButtons[0].color[3] = 128; //50%
	
	sprintf(configureButtons[1].text, "OK");
	configureButtons[1].position.x = FRAMEWIDTH*2/3;
	configureButtons[1].position.y = FRAMEHEIGHT*1/8;
		configureButtons[1].color[0] = 0;
		configureButtons[1].color[1] = 255;
		configureButtons[1].color[2] = 0;
		configureButtons[1].color[3] = 128; //50%
	
	glutPostRedisplay();
}

/****************************************************
 * configureDisplay
 *  paints all the configure elements
 ***************************************************/
void configureDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawStarfield(); //in asteroids.c
	configurePrintTitle();
	configurePrintText();
	buttonHeight = 24;
	configurePrintButtons(playerButtons[P(1)], NUMBER_ACTIONS, GL_RENDER, 100);
	configurePrintButtons(playerButtons[P(2)], NUMBER_ACTIONS, GL_RENDER, 200);
	buttonHeight = 32; //control buttons are bigger!
	configurePrintButtons(configureButtons, 2, GL_RENDER, 0);
	glutSwapBuffers();
	
}


/****************************************************
 * configurePrintTitle
 *  writes the word "CONFIGURE KEYS" in large letters at the top
 ***************************************************/
void configurePrintTitle() {
	char gameTitle[] = "CONFIGURE KEYS";
	unsigned int counter;
	int stringWidth = 0;
	float scaleFactor = 0.35; //35%
	
	glColor3ub(255,255,255);
	glLineWidth(4.0);
	//DO NOT LOAD A MATRIXMODE! It totally messes up the coordinate system.
	glPushMatrix();
	for(counter = 0; counter < strlen(gameTitle); counter++){
		stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , gameTitle[counter]);
	}
	glTranslatef( FRAMEWIDTH/2 - (stringWidth/2)*scaleFactor, FRAMEHEIGHT-64, 0);
	glScalef(scaleFactor, scaleFactor, 0);
	glRasterPos2i(0,0);
	for(counter = 0; counter < strlen(gameTitle); counter++){
		glutStrokeCharacter( GLUT_STROKE_ROMAN , gameTitle[counter]);
	}
	
	glPopMatrix();
}


/****************************************************
 * configurePrintText
 *  prints all labels for buttons and explanatory text
 ***************************************************/
void configurePrintText() {
	char            mainTitle[64];
	unsigned		stringLength		= 0;
	float           stringWidth         = 0;
	char            columnHeadings[32];
	char            labelString[64];
	char            *actionNames[]      = {	"Thrust",
											"Rotate CCW (Left)",
											"Rotate CW (Right)",
											"Fire",
											"Mystical Reverse*"};
	char            *actionName         = NULL;
	float           linePosition; //in pixels; the distance from the top of the screen to render the current line of text.
	unsigned int    playerCounter;
	unsigned int    counter;
	int             scoreIndex; //counter for highScore array
	float           scaleFactor         = 0.125; // 3/16ths
	
	sprintf(mainTitle, "Click a box and type the new key to change that control.");

	//--------------------------------------------------------------------------
	// Instructions
	
	glColor3ub(255,255,255);
	glLineWidth(1.0);
	
	//DO NOT LOAD A MATRIXMODE! It totally messes up the coordinate system.
	glPushMatrix();
	{
		// Find string width
		for(counter = 0; counter < strlen(mainTitle); counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , mainTitle[counter]);
		}
		stringWidth *= scaleFactor;
		
		linePosition = 108;
		//print out the words "Click a box to change that key" at the top
		glTranslatef( FRAMEWIDTH/2 - stringWidth/2, FRAMEHEIGHT - linePosition, 0);
		glScalef(scaleFactor, scaleFactor, 0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < strlen(mainTitle); counter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , mainTitle[counter]);
		}
	}
	glPopMatrix();
	
	
	//--------------------------------------------------------------------------
	// Print Column Headings for Players
	
	linePosition += 48; //now it equals 156
	for(playerCounter = 1; playerCounter <= MAX_NUMBER_PLAYERS; playerCounter++){
		glPushMatrix();
		glColor3ubv(playerColors[P(playerCounter)]);
		glLineWidth(2.0);
		sprintf(columnHeadings, "Player %d", playerCounter);
		
		// Find width of string
		stringLength = strlen(columnHeadings);
		stringWidth  = 0;
		for(counter = 0; counter < stringLength; counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , columnHeadings[counter]); //since this is a variable-width font
		}
		stringWidth *= scaleFactor;
		
		//Draw the headings at:
		//shifted right by 1/4 screen, then divided up by the remaining distance
		glTranslatef(     FRAMEWIDTH*(1/4.0)//first, we reserve 1/4 screen for the button labels
						+ playerCounter * FRAMEWIDTH*((float)3/4/MAX_NUMBER_PLAYERS) //now we divide the remaining 3/4 screen by the number of players, and shift that much more
						- FRAMEWIDTH*((float)3/4/MAX_NUMBER_PLAYERS)/2 //now center ourselves within the block
						- (stringWidth/2), //then we shift back by half the string length so the label is centered
						
						FRAMEHEIGHT - linePosition, //y-shift
						0 //z-shift
					);
		glScalef(scaleFactor, scaleFactor, 0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < stringLength; counter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , columnHeadings[counter]);
		}
		glPopMatrix();
	}
	
	
	//--------------------------------------------------------------------------
	// Print the labels for the buttons
	
	buttonHeight = 24; //the height of a small button
	linePosition = FRAMEHEIGHT*(float)3/8 + buttonHeight/4; //initial line position
	
	glColor3ub(255,255,255);
	glLineWidth(1.0);
	scaleFactor = 0.09;
	
	for(scoreIndex = 0; scoreIndex < NUMBER_ACTIONS; scoreIndex++)
	{
		actionName = actionNames[scoreIndex];
		
		// Find string width
		stringLength = strlen(actionName);
		stringWidth = 0;
		for(counter = 0; counter < stringLength; counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , actionName[counter]);
		}
		stringWidth *= scaleFactor;
		
		// Draw string
		glPushMatrix();
		{
			// Right-justify the text
			glTranslatef(FRAMEWIDTH*(1/4.) - stringWidth, FRAMEHEIGHT - linePosition, 0);
			glScalef(scaleFactor, scaleFactor, 0);
			
			glRasterPos2i(0,0);
			for(counter = 0; counter < stringLength; counter++){
				glutStrokeCharacter( GLUT_STROKE_ROMAN , actionName[counter]);
			}
			
		}
		glPopMatrix();
		
		linePosition += buttonHeight + buttonHeight/2; //increment for button spacing height
	}
	
	
	//--------------------------------------------------------------------------
	// Print warning label about mystical reverse drive
	
	glColor3ub(72,72,72);
		sprintf(labelString,  "*Nobody seems to know how this works, and it's best not to ask.");
		glRasterPos2f( (FRAMEWIDTH*(float) 1/16), (FRAMEHEIGHT * (float) 1/32) );
		for(counter = 0; counter < strlen(labelString); counter++){
			glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , labelString[counter]);
		}
}


/****************************************************
 * configurePrintButtons
 *  displays the buttons at their positions
 ***************************************************/
void configurePrintButtons(MyButton *buttonList, unsigned int numberOfButtons, GLenum mode, int listNumberModifier) {
	//the listNumberModifier helps indentify different lists of buttons to the feedback mechanism
	//	i.e., player2 buttons should be >200, player1's >100, neutral buttons >0
	float stringWidth;
	unsigned int counter, letterCounter;
	float scaleFactor = 0.09;

	for(counter = 0; counter < numberOfButtons; counter++){
		glPushMatrix();
		glTranslatef(buttonList[counter].position.x, buttonList[counter].position.y, 0);

		//print button name
			//By placing this call first, the white text will be tinted by the translucent buttons
		glColor3ub(255,255,255);
		if(mode == GL_SELECT)
			glLoadName(counter + listNumberModifier);
		
/**
		//Bitmap font
		stringWidth = strlen(buttonList[counter].text);
		//Now change stringWidth to mean the width of our string in world coordinates
		stringWidth *= glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'A');
		glRasterPos2i( - stringWidth/2, - 13/4);
		for(letterCounter = 0; letterCounter < strlen(buttonList[counter].text); letterCounter++){
			glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , buttonList[counter].text[letterCounter]);
		}
***/
		
		
		//Proportional Font
		glPushMatrix();

		stringWidth = 0;
		for(letterCounter = 0; letterCounter < strlen(buttonList[counter].text); letterCounter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , buttonList[counter].text[letterCounter])*1.2;
		}
		//Now change stringWidth to mean the width of our string in world coordinates
		glScalef(scaleFactor, scaleFactor, 0);
		glTranslatef( - (stringWidth/2), - 119.05*3/8, 0);
		glLineWidth(1);
		for(letterCounter = 0; letterCounter < strlen(buttonList[counter].text); letterCounter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , buttonList[counter].text[letterCounter]);
			glTranslatef( glutStrokeWidth( GLUT_STROKE_ROMAN , buttonList[counter].text[letterCounter]) * 0.2,
							0, 0);
		}	
		glPopMatrix();

		//Background
		if(activeButton == (counter + listNumberModifier) ) //we are drawing the button of the key to be changed
			glColor4ub(128,0,64, 128); //Maroon with 50% opacity
		else if(buttonBeingPressed == (counter + listNumberModifier) ){ //we are drawing the button being clicked
			glColor4ub(	(GLubyte) (buttonList[counter].color[0] * 0.5), //darken by 50%
						(GLubyte) (buttonList[counter].color[1] * 0.5),
						(GLubyte) (buttonList[counter].color[2] * 0.5),
						           buttonList[counter].color[4]
					  );
		}
		else //just any old button
			glColor4ubv(buttonList[counter].color);
		
		glBegin(GL_POLYGON);
			glVertex2f( - buttonWidth/2,
						- buttonHeight/2);
						
			glVertex2f( + buttonWidth/2,
						- buttonHeight/2);
			
			glVertex2f( + buttonWidth/2,
						+ buttonHeight/2);
						
			glVertex2f( - buttonWidth/2,
						+ buttonHeight/2);
		glEnd();
		
		//Border
		glColor3ubv(buttonList[counter].color);
		glLineWidth(3.0);
		glBegin(GL_LINE_LOOP);
			glVertex2f( - buttonWidth/2,
						+ buttonHeight/2);
						
			glVertex2f( + buttonWidth/2,
						+ buttonHeight/2);
						
			glVertex2f( + buttonWidth/2,
						- buttonHeight/2);
						
			glVertex2f( - buttonWidth/2,
						- buttonHeight/2);
		glEnd();
		

		glPopMatrix();
		
	}//end for loop for buttons
}


/****************************************************
 * writeConfiguration
 *  writes key configuration to a file
 ***************************************************/
void writeConfiguration(){
	FILE *filePtr;
	unsigned int counter;
	PlayerKeys currentKeys;
	if(( filePtr = fopen("Key Configuration", "w") ) == NULL){
		//printf("The file could not be opened.");
	}
	else{
		for(counter = 0; counter < MAX_NUMBER_PLAYERS; counter++){
			//make a copy of the current score so we can safely flip bytes
			// if we need to without affecting the original data.
			currentKeys = playerKeys[counter];
			
			//On Macs, we always write the data big-endian. So if 
			// we're on an x86 Mac, we need to flip the bytes.
			#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
				flipBytes(&currentKeys.thrust,					sizeof(int));
				flipBytes(&currentKeys.rotateCounterClockwise,	sizeof(int));
				flipBytes(&currentKeys.rotateClockwise,			sizeof(int));
				flipBytes(&currentKeys.fire,					sizeof(int));
				flipBytes(&currentKeys.reverse,					sizeof(int));
			#endif
			fwrite( &currentKeys, sizeof(PlayerKeys), 1, filePtr);
		}
		fclose( filePtr );
	}
}

		
/****************************************************
 * readConfiguration
 *  reads the key preferences out of a data file.
 ***************************************************/
void readConfiguration(){
	FILE *filePtr;
	unsigned int counter = 0;
	if(( filePtr = fopen("Key Configuration", "r") ) == NULL){
		//printf("The file could not be opened for reading.");
		restoreDefaults();
	}
	else{
		for(counter = 0; counter < MAX_NUMBER_PLAYERS; counter++){
			if( !feof(filePtr) ){
				fread( &playerKeys[counter], sizeof(PlayerKeys), 1, filePtr);
				//On Macs, we always write the data big-endian. So if 
				// we're on an x86 Mac, we need to flip the bytes.
				#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
					flipBytes(&playerKeys[counter].thrust,					sizeof(int));
					flipBytes(&playerKeys[counter].rotateCounterClockwise,	sizeof(int));
					flipBytes(&playerKeys[counter].rotateClockwise,			sizeof(int));
					flipBytes(&playerKeys[counter].fire,					sizeof(int));
					flipBytes(&playerKeys[counter].reverse,					sizeof(int));
				#endif
			}
			else{
				//printf("The file is not of a valid format");
				restoreDefaults();
				readConfiguration();
			}
		}//end for
		fclose(filePtr);
	}//end else

}//end readConfiguration

/****************************************************
 * restoreDefaults
 *  returns all keys to their default settings if
 *	if reading a file fails.
 ***************************************************/
void restoreDefaults(){
	int counter, subcounter;
	
	//Set up player1's keys
	THRUST(		P(1) )= -GLUT_KEY_UP; //Up Arrow
	ROTATE_CCW(	P(1) )= -GLUT_KEY_LEFT; //Left Arrow
	ROTATE_CW(	P(1) )= -GLUT_KEY_RIGHT; //Right Arrow
	FIRE(		P(1) )= ' '; //Space
	REVERSE(	P(1) )= -GLUT_KEY_DOWN; //Down Arrow
	
	//Set up player2's keys
	THRUST(		P(2) )= '8';
	ROTATE_CCW(	P(2) )= '4';
	ROTATE_CW(	P(2) )= '6';
	FIRE(		P(2) )= '0';
	REVERSE(	P(2) )= '5';
	
	//In the rather unlikely event this game is ever extended to more than 2 players, this will handle that
	for(counter = 3; counter <= MAX_NUMBER_PLAYERS; counter++){
		for(subcounter = 0; subcounter < NUMBER_ACTIONS; subcounter++)
			memset( &playerKeys[P(counter)], '\0', sizeof(PlayerKeys) ); //null value is read as "unassigned" in convertASCIItoText
	}
	
	writeConfiguration();
}	


/****************************************************
 * convertASCIItoText
 *  accepts the values of a keypressed and coverts it
 *	to a nice pretty string.
 ***************************************************/
char* convertASCIItoText(int ascii){
	static char string[] = {0,'\0'}; //static because I'm lazy: I'm returning a local variable and I don't want it to get overwritten after the function exits!
	if(ascii >= 33 && ascii < 127){
		string[0] = ascii;
		return string;
	}
	else
		if(ascii >= 0){
			switch(ascii){
				case 0:
					return "Unassigned";
					break;
				case 3:
					return "Enter";
					break;
				case 8: //delete key (backspace)
					return "Delete";
					break;
				case 9:
					return "Tab";
					break;
				case 13:
					return "Return";
					break;
				case 32: // a space ' '
					return "Space";
					break;
			}
		}//end if for ascii
		
		else{ //a special character; by convention in my program a negative number
			switch(-ascii){
					
				case GLUT_KEY_UP:
					return "Up Arrow";
					break;
				case GLUT_KEY_DOWN:
					return "Down Arrow";
					break;
				case GLUT_KEY_LEFT:
					return "Left Arrow";
					break;
				case GLUT_KEY_RIGHT:
					return "Right Arrow";
					break;
					
				case GLUT_KEY_F1:
					return "F1";
					break;
				case GLUT_KEY_F2:
					return "F2";
					break;
				case GLUT_KEY_F3:
					return "F3";
					break;
				case GLUT_KEY_F4:
					return "F4";
					break;
				case GLUT_KEY_F5:
					return "F5";
					break;
				case GLUT_KEY_F6:
					return "F6";
					break;
				case GLUT_KEY_F7:
					return "F7";
					break;
				case GLUT_KEY_F8:
					return "F8";
					break;
				case GLUT_KEY_F9:
					return "F9";
					break;
				case GLUT_KEY_F10:
					return "F10";
					break;
				case GLUT_KEY_F11:
					return "F11";
					break;
				case GLUT_KEY_F12:
					return "F12";
					break;
				
				case GLUT_KEY_PAGE_DOWN:
					return "Page Down";
					break;
				case GLUT_KEY_HOME:
					return "Home";
					break;
				case GLUT_KEY_END:
					return "End";
					break;
				case GLUT_KEY_INSERT:
					return "Insert";
					break;
				
			}
		}
	return "Invalid"; //If all else fails
}


/****************************************************
 * configureHandleMouse
 *  accepts mouse input
 ***************************************************/
void configureHandleMouse(int button, int state, int x, int y){
	GLuint nameBuffer[512];
	int numberOfHits;
	GLint viewport[4];
	
	if(button == GLUT_LEFT_BUTTON){
		//view for selection mode
		glGetIntegerv(GL_VIEWPORT, viewport);
		glSelectBuffer(512, nameBuffer);
		glRenderMode(GL_SELECT);
		
		glInitNames();
		glPushName(0);
		
		//save original matrix
		glPushMatrix();		
		glLoadIdentity();
		//1x1 area around mouse is used for selection
		//viewport is stored with: x,y window coordinates, viewport height,width. So viewport[3] is the height
		gluPickMatrix( (double) x, (double)(viewport[3] - y), 2.0, 2.0, viewport);
			//gluOrtho2D(xmin, xmax, ymin, ymax); that is, the normal viewing area
		gluOrtho2D( 0, FRAMEWIDTH, 0, FRAMEHEIGHT );
		
		buttonHeight = 32;
		configurePrintButtons(configureButtons, 2, GL_SELECT, 0);
		buttonHeight = 24;
		configurePrintButtons(playerButtons[P(1)], NUMBER_ACTIONS, GL_SELECT, 100);
		configurePrintButtons(playerButtons[P(2)], NUMBER_ACTIONS, GL_SELECT, 200);
		//restore viewing matrix
		glPopMatrix();
		glutSwapBuffers();
		
		numberOfHits = glRenderMode(GL_RENDER); //normal render mode
		configureProcessHits(numberOfHits, nameBuffer, state);
		glutPostRedisplay();
	}
}


/****************************************************
 * configureProcessHits
 *  process a click on a button
 ***************************************************/
void configureProcessHits(unsigned int numberOfHits, GLuint buffer[], int mouseState){
	unsigned int counterHits, counterNames;
	GLuint names, *namesPtr;
	
	namesPtr = (GLuint *) buffer;
	
	if(numberOfHits == 0){
		activeButton = -1;
		buttonBeingPressed = -1;
	}
	else{ //we hit something
		if(mouseState == GLUT_DOWN){
			namesPtr += 3;
			buttonBeingPressed = (int) *namesPtr;
		}
		else{ //mouse released
			for(counterHits = 0; counterHits < numberOfHits; counterHits++){
				names = *namesPtr;
				namesPtr += 3;
				for(counterNames = 0; counterNames < names; counterNames++){
					if(buttonBeingPressed == *namesPtr){ //make sure we are releasing the mouse on the button we started with
						if(*namesPtr == 0){ //Cancel
							titleInit();
						}
						else if(*namesPtr == 1){ //OK
							writeConfiguration();
							titleInit(); //return to titleScreen
						}
						else{
							activeButton = *namesPtr;
							glutPostRedisplay();
						}
					}
					namesPtr++;
				}//end for names
			}//end for hits
			buttonBeingPressed = -1;
		}//end else for mouse released
	}//end numberHits > 0
}


/****************************************************
 * configureHandleKeyboard
 *  accepts regular keyboard input
 ***************************************************/
void configureHandleKeyboard(unsigned char keypressed, int x, int y){
	assignKey(keypressed);
}


/****************************************************
 * configureHandleSpecialKeys
 *  accepts regular keyboard input
 ***************************************************/
void configureHandleSpecialKeys(int keypressed, int x, int y){
	assignKey( -(keypressed) ); //note the MINUS SIGN! Special keys are denoted by negative numbers
}

/****************************************************
 * assignKey
 *  assigns a key to the action specified
 ***************************************************/
void assignKey(int keypressed){
	int playerNumber;
	int actionNumber;
	PlayerKeys *keys;
	if(activeButton != -1){
		playerNumber = activeButton/100; //essentially a div operation; finds the quotient via integer divison
		actionNumber = activeButton%100; //mod returns the actual index
		if(playerNumber > 0){
			keys = &playerKeys[P(playerNumber)];
			
			switch(actionNumber){
				case 0:	keys->thrust					= keypressed;	break;
				case 1:	keys->rotateCounterClockwise	= keypressed;	break;
				case 2:	keys->rotateClockwise			= keypressed;	break;
				case 3:	keys->fire						= keypressed;	break;
				case 4:	keys->reverse					= keypressed;	break;
			}
			
			sprintf(playerButtons[P(playerNumber)][actionNumber].text, "%s", convertASCIItoText(keypressed) );
			activeButton = -1;
		}
	}
	glutPostRedisplay();
}

