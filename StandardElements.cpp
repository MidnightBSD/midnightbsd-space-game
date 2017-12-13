/*
 *  StandardElements.cpp
 *  Allenroids
 *
 *  Created by Allen Smith on 1/11/06.
 *  Copyright 2006. All rights reserved.
 *
 */

#include "StandardElements.h"

/****************************************************
* flipBytes
*	reverses the order of all the bytes in the given 
*	buffer. Real handy for those Intel Macs.
***************************************************/
void flipBytes(void *bytes, size_t bufferSize)
{
	unsigned char	*buffer		= (unsigned char*)bytes;
	unsigned char	 firstByte;
	unsigned char	*lastByte;
	unsigned int	 counter;
	
	for(counter = 0; counter < bufferSize/2; counter++)
	{
		firstByte	=  buffer[counter];
		lastByte	= &buffer[bufferSize - counter - 1];
		
		//Swap the two.
		buffer[counter] = *lastByte;
		*lastByte = firstByte;
	}
	
}//end flipBytes

