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
#include "OgrianWizardThing.h"
#include "OgrianConstants.h"
#include "OgrianMultiplayer.h"
#include "OgrianTeam.h"

using namespace Ogre;

namespace Ogrian
{
// effects have a UID of -1
#define EFFECT_UID -1

// The Pysics class handles collision detection and notification among all Thing objects.
class Physics : public Singleton< Physics >
{
public:
	virtual ~Physics();

	// add a thing to the world 
	void addThing(Thing* thing);

	// add a thing to the world, but don't do collision detection for it. Useful for effects like smoke.
	void addEffect(Thing* thing);

	// this should be called by Thing::setPosition();
	void updateThing(Thing* thing, Vector3 oldPos, Vector3 newPos);

	// return the number of entities in the world
	int numThings();

	// returns true if the thing is in the physics engine
	bool containsThing(Thing* thing);

	// get the thing with the specified uid
	Thing* getThing(int uid);

	// remove and delete all things in the world
	void clear();

	// call this every frame
	void frame(Real time);

	// this must be called before adding things
	void setWorldSize(int size);

	// handle server packet, return true if handled
	bool handleServerPacket(Packet* p, PacketID pid);

	// handle client packet, return true if handled
	bool handleClientPacket(Packet* p, PacketID pid);

	// add a new team
	int addTeam(int wizardUID);

	// get a team by number
	Team* getTeam(int index);

	// clear the teams
	void clearTeams();

	// send a list of all things to the log
	void listThings();

    static Physics& getSingleton(void);

private:
	// a matrix of vectors for collision culling
	std::vector<Thing*> mThingGrid[PHYSICS_GRID_SIZE][PHYSICS_GRID_SIZE];
	std::vector<Thing*> mOtherThings; // the things outside the grid
	std::vector<Thing*> mAllThings; // All things in the world
	std::vector<Thing*> mEffects; // a list of effects

	std::vector<Team*> mTeams;

	int mWorldSize;

	int mCurrentUID; // each thing is assigned an incrementing UID

	Physics();

	// notify all things of a frame move
	void moveAll(Real time);

	// run a collision check and notify all affected things
	void collisionCheck();

	inline int getGridU(Real x); // get the grid u index that corresponds to the real x coord
	inline int getGridV(Real z); // get the grid v index that corresponds to the real z coord

	// do a comprehensive collision check between two things,
	// and notify them if they collide
	inline void pairCollisionCheck(Thing* a, Thing* b);

	// cause a and b to collide
	inline void collide(Thing* a, Thing* b);

	// call this every frame for servers
	void serverFrame(Real time);

	// call this every frame for clients
	void clientFrame(Real time);

	// add a thing to the grid
	void _addThing(Thing* thing, int grid_u, int grid_v);
	// remove a thing from the grid. 
	void _removeThing(Thing* thing, int grid_u, int grid_v);

	// remove a thing from the world
	void deleteThing(Thing* thing);

	// remove an effect from the world
	void deleteEffect(Thing* thing);

	// add a thing as a client
	void clientAddThing(Thing* thing, int uid);

	// sort mAllThings by uid
	void _sortAllThings();

	// make a new thing of the specified type
	Thing* newThing(ThingType type, int teamNum);
};

}
#endif