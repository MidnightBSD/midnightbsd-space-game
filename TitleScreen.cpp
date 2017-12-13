/*
 *  Title Screen.c
 *  Asteroids
 *
 *  Created by Allen Smith on Mon Oct 21 2002.
 *
 */

#include "Allenroids.h"
#include "TitleScreen.h"
#include "ConfigureScreen.h"

/*****Global Variables*****/
extern GLubyte playerColors[][3];

MyButton titleButtons[3];
extern int buttonBeingPressed; //defined in Configure Screen
extern int buttonWidth;
extern int buttonHeight;

//Modes
boolean enterHighScoredMode = false;

Score highscores[NUMBER_HIGH_SCORES];
Score newScore; //temporary holding cell for a new high score
short indexNewScore; //the index of the new score
short playerNumberWithScore;

//Players
extern short numberOfPlayers;
extern Player playerObjects[MAX_NUMBER_PLAYERS];
extern int playerKeys[MAX_NUMBER_PLAYERS][5];



/****************************************************
 * titleInit
 *  set up the title screen
 ***************************************************/
void titleInit(){
	//GLUT function setup
	glutKeyboardFunc(titleHandleKeyboard);
	glutIgnoreKeyRepeat(true);
	glutMouseFunc(titleHandleMouse);
	glutDisplayFunc(titleDisplay);
	glutReshapeFunc(reshape); //OS 9 GLUT crashes with this function.
	glutIdleFunc(NULL); //stop idle loop leftover from a previous game

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //cull faces produced from clockwise vertices
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0,0,0, 0.5);
	glClear(GL_COLOR_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
//	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, FRAMEWIDTH, 0, FRAMEHEIGHT);
	
	readHighScores();
	readConfiguration();
	
	spawnStarfield();
	buttonWidth = 128;
	buttonHeight = 32;
		
	sprintf(titleButtons[0].text, "One-player Game");
	titleButtons[0].position.x = FRAMEWIDTH*1/4;
	titleButtons[0].position.y = FRAMEHEIGHT*3/16;
		titleButtons[0].color[0] = playerColors[P(1)][0]; //Note:
		titleButtons[0].color[1] = playerColors[P(1)][1]; //the player objects haven't been initialized yet
		titleButtons[0].color[2] = playerColors[P(1)][2]; //so you can't get the colors from there!
		titleButtons[0].color[3] = 128; //50%
	
	sprintf(titleButtons[1].text, "Two-player Game");
	titleButtons[1].position.x = FRAMEWIDTH*1/2;
	titleButtons[1].position.y = FRAMEHEIGHT*3/16;
		titleButtons[1].color[0] = playerColors[P(2)][0];
		titleButtons[1].color[1] = playerColors[P(2)][1];
		titleButtons[1].color[2] = playerColors[P(2)][2];
		titleButtons[1].color[3] = 128; //50%
	
	sprintf(titleButtons[2].text, "Configure");
	titleButtons[2].position.x = FRAMEWIDTH*3/4;
	titleButtons[2].position.y = FRAMEHEIGHT*3/16;
		titleButtons[2].color[0] = 255;
		titleButtons[2].color[1] = 0;
		titleButtons[2].color[2] = 0;
		titleButtons[2].color[3] = 128; //50% transparency
	

}

/****************************************************
 * titleDisplay
 *  paints all the title elements
 ***************************************************/
void titleDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawStarfield(); //in asteroids.c
	printTitle();
	printHighScores();
	if(enterHighScoredMode == false)
		configurePrintButtons(titleButtons,3,GL_RENDER, 0);
	//	printButtons(GL_RENDER);
	glutSwapBuffers();
	
}


/****************************************************
 * printTitle
 *  writes the word "Allenroids" in large letters at the top
 ***************************************************/
