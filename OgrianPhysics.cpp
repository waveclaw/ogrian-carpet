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
OgrianPhysics.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: Physics is the physics engine. It handles collision detection. 
When collisions are detected, the Things involved are notified via collided().
This also has a moveAll() function that moves all of the particles.
collisionCheck() and moveAll() should be called every frame. 
It is a Singleton.
At the moment, it just checks all all Things against all other Things. 
This will be changed to a quadtree or something for performance.

/*------------------------------------*/



#include "OgrianPhysics.h"
#include "OgrianRollingThing.h"
#include "OgrianConstants.h"

using namespace Ogre;

template<> Ogrian::Physics * Singleton< Ogrian::Physics >::ms_Singleton = 0;
namespace Ogrian
{


Physics::Physics()
{
	mWorldSize = -1;
}

// add a Thing to the world
void Physics::addThing(Thing* thing)
{
	// add to grid
	Vector3 pos = thing->getPosition();
	_addThing(thing, getGridU(pos.x), getGridV(pos.z));

	// add to full list
	mAllThings.push_back(thing);
}

// add a Thing to the grid
void Physics::_addThing(Thing* thing, int grid_u, int grid_v)
{
	assert(thing != NULL);
	assert(mWorldSize > 0);

	if (grid_u >= 0 && 
		grid_v >= 0 && 
		grid_u < PHYSICS_GRID_SIZE && 
		grid_v < PHYSICS_GRID_SIZE)
	{
		// put it in the grid
		mThingGrid[grid_u][grid_v].push_back(thing);
	}
	else
	{
		// put it among the others
		mOtherThings.push_back(thing);
	}
}

// remove a thing from the grid
void Physics::_removeThing(Thing* thing, int grid_u, int grid_v)
{
	assert(thing != NULL);
	assert(mWorldSize > 0);

	std::vector<Thing*> vec;

	if (grid_u >= 0 && 
		grid_v >= 0 && 
		grid_u < PHYSICS_GRID_SIZE && 
		grid_v < PHYSICS_GRID_SIZE)
	{
		// it is in the grid
		vec = mThingGrid[grid_u][grid_v];
	}
	else
	{
		// it is in the others
		vec = mOtherThings;
	}

	// find the thing from the selected vector
	for (unsigned int i=0; i<vec.size(); i++)
	{
		if (vec[i] == thing)
		{
			// erase it
			vec.erase(vec.begin()+i);
			break;
		}
	}
	assert(i<vec.size());
}

// remove a thing from the world
void Physics::deleteThing(Thing* thing)
{
	// remove it from the grid
	Vector3 pos = thing->getPosition();
	_removeThing(thing, getGridU(pos.x), getGridV(pos.z));

	// remove it from allThings
	for (unsigned int i=0; i<mAllThings.size(); i++)
	{
		if (mAllThings[i] == thing)
		{
			// erase it
			mAllThings.erase(mAllThings.begin()+i);
			break;
		}
	}

	assert(i<mAllThings.size());

	// delete it
	delete thing;
}

void Physics::updateThing(Thing* thing, Vector3 lastPos)
{
	Vector3 pos = thing->getPosition();
	int from_u = getGridU(lastPos.x);
	int from_v = getGridV(lastPos.z);
	int to_u = getGridU(pos.x);
	int to_v = getGridV(pos.z);

	// if it crossed a boundary, move it to the new cell
	if (from_u != to_u || from_v != to_v)
	{
		_removeThing(thing, from_u, from_v);
		_addThing(thing, to_u, to_v);
	}
}

int Physics::getGridU(Real x)
{
	return (x/mWorldSize) * PHYSICS_GRID_SIZE;
}

int Physics::getGridV(Real z)
{
	return (z/mWorldSize) * PHYSICS_GRID_SIZE;
}

// remove and delete all things
void Physics::clear()
{
	// clear the grid
	for (int i=0; i<PHYSICS_GRID_SIZE; i++)
		for (int j=0; j<PHYSICS_GRID_SIZE; j++)
			while (!mThingGrid[i][j].empty())
				mThingGrid[i][j].pop_back();

	// delete each entity from mAllThings
	while(!mAllThings.empty())
	{
		delete mAllThings[mAllThings.size()-1];
		mAllThings.pop_back();
	}
}

// move all things, delete the ones not alive
void Physics::moveAll(Real time)
{
	for (unsigned int i=0; i<mAllThings.size(); i++)
	{
		Thing* thing = mAllThings[i];

		// delete anything thats not alive
		if (!thing->isAlive())
		{
			deleteThing(thing);
		}
		// otherwise move it
		else thing->move(time);
	}
}

// the number of things in the list
int Physics::numThings()
{
	return int(mAllThings.size());
}

/* this method works by keeping all of the things in a grid. 
Every frame, each grid cell checks its things against each other
and its neighbors' things.
*/
void Physics::collisionCheck()
{
	unsigned int i,j,t,u;
	i = j = t = u = 0;

	// check the grid for collisions among itself
	for (i=0; i<PHYSICS_GRID_SIZE; i++)
	{
		for (j=0; j<PHYSICS_GRID_SIZE; j++)
		{
			for (t=0; t<mThingGrid[i][j].size(); t++)
			{
				// get the thing
				Thing* thing = mThingGrid[i][j][t];

				// check it against all following things in this cell
				for (u=t+1; u<mThingGrid[i][j].size(); u++)
					pairCollisionCheck(thing, mThingGrid[i][j][u]);

				// check it against all things in the first adjacent cell
				if (i+1 < PHYSICS_GRID_SIZE)
					for (u=0; u<mThingGrid[i+1][j].size(); u++)
						pairCollisionCheck(thing, mThingGrid[i+1][j][u]);
				
				// check it against all things in the second adjacent cell
				if (j+1 < PHYSICS_GRID_SIZE)
					for (u=0; u<mThingGrid[i][j+1].size(); u++)
						pairCollisionCheck(thing, mThingGrid[i][j+1][u]);
				
				// check it against all things in the third adjacent cell
				if (i+1 < PHYSICS_GRID_SIZE && j+1 < PHYSICS_GRID_SIZE)
					for (u=0; u<mThingGrid[i+1][j+1].size(); u++)
						pairCollisionCheck(thing, mThingGrid[i+1][j+1][u]);
			}
		}
	}

	// check the last row for collisions with the others
	
	// check the last column for collisions with the others

	// check the others for collisions with the first row

	// check the others for collisions with the first col

	// check the others for collisions among themselves

}

void Physics::pairCollisionCheck(Thing* a, Thing* b)
{
	Real maxdist = 
		a->getRadius() 
		+ b->getRadius();

	// if their AABB don't interset, return
	if (Math::Abs(a->getPosition().x - b->getPosition().x) > maxdist) return;
	if (Math::Abs(a->getPosition().z - b->getPosition().z) > maxdist) return;

	// if they are too far apart in the x/z plane, return
	if (a->distance(b) > maxdist) return;

	Real ay = a->getPosition().y;
	Real by = b->getPosition().y;

	// if they are close enough in altitude
	if (ay-by < maxdist && by-ay < maxdist)
	{
		// they collide
		a->collided(b);
		b->collided(a);
	}
}

void Physics::setWorldSize(int size)
{
	mWorldSize = size;
}


Physics& Physics::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Physics();
	}
    return Singleton<Physics>::getSingleton();
}


Physics::~Physics()
{
	clear();
}

}
