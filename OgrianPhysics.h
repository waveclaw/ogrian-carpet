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

using namespace Ogre;

namespace Ogrian
{

// The Pysics class handles collision detection and notification among all Thing objects.
class Physics : public Singleton< Physics >
{
public:
	virtual ~Physics();

	// add a physical entity to the world.
	virtual void addThing(Thing* ent);

	// return the number of entities in the world
	virtual int numThings();

	// remove all physical entities from the world.
	virtual void removeAll();

	// notify all physical objects of a frame move
	virtual void moveAll(Real time);

	// run a collision check and notify all affected physical objects
	virtual void collisionCheck();

    static Physics& getSingleton(void);

private:
	Physics();

	// remove a physical entity from the world. 
	virtual void removeThing(Thing* ent);

	std::vector<Thing*> things;
};

}
#endif