void printTitle() {
	static const char gameTitle[] = "MidnightBSD Space Game";
	unsigned int counter;
	int stringWidth = 0;
	float scaleFactor = 0.2; //20%
	
	glColor3ub(255,255,255);
	glLineWidth(5.0);
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
 * printHighScores
 *  prints out the whole beautiful high score list
 ***************************************************/
void printHighScores() {
	char            mainTitle[36]           = "";
	unsigned		stringLength			= 0;
	float           stringWidth             = 0;
	float			charWidth				= 0.0;
	char            columnHeadings[72]      = "";
	char            dateForScore[10]        = "";	//a textual version of the high score's date
	char            highScoreString[128]    = "";
	char            name[32]                = "";	// a temporary place to hold the name to print for the score
	int             linePostion             = 0;	//in pixels; the distance from the top of the screen to render the current line of text.
	float			highscoreLeftMargin		= 0;
	unsigned int    counter                 = 0;
	int             scoreIndex              = 0;	//counter for highScore array
	float           scaleFactor             = 0.25;	//25%
	
	//--------------------------------------------------------------------------
	// Print heading/instructions
	
	// This mode can be interactive!
	if(enterHighScoredMode == false){
		sprintf(mainTitle, "High Scores");
		glColor3ub(255,255,255);
	}
	else{
		sprintf(mainTitle, "Player %d earned a new high score!", playerNumberWithScore);
		glColor3ubv(playerObjects[P(playerNumberWithScore)].getColors());
	}
	glLineWidth(2.0);
	//DO NOT LOAD A MATRIXMODE! It totally messes up the coordinate system.
	glPushMatrix();
	{
		for(counter = 0; counter < strlen(mainTitle); counter++){
			stringWidth += glutStrokeWidth( GLUT_STROKE_ROMAN , mainTitle[counter]); //since this is a variable-width font
		}
		
		linePostion = 128;
		//print out the words "High Scores" at the top
		glTranslatef( FRAMEWIDTH/2 - (stringWidth/2)*scaleFactor, FRAMEHEIGHT - linePostion, 0);
		glScalef(scaleFactor, scaleFactor, 0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < strlen(mainTitle); counter++){
			glutStrokeCharacter( GLUT_STROKE_ROMAN , mainTitle[counter]);
		}
	}
	glPopMatrix();


	//--------------------------------------------------------------------------
	// Print column headings for the high score list
	
	sprintf(columnHeadings, "    %-30s%5s      %-7s %-14s", "Name", "Level", "Score", "Date");
	stringLength = strlen(columnHeadings);
	linePostion += 36; //now it equals 164
	
	glPushMatrix();
	{
		charWidth	= glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'A');
		scaleFactor = charWidth / 104.76; // GLUT_STROKE_MONO_ROMAN width in GLUT spec
		stringWidth = stringLength * charWidth;
		
		highscoreLeftMargin = FRAMEWIDTH/2 - stringWidth/2;
		
		glTranslatef( highscoreLeftMargin, FRAMEHEIGHT - linePostion, 0);
		glScalef(scaleFactor, scaleFactor, 0);
		
		glLineWidth(1.0);
		glRasterPos2i(0,0);
		for(counter = 0; counter < stringLength; counter++){
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN , columnHeadings[counter]);
//			glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , columnHeadings[counter]);
		}
	}
	glPopMatrix();
	
	// Underscore separating high score heading from list
	glLineWidth(1.0);
	glBegin(GL_LINES);
	{
		glVertex2f(highscoreLeftMargin, FRAMEHEIGHT-170);
		glVertex2f(FRAMEWIDTH/2 + stringWidth/2, FRAMEHEIGHT-170);
	}
	glEnd();
	

	//--------------------------------------------------------------------------
	// Print the actual high score list
	
	linePostion += 2*13; //initial line position
	
	for(scoreIndex = 0; scoreIndex < NUMBER_HIGH_SCORES; scoreIndex++)
	{
		//Format the time for Month/Day/Year format
		strftime(dateForScore,14, "%b. %d, %Y",localtime(&(highscores[scoreIndex].date)) );
		
		if(enterHighScoredMode == true && scoreIndex == indexNewScore){ //a line for entry of a new high score
			glColor3ub(255,0,0); //Highlight the new score
			strcpy( name,newScore.name );
			strcat(name, "_");
			sprintf(highScoreString,  "%2d. %-31s%5d %10d  %-14s",scoreIndex+1, name, newScore.level, newScore.score, "");
		}
		else{ //print a preexisting high score
			glColor3ub(255,255,255);
			sprintf(highScoreString,  "%2d. %-31s%5d %10d  %-14s",scoreIndex+1, highscores[scoreIndex].name, highscores[scoreIndex].level, highscores[scoreIndex].score, dateForScore);
		}
		
		// Get string measurements
		stringLength    = strlen(highScoreString);
		charWidth       = glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'A');
		scaleFactor     = charWidth / 104.76; // GLUT_STROKE_MONO_ROMAN width in GLUT spec
		stringWidth     = stringLength * charWidth;
		
		// Draw the string
		glPushMatrix();
		{
			glTranslatef( highscoreLeftMargin, FRAMEHEIGHT - linePostion, 0);
			glScalef(scaleFactor, scaleFactor, 0);
			
			glLineWidth(1.0);
			glRasterPos2i(0,0);
			for(counter = 0; counter < stringLength; counter++){
				glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN , highScoreString[counter]);
//				glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , highScoreString[counter]);
			}
		}
		glPopMatrix();

		linePostion += 13; //increment for character height
		
	}//end for loop to print highscores
	
	
	//--------------------------------------------------------------------------
	// Print Author notice at bottom
	
	if(enterHighScoredMode == false){
		glColor3ub(255,255,255);
		stringWidth = sprintf(highScoreString,  "MidnightBSD Space Game %s: This masterpiece brought to you by Allen M. Smith, 2002-%d.", ALLENROIDS_VERSION, ALLENROIDS_VERSION_DATE);
		stringWidth = stringWidth*glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'A');
		
		glRasterPos2f( FRAMEWIDTH/2 - (stringWidth/2)/((float)glutGet(GLUT_WINDOW_WIDTH)/FRAMEWIDTH), (FRAMEHEIGHT * (float) 1/32) );
		for(counter = 0; counter < strlen(highScoreString); counter++){
			glutBitmapCharacter( GLUT_BITMAP_8_BY_13 , highScoreString[counter]);
		}
	}

}


