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
#include "OgrianRenderer.h"
#include "OgrianConstants.h"
#include "OgrianTime.h"
#include "OgrianTeam.h"

#include "OgrianFireballThing.h"
#include "OgrianManaThing.h"
#include "OgrianCameraThing.h"
#include "OgrianFoliageThing.h"

#include "OgreLogManager.h"

using namespace Ogre;

template<> Ogrian::Physics * Singleton< Ogrian::Physics >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

Physics::Physics()
{
	mWorldSize = -1;
	mCurrentUID = 0;
}

//----------------------------------------------------------------------------

void Physics::frame(Real time)
{
	// move all particles
	moveAll(time);

	if (!Multiplayer::getSingleton().isClient())
	{
		// clients dont do collision checking
		collisionCheck();
	}

	if (Multiplayer::getSingleton().isClient())
	{
		// clients need to send updates
		clientFrame(time);
	}
	else if (Multiplayer::getSingleton().isServer())
	{
		// servers need to send updates
		serverFrame(time);
	}
}

//----------------------------------------------------------------------------

void Physics::clientFrame(Real time)
{
	CameraThing* cthing = static_cast<CameraThing*>(Renderer::getSingleton().getCameraThing());

	// do nothing if we dont have a camera thing yet
	if (cthing == 0) return;

	// do nothing if we don't have a postive uid for the camera
	if (cthing->getUID() < 1) return;

	if (cthing->lastUpdateTime() + THING_UPDATE_PERIOD/2 < Time::getSingleton().getTime())
	{
		// notify the server of our camerathing
		BitStream bs;
		cthing->generateBitStream(bs);
		Multiplayer::getSingleton().clientSend(&bs, false);
	}
}
//----------------------------------------------------------------------------

void Physics::serverFrame(Real time)
{
	// for each client
	for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
	{
		PlayerInfo player = Multiplayer::getSingleton().getClient(i);

		// notify clients of all things
		for (int i=0; i<(int)mAllThings.size(); i++)
		{
			Thing* thing = mAllThings[i];
			WizardThing* wiz = static_cast<WizardThing*>(getThing(player.wizardUID));

			if (thing->lastUpdateTime() + THING_UPDATE_PERIOD < Time::getSingleton().getTime() // only send periodically
				&& thing->getUID() != player.wizardUID // dont send their own wizard to them
				&& thing->axisDistance(wiz) < THING_CULL_DIST // dont send things they cant see
				&& thing->isMoving() // dont send things that aren't moving
				)
			{
				BitStream bs;
				thing->generateBitStream(bs);
				Multiplayer::getSingleton().serverSend(&bs, player.id, false); // send unreliably
			}
		}
	}
}

//----------------------------------------------------------------------------

