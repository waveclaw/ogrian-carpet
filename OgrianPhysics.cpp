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

}

// add a Thing to the things list
void Physics::addThing(Thing* ent)
{
	assert(ent != NULL);

	things.push_back(ent);
}

// remove a thing from the things list and delete it
void Physics::removeThing(Thing* ent)
{
	assert(ent != NULL);

	// find the entity
	for (unsigned int i=0; i<things.size(); i++)
	{
		if (things[i] == ent)
		{
			// erase it
			Thing* thing = things[i];
			things.erase(things.begin()+i);
			delete thing;
			break;
		}
	}
}

// remove and delete all things
void Physics::removeAll()
{
	// delete each entity
	while(!things.empty())
	{
		delete things[things.size()-1];
		things.pop_back();
	}
}

// move all things, delete the ones not alive
void Physics::moveAll(Real time)
{
	for (unsigned int i=0; i<things.size(); i++)
	{
		Thing* thing = things[i];

		// delete anything thats not alive
		if (!thing->isAlive()) removeThing(thing);			
		else thing->move(time);
	}
}

// the number of things in the list
int Physics::numThings()
{
	return int(things.size());
}

/* this method works by keeping all of the entities in an ordered vector.
Each of the entities is check against a few of the following entities for
collisions. 
*/
void Physics::collisionCheck()
{
	// sort the vector (by x location)
	std::sort(things.begin(), things.end());

	Thing* a;
	Thing* b;

	// for each thing
	for (unsigned int i=0; i<things.size(); i++)
	{
		a = things[i];

		// look at the following things
		for (unsigned int j=i+1; j<things.size(); j++)
		{
			b = things[j];

			// they should be sorted by x coord
			assert(a->getPosition().x <= b->getPosition().x);

			if (b->getPosition().x - a->getPosition().x > MAX_THING_RADIUS*2)
			{
				// if they are very far apart, dont look at the following things - WHY DOESN'T THIS WORK?
				//j=(unsigned int)things.size();
			}
			else
			{
				Real maxdist = a->getRadius() + b->getRadius();
				// if they are close enough, they collide
				if (a->distance(b) < maxdist)
				{
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
			}
		}
	}
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
	removeAll();
}

}