/****************************************************
 * writeHighScores
 *  writes scores to a file
 ***************************************************/
void writeHighScores(){
	FILE *filePtr;
	unsigned int counter;
	Score currentScore;
	if(( filePtr = fopen("High Scores", "w+") ) == NULL){
		//printf("The file could not be opened.");
	}
	else{
		for(counter = 0; counter < NUMBER_HIGH_SCORES; counter++){
			//make a copy of the current score so we can safely flip bytes
			// if we need to without affecting the original data.
			currentScore = highscores[counter];
			
			//On Macs, we always write the data big-endian. So if 
			// we're on an x86 Mac, we need to flip the bytes.
			#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
				flipBytes(&currentScore.level, sizeof(short));
				flipBytes(&currentScore.score, sizeof(int));
				flipBytes(&currentScore.date,  sizeof(time_t));
			#endif

			fwrite( &currentScore, sizeof(struct highScore), 1, filePtr);
		}
		fclose( filePtr );
	}
}

		
/****************************************************
 * readHighScores
 *  writes scores to a file
 ***************************************************/
void readHighScores(){
	FILE *filePtr;
	unsigned int counter = 0;
	if(( filePtr = fopen("High Scores", "r") ) == NULL){
		//printf("The file could not be opened for reading.");
		writeHighScores();
	}
	else{
		for(counter = 0; counter < NUMBER_HIGH_SCORES; counter++){
			if( !feof(filePtr) ){
				fread( &highscores[counter], sizeof(struct highScore), 1, filePtr);
				//On Macs, we always write the data big-endian. So if 
				// we're on an x86 Mac, we need to flip the bytes.
				#if defined(__APPLE__) && defined(__LITTLE_ENDIAN__)
					flipBytes(&(highscores[counter].level), sizeof(short));
					flipBytes(&(highscores[counter].score), sizeof(int));
					flipBytes(&(highscores[counter].date),  sizeof(time_t));
				#endif
			}
			else{
				//printf("The file is not of a valid format");
				writeHighScores();
				readHighScores();
			}
		}//end for
		fclose(filePtr);
	}//end else
	
}//end readHighScores

		
/****************************************************
 * newHighScore
 *  adds a new high score to the list for the specified
 *	player. It is used by first being called for player 1,
 *	then for player 2 in the keyboard method when player1's
 *	score is confirmed.
 ***************************************************/
void newHighScore(int player){
	int counter;
	
	indexNewScore = -1; //bogus value, since NULL apparently evaluates to zero
	counter = 0;
	
	while( (counter < NUMBER_HIGH_SCORES) && (indexNewScore == -1) ){
		if( highscores[counter].score < playerObjects[P(player)].score ){
			indexNewScore = counter;
		}
		else
			counter++;
	}
	if(indexNewScore != -1){
		for(counter = NUMBER_HIGH_SCORES - 1; counter > indexNewScore; counter--)
			highscores[counter] = highscores[counter-1]; //that is, the current cell is set to the previous array cell (shifting down)
		newScore.level = playerObjects[P(player)].highestLevelAttained;
		newScore.score = playerObjects[P(player)].score;
		newScore.date = time(NULL);
		memset( newScore.name, '\0', sizeof(newScore.name) ); //clear the new name
		playerNumberWithScore = player;
		enterHighScoredMode = true; //change display to accept score input
		glutPostRedisplay();
	}
	else if(numberOfPlayers > player)
		newHighScore(player + 1);
		
}


