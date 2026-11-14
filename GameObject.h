#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

/*
 *  GameObject.h
 *  Allenroids
 *
 *  Abstract Base Class for all objects used during gameplay
 *
 *  Created by Allen Smith on Thu Jun 17 2004.
 *
 */

#include "StandardElements.h"
#include <stdio.h>
#include <math.h>

class GameObject{

	public:
		Coordinate position{}; //current centerpoint of the object
		Vector direction{}; //the direction vector of *movement*
		int mass = 0; //how heavy this object is; used in collisions
		float radius = 0.0F; //every object is within some radius
		
	public:
		virtual ~GameObject();
	
		virtual void drawInGame() const; //draws the object (calling draw) at its position; handles wrapping at screen edges
		virtual void draw() const =0; //generic draw function pays no attention to position
		void adjustCoordinate(float movementScaleFactor);
		virtual bool intersectsWith(const GameObject *, bool twoWayCompare = true) const; //returns true if this object intersects with the specified object

	protected:
		virtual float collisionRadius() const;
};


#endif
