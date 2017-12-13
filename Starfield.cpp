//==============================================================================
//
// File:		Starfield.cpp
//
// Purpose:		Generates and draws a random starfield for the background.
//
// Modified:	04/07/2009 Allen Smith. Creation Date.
//
//==============================================================================
#include "Starfield.h"

#include <cstdlib>


//========== Starfield =========================================================
//
// Purpose:		Constructor.
//
//==============================================================================
Starfield::Starfield()
{
	displayListTag	= 0;
	starCount		= 0;
}


//========== ~Starfield ========================================================
//
// Purpose:		Destructor.
//
//==============================================================================
Starfield::~Starfield()
{
	DestroyDisplayList();
}


//========== SetStarCount ======================================================
//
// Purpose:		Regenerates a starfield with the specified number of stars.
//
//==============================================================================
void Starfield::SetStarCount(unsigned starCountIn)
{
	this->starCount = starCountIn;
	
	GenerateRandomStarfield();
	OptimizeDrawing();
	
}//end SetStarCount


//========== Draw ==============================================================
//
// Purpose:		Paints the starfield onscreen.
//
//==============================================================================
void Starfield::Draw()
{
	unsigned int    counter     = 0;
	Star			currentStar;
	
	if(this->displayListTag == 0)
	{
		// Immediate-mode drawing
		glBegin(GL_QUADS);
		{
			for(counter = 0; counter < this->starCount; counter++)
			{
				currentStar	= this->stars.at(counter);
			
				glColor3f(currentStar.whiteness, currentStar.whiteness, currentStar.whiteness);
				
				glVertex2f(currentStar.location.x					, currentStar.location.y    );
				glVertex2f(currentStar.location.x + currentStar.size, currentStar.location.y    );
				glVertex2f(currentStar.location.x + currentStar.size, currentStar.location.y + currentStar.size);
				glVertex2f(currentStar.location.x					, currentStar.location.y + currentStar.size);
			}
		}
		glEnd();
	}
	else
	{
		// Optimize drawing
		glCallList(this->displayListTag);
	}	
	
}//end Draw


//========== GenerateRandomStarfield ===========================================
//
// Purpose:		Creates a pretty starfield for the background
//
//==============================================================================
void Starfield::GenerateRandomStarfield()
{
	unsigned	counter		= 0;
	Star		currentStar;
	
	// Get rid of the old stars
	this->stars.clear();
	
	// Make new ones
	for(counter = 0; counter < this->starCount; counter++)
	{
		currentStar.location.x	= rand() % FRAMEWIDTH;
		currentStar.location.y	= rand() % FRAMEHEIGHT;
		
		currentStar.whiteness	= (float) rand() / RAND_MAX;
		currentStar.size		= (float) rand() / RAND_MAX * 0.75 + 0.25;
		
		this->stars.push_back(currentStar);
	}
	
}//end GenerateRandomStarfield


//========== OptimizeDrawing ===================================================
//
// Purpose:		Stores the drawing into a display list, so that it happens very 
//				quickly next time! 
//
//==============================================================================
void Starfield::OptimizeDrawing()
{
	GLuint	newList	= 0;
	
	// clear the current display list.
	DestroyDisplayList();
	
	// generate a new one
	newList	= glGenLists(1);
	glNewList(newList, GL_COMPILE);
	{
		Draw();
	}
	glEndList();
	
	this->displayListTag = newList;
	
}//end OptimizeDrawing


//========== DestroyDisplayList ================================================
//
// Purpose:		Eliminates the fast optimized display list.
//
//==============================================================================
void Starfield::DestroyDisplayList()
{
	if(this->displayListTag != 0)
	{
		glDeleteLists(this->displayListTag, 1);
		this->displayListTag = 0;
	}
	
}//end DestroyDisplayList