/****************************************************
 * titleHandleMouse
 *  accepts mouse input
 ***************************************************/
void titleHandleMouse(int button, int state, int x, int y){
	GLuint nameBuffer[512] = {0};
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
		
		configurePrintButtons(titleButtons,3,GL_SELECT, 0);
		//restore viewing matrix
		glPopMatrix();
		glutSwapBuffers();
		
		numberOfHits = glRenderMode(GL_RENDER); //normal render mode
		processHits(numberOfHits, nameBuffer,state);
		glutPostRedisplay();
	}
}


/****************************************************
 * processHits
 *  process a click on a button
 ***************************************************/
void processHits(unsigned int numberOfHits, GLuint buffer[], int mouseState){
	unsigned int counterHits, counterNames;
	GLuint names, *namesPtr;
	
	namesPtr = (GLuint *) buffer;
	
	if(numberOfHits == 0){
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
					if(buttonBeingPressed == (int)*namesPtr){ //make sure we are releasing the mouse on the button we started with
						if(*namesPtr == 0){
							gameInit(1); //new 1-Player game
						}
						else if(*namesPtr == 1){
							gameInit(2); //new 2-Player game
						}
						else if(*namesPtr == 2){
							configureInit(); //configure
						}
						glutPostRedisplay();
					}
					namesPtr++;
				}//end for names
			}//end for hits
			buttonBeingPressed = -1;
		}//end else for mouse released
	}//end numberHits > 0
}


/****************************************************
 * titleHandleKeyboard
 *  accepts regular keyboard input
 ***************************************************/
void titleHandleKeyboard(unsigned char keypressed, int x, int y){
//	printf("%d\n",keypressed);
	if(enterHighScoredMode == true){
		if(keypressed == 13 || keypressed == 3){ //carriage return
			highscores[indexNewScore] = newScore;
			enterHighScoredMode = false; //done entering score
			writeHighScores();
			if(numberOfPlayers > playerNumberWithScore)
				newHighScore(playerNumberWithScore + 1);
		}
		else if (keypressed == 8 || keypressed == 127){ //Delete is a 127, not an 8. Don't ask me why.
			//Make the previous character into the terminator. This removes it from the string.
			newScore.name[strlen(newScore.name) - 1] = '\0';
		}
		else{
			if(strlen(newScore.name) < 31) //this is the maximum length of a name
				sprintf(newScore.name, "%s%c",newScore.name, keypressed);
			else
				titleHandleKeyboard(13,0,0); //press return for them!
		}
	}
	glutPostRedisplay();
}

/****************************************************
 * reshape
 *  when the window is resized
 *	constrains the window size to the proper aspect ratio
 ***************************************************/
void reshape(int newWidth, int newHeight){
	float aspectRatio = (float) FRAMEWIDTH / (float) FRAMEHEIGHT;
//	newHeight = newHeight - newHeight%12;
	newWidth = (int) ((float) newHeight * aspectRatio);
		
	glViewport(0,0,newWidth, newHeight); //reconstrain the window contents to the actual window!
	glutReshapeWindow(newWidth, newHeight);
}



/****************************************************
 * main
 ***************************************************/
int main(int argc, char **argv) {
	float   aspectRatio     = (float) FRAMEWIDTH / (float) FRAMEHEIGHT;
	int     screenWidth     = 0;
	int     screenHeight    = 0;
	int     windowWidth     = 0;
	int     windowHeight    = 0;
	int     windowOriginX   = 0;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	// Make the window as big as the screen allows
	screenWidth     = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight    = glutGet(GLUT_SCREEN_HEIGHT);
	
	windowHeight	= screenHeight;
	windowWidth		= (int)(windowHeight * aspectRatio);
	
	// Center it
	windowOriginX	= (screenWidth - windowWidth) / 2;
	
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(windowOriginX, 0);
	glutCreateWindow("MidnightBSD Space Game");
	
	// Here we go!
	titleInit();
	glutMainLoop();
	
	return 0;
}