bool Physics::handleClientPacket(Packet* packet, PacketID pid)
{
	if (packet == 0) return false;

	// if its a thing update
	if (pid == ID_UPDATE_THING)
	{
		BitStream bitstream(packet->data, packet->length, true);
		int id, uid, type;

		bitstream.Read(id);
		bitstream.Read(uid);
		bitstream.Read(type);
		bitstream.ResetReadPointer();

		// find the thing
		Thing* thing = getThing(uid);

		if (thing == 0) // we need to make a new one if we dont have it
		{
			// make a new thing
			thing = newThing((ThingType)type, 0);

			// log it
			//LogManager::getSingleton().logMessage(String("Making New Thing for client: #") << uid);

			// send the bitstream to the thing
			thing->interpretBitStream(bitstream);

			// add it to physics
			clientAddThing(thing, uid);

			if (getThing(uid) == 0) 
			{
				LogManager::getSingleton().logMessage(String("Add Failure on thing #") << uid);
				listThings();
			}
		}
		else // just send the update
		{
			// send the bitstream to the thing
			thing->interpretBitStream(bitstream);
		}

		return true;
	}

	// if its a thing deletion
	else if (pid == ID_REMOVE_THING)
	{
		// find the thing
		BitStream bitstream(packet->data, packet->length, true);
		int id, uid;

		bitstream.Read(id);
		bitstream.Read(uid);
		bitstream.ResetReadPointer();

		Thing* thing = getThing(uid);

		// destroy it
		if (thing != 0) thing->destroy();
		else 
		{
			LogManager::getSingleton().logMessage(String("Remove Failure on thing #") << uid);
			listThings();
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

bool Physics::handleServerPacket(Packet* packet, PacketID pid)
{
	if (packet == 0) return false;

	// if its a thing creation or update
	if (pid == ID_UPDATE_THING)
	{
		BitStream bitstream(packet->data, packet->length, true);
		int id, uid, type;

		bitstream.Read(id);
		bitstream.Read(uid);
		bitstream.Read(type);
		bitstream.ResetReadPointer();

		// if its a client updating its cameraThing
		if (type == CAMERATHING)
		{
			// get the wizardThing
			int uid = Multiplayer::getSingleton().getWizardUID(packet->playerId);
			WizardThing* wthing = static_cast<WizardThing*>(getThing(uid));

			if (wthing == 0)
			{
				LogManager::getSingleton().logMessage(String("wizardthing not found: #") << uid);
			}
			else
			{
				// send the update to it
				wthing->interpretBitStream(bitstream);
			}
		}
		else // otherwise, its a new thing
		{
			// make a new thing
			Thing* thing = newThing((ThingType)type, 
				Multiplayer::getSingleton().getPlayerInfo(packet->playerId)->teamNum);

			// send the bitstream to the thing
			thing->interpretBitStream(bitstream);

			// add it to the physics
			addThing(thing);

			// send it to the clients immediately
			BitStream bs;
			thing->generateBitStream(bs);
			Multiplayer::getSingleton().serverSendAll(&bs, false);
		}
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------

Thing* Physics::newThing(ThingType type, int teamNum)
{
	//LogManager::getSingleton().logMessage(String("Making Thing: ") << type << " on team #" << teamNum);

	switch(type)
	{
		case FOLIAGETHING:	return new FoliageThing();

		case MANATHING:	return new ManaThing();

		case FIREBALLTHING:	return new FireballThing(teamNum);

		case CAMERATHING: return new WizardThing();

		default:
			LogManager::getSingleton().logMessage(String("ERROR: Thing Unknown: ") << type);
			return 0;
	}
}

//----------------------------------------------------------------------------

void Physics::listThings()
{
	LogManager::getSingleton().logMessage("All Things:");

	for (int i=0; i<(int)mAllThings.size(); i++)
		LogManager::getSingleton().logMessage(String(" thing: ") << mAllThings[i]->getString());
}

//----------------------------------------------------------------------------

Thing* Physics::getThing(int uid)
{
	if (mAllThings.size() == 0) return 0;

	// binary search
	int left = 0;
	int right = (int)mAllThings.size()-1;
	int mid, cuid;

	while (left <= right)
	{
		mid = (left+right)/2;
		cuid = mAllThings[mid]->getUID();
		if (cuid == uid) return mAllThings[mid];
		else if (uid < cuid) right = mid-1;
		else if (uid > cuid) left = mid+1;
	}

	return 0;
}
//----------------------------------------------------------------------------

// checks to see if a thing is in the list mAllThings
bool Physics::containsThing(Thing* thing)
{
	for (unsigned int i=0; i<mAllThings.size(); i++)
		if (mAllThings[i] == thing) return true;

	return false;
}

//----------------------------------------------------------------------------

void Physics::addEffect(Thing* thing)
{
	mEffects.push_back(thing);
		
	// notify the thing
	thing->placedInPhysics(EFFECT_UID);
}

//----------------------------------------------------------------------------

// add a Thing to the world
void Physics::addThing(Thing* thing)
{
	// if its a client
	if (Multiplayer::getSingleton().isClient())
	{
		// send a message to the server telling it about the new thing
		BitStream bs;
		thing->generateBitStream(bs);
		Multiplayer::getSingleton().clientSend(&bs);

		// discard the thing
		delete thing;
	}
	else // if its a server or singleplayer
	{
		// make sure it's not too big
		if (thing->getWidth() > mWorldSize / PHYSICS_GRID_SIZE)
		{
			Except( Exception::ERR_INTERNAL_ERROR, "Error: Thing Too Big for Grid. Make the world bigger, the thing smaller, or the grid coarser.",
					"Physics::addThing" );
		}


		// add to grid
		Vector3 pos = thing->getPosition();
		_addThing(thing, getGridU(pos.x), getGridV(pos.z));

		// add to full list
		mAllThings.push_back(thing);

		// notify the thing
		thing->placedInPhysics(mCurrentUID++);

		// keep allthings sorted by uid
		_sortAllThings();
	}
}

//----------------------------------------------------------------------------

// add a Thing to the world as a client
void Physics::clientAddThing(Thing* thing, int uid)
{
	// add to full list
	mAllThings.push_back(thing);

	// notify the thing
	thing->placedInPhysics(uid);

	// keep allthings sorted by uid
	_sortAllThings();
}

//----------------------------------------------------------------------------

void Physics::_sortAllThings()
{
	// use insertion sort because it should already be sorted or nearly sorted
	for (int i=1; i<(int)mAllThings.size(); i++)
	{
		Thing* t = mAllThings[i];
		int u = t->getUID();
		int j=i;
		while (j>0 && mAllThings[j-1]->getUID() > u)
		{
			mAllThings[j] = mAllThings[j-1];
			j--;
		}
		mAllThings[j] = t;
	}
}

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

// remove a thing from the grid
bool Physics::_removeThing(Thing* thing, int grid_u, int grid_v)
{
	assert(thing != NULL);
	assert(mWorldSize > 0);

	if (grid_u >= 0 && 
		grid_v >= 0 && 
		grid_u < PHYSICS_GRID_SIZE && 
		grid_v < PHYSICS_GRID_SIZE)
	{
		// find the thing from the grid
		size_t s = mThingGrid[grid_u][grid_v].size();
		for (unsigned int i=0; i<mThingGrid[grid_u][grid_v].size(); i++)
		{
			if (mThingGrid[grid_u][grid_v][i] == thing)
			{
				// erase it
				mThingGrid[grid_u][grid_v].erase(mThingGrid[grid_u][grid_v].begin()+i);
				return true;
			}
		}
		// assert that one was removed
		assert(mThingGrid[grid_u][grid_v].size() == s-1);
		LogManager::getSingleton().logMessage(String("Error Removing Thing, not found in grid: ") << thing->getString());
		return false;
	}
	else
	{
		// find the thing from the other things
		size_t s = mOtherThings.size();
		for (unsigned int i=0; i<mOtherThings.size(); i++)
		{
			if (mOtherThings[i] == thing)
			{
				// erase it
				mOtherThings.erase(mOtherThings.begin()+i);
				return true;;
			}
		}
		// assert that one was removed
		assert(mOtherThings.size() == s-1);
		LogManager::getSingleton().logMessage(String("Error Removing Thing, not found in others: ") << thing->getString()); 
		return false;
	}
}

//----------------------------------------------------------------------------

void Physics::deleteEffect(Thing* thing)
{
	// remove it from mEffects
	for (unsigned int i=0; i<mEffects.size(); i++)
	{
		if (mEffects[i] == thing)
		{
			// erase it
			mEffects.erase(mEffects.begin()+i);
			break;
		}
	}

	delete thing;
}

//----------------------------------------------------------------------------

// remove a thing from the world
void Physics::deleteThing(Thing* thing)
{
	// if this is a server, propogate the deletion to clients
	if (Multiplayer::getSingleton().isServer())
	{
		int uid = thing->getUID();
		BitStream bs;
		bs.Write(ID_REMOVE_THING);
		bs.Write(uid);

		if (thing->getType() == WIZARDTHING)
		{
			// for wizardthings, 
			for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
			{
				PlayerInfo client = Multiplayer::getSingleton().getClient(i);

				// dont send it to the removed player
				if (uid != client.wizardUID)
					Multiplayer::getSingleton().serverSend(&bs, client.id);
			}
		}
		else
		{
			// otherwise, just send it to all
			Multiplayer::getSingleton().serverSendAll(&bs);
		}
	}

	// remove it from the grid
	Vector3 pos = thing->getPosition();
	bool removed = _removeThing(thing, getGridU(pos.x), getGridV(pos.z));

	// remove it from allThings
	size_t s = mAllThings.size();
	for (unsigned int i=0; i<mAllThings.size(); i++)
	{
		if (mAllThings[i] == thing)
		{
			// erase it
			mAllThings.erase(mAllThings.begin()+i);
			break;
		}
	}

	// assert that one was removed
	assert(mAllThings.size() == s-1);

	// delete it
	if (removed)
		delete thing;
}

//----------------------------------------------------------------------------

void Physics::updateThing(Thing* thing, Vector3 oldPos, Vector3 newPos)
{
	int from_u = getGridU(oldPos.x);
	int from_v = getGridV(oldPos.z);
	int to_u = getGridU(newPos.x);
	int to_v = getGridV(newPos.z);

	// if it crossed a boundary, move it to the new cell
	if (from_u != to_u || from_v != to_v)
	{
		_removeThing(thing, from_u, from_v);
		_addThing(thing, to_u, to_v);
	}
}

//----------------------------------------------------------------------------

int Physics::getGridU(Real x)
{
	return (x/mWorldSize) * PHYSICS_GRID_SIZE;
}

//----------------------------------------------------------------------------

int Physics::getGridV(Real z)
{
	return (z/mWorldSize) * PHYSICS_GRID_SIZE;
}

//----------------------------------------------------------------------------

// remove and delete all things
void Physics::clear()
{
	if (!Multiplayer::getSingleton().isClient())
	{
		// clear the grid
		for (int i=0; i<PHYSICS_GRID_SIZE; i++)
			for (int j=0; j<PHYSICS_GRID_SIZE; j++)
				while (!mThingGrid[i][j].empty())
					mThingGrid[i][j].pop_back();
	}

	// delete each thing from mAllThings
	while(!mAllThings.empty())
	{
		Thing* thing = mAllThings[mAllThings.size()-1];
		if (thing->getType() != CAMERATHING) delete thing;
		mAllThings.pop_back();
	}

	// delete each effect from mEffects
	while (!mEffects.empty())
	{
		delete mEffects[mEffects.size()-1];
		mEffects.pop_back();
	}

}

//----------------------------------------------------------------------------

// move all things, delete the ones not alive
void Physics::moveAll(Real time)
{
	// move all things
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
	
	// move all effects
	for (unsigned int i=0; i<mEffects.size(); i++)
	{
		Thing* thing = mEffects[i];

		// delete anything thats not alive
		if (!thing->isAlive())
		{
			deleteEffect(thing);
		}
		// otherwise move it
		else thing->move(time);
	}
}

//----------------------------------------------------------------------------

// the number of things in the list
int Physics::numThings()
{
	return int(mAllThings.size());
}

//----------------------------------------------------------------------------

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
	for (i=0; i<PHYSICS_GRID_SIZE; i++)
	{
		for (t=0; t<mThingGrid[i][PHYSICS_GRID_SIZE-1].size(); t++)
		{
			// get the thing
			Thing* thing = mThingGrid[i][PHYSICS_GRID_SIZE-1][t];

			// check it against all following things in this cell
			for (u=0; u<mOtherThings.size(); u++)
				pairCollisionCheck(thing, mOtherThings[u]);
		}
	}

	// check the last column for collisions with the others
	for (j=0; j<PHYSICS_GRID_SIZE-1; j++)
	{
		for (t=0; t<mThingGrid[PHYSICS_GRID_SIZE-1][j].size(); t++)
		{
			// get the thing
			Thing* thing = mThingGrid[PHYSICS_GRID_SIZE-1][j][t];

			// check it against all other things
			for (u=0; u<mOtherThings.size(); u++)
				pairCollisionCheck(thing, mOtherThings[u]);
		}
	}

	// check the others for collisions with the first row
	for (i=0; i<PHYSICS_GRID_SIZE; i++)
	{
		for (t=0; t<mThingGrid[i][0].size(); t++)
		{
			// get the thing
			Thing* thing = mThingGrid[i][0][t];

			// check it against all other things
			for (u=0; u<mOtherThings.size(); u++)
				pairCollisionCheck(thing, mOtherThings[u]);
		}
	}

	// check the others for collisions with the first col
	for (j=0; j<PHYSICS_GRID_SIZE-1; j++)
	{
		for (t=0; t<mThingGrid[0][j].size(); t++)
		{
			// get the thing
			Thing* thing = mThingGrid[0][j][t];

			// check it against all other things
			for (u=0; u<mOtherThings.size(); u++)
				pairCollisionCheck(thing, mOtherThings[u]);
		}
	}

	// check the others for collisions among themselves
	for (i=0; i<mOtherThings.size(); i++)
	{
		// get the thing
		Thing* thing = mOtherThings[i];

		// check it against all following other things
		for (u=i+1; u<mOtherThings.size(); u++)
			pairCollisionCheck(thing, mOtherThings[u]);
	}
}

//----------------------------------------------------------------------------

void Physics::pairCollisionCheck(Thing* a, Thing* b)
{
	Real maxdist = (a->getWidth() + b->getWidth())/2;
	Real maxHdist = (a->getHeight() + b->getHeight())/2;

	Vector3 apos = a->getPosition();
	Vector3 bpos = b->getPosition();

	// if their AABB don't interset, return
	if (Math::Abs(apos.x - bpos.x) > maxdist) return;
	if (Math::Abs(apos.y - bpos.y) > maxHdist) return;
	if (Math::Abs(apos.z - bpos.z) > maxdist) return;

	ThingShape sa = a->getShape();
	ThingShape sb = b->getShape();

	// for cube intersection
	if (sa == CUBE || sb == CUBE)
	{
		// note, their AABB already intersect, so we're done
		collide(a,b);
	}

	// for cylidner intersection
	else if (sa == CYLINDER || sb == CYLINDER)
	{
		// if they are too far apart in the x/z plane, return
		if (a->cylinderDistance(b) > maxdist) return;

		// otherwise collide
		collide(a,b);
	}

	// for sphere intersection
	else //if (sa == SPHERE || sb == SPHERE)
	{
		// if they are too far apart in the x/z plane, return
		if (a->sphereDistance(b) > maxdist) return;

		// otherwise collide
		collide(a,b);
	}
}

//----------------------------------------------------------------------------

void Physics::collide(Thing* a, Thing* b)
{
	// they collide
	a->collided(b);
	b->collided(a);
}

//----------------------------------------------------------------------------

void Physics::setWorldSize(int size)
{
	mWorldSize = size;
}

//----------------------------------------------------------------------------

int Physics::addTeam(int wizardUID)
{
	mTeams.push_back(new Team(wizardUID));
	return (int)mTeams.size()-1;
}

//----------------------------------------------------------------------------

Team* Physics::getTeam(int index)
{
	return mTeams[index];
}

//----------------------------------------------------------------------------

void Physics::clearTeams()
{
	while (mTeams.size() > 0)
	{
		delete mTeams[mTeams.size()-1];
		mTeams.erase(mTeams.end());
	}
}

//----------------------------------------------------------------------------

Physics& Physics::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Physics();
	}
    return Singleton<Physics>::getSingleton();
}

//----------------------------------------------------------------------------

Physics::~Physics()
{
	clear();

	clearTeams();
}

//----------------------------------------------------------------------------

}
