/*****************************************************************************
	Copyright 2004 Mike Prosser

    This file is part of Ogrian Carpet.

    Ogrian Carpet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Ogrian Carpet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ogrian Carpet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

/*------------------------------------*
OgrianPhysics.h
Original Author: Mike Prosser
Additional Authors: 

Description: Physics is the physics engine. It handles collision detection. 
When collisions are detected, the Things involved are notified via collided().
This also has a moveAll() function that moves all of the particles.
collisionCheck() and moveAll() should be called every frame. 
It is a Singleton.

/*------------------------------------*/

#ifndef __OgrianPhysics_H__
#define __OgrianPhysics_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianThing.h"
#include "OgrianConstants.h"

using namespace Ogre;

namespace Ogrian
{

// The Pysics class handles collision detection and notification among all Thing objects.
class Physics : public Singleton< Physics >
{
public:
	virtual ~Physics();

	// add a thing to the world in the specified grid location
	// -1 for grid_u or grid_v puts it outside the grid
	virtual void addThing(Thing* thing);

	// this should be called by Thing::setPosition();
	virtual void updateThing(Thing* thing, Vector3 oldPos, Vector3 newPos);

	// return the number of entities in the world
	virtual int numThings();

	// returns true if the thing is in the physics engine
	virtual bool containsThing(Thing* thing);

	// remove and delete all things in the world
	virtual void clear();

	// notify all things of a frame move
	virtual void moveAll(Real time);

	// run a collision check and notify all affected things
	virtual void collisionCheck();

	// this must be called before adding things
	virtual void setWorldSize(int size);

    static Physics& getSingleton(void);

private:
	// a matrix of vectors for collision culling
	std::vector<Thing*> mThingGrid[PHYSICS_GRID_SIZE][PHYSICS_GRID_SIZE];
	std::vector<Thing*> mOtherThings; // the things outside the grid

	std::vector<Thing*> mAllThings; // All things in the world

	int mWorldSize;

	Physics();

	virtual inline int getGridU(Real x); // get the grid u index that corresponds to the real x coord
	virtual inline int getGridV(Real z); // get the grid v index that corresponds to the real z coord

	// do a comprehensive collision check between two things,
	// and notify them if they collide
	virtual inline void pairCollisionCheck(Thing* a, Thing* b);

	// add a thing to the grid
	virtual void _addThing(Thing* thing, int grid_u, int grid_v);
	// remove a thing from the grid. 
	virtual void _removeThing(Thing* thing, int grid_u, int grid_v);

	// remove a thing from the world
	virtual void deleteThing(Thing* thing);
};

}
#endif