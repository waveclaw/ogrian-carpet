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
#include "OgrianClock.h"
#include "OgrianTeam.h"

#include "OgrianFireballThing.h"
#include "OgrianClaimSpellThing.h"
#include "OgrianBuildSpellThing.h"
#include "OgrianManaThing.h"
#include "OgrianCameraThing.h"
#include "OgrianFoliageThing.h"
#include "OgrianTowerThing.h"
#include "OgrianPortalThing.h"
#include "OgrianHutThing.h"
#include "OgrianSummonSpellThing.h"
#include "OgrianSentinelThing.h"
#include "OgrianTickThing.h"
#include "OgrianGnomeThing.h"
#include "OgrianAlbatrossThing.h"
#include "OgrianTeleportSpellThing.h"

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
	mLastI = 0;
}

//----------------------------------------------------------------------------

void Physics::frame(Real time)
{
	// move all particles
	moveAll(time);

	if (!Multiplayer::getSingleton().wasClient())
	{
		// clients dont do normal collision checking
		collisionCheck();
	}
	else if (Multiplayer::getSingleton().isClient())
	{
		// check the camera against the buildings
		clientCollisionCheck();
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

void Physics::clientCollisionCheck()
{

}

//----------------------------------------------------------------------------

void Physics::clientFrame(Real time)
{
	CameraThing* cam = static_cast<CameraThing*>(Renderer::getSingleton().getCameraThing());

	// do nothing if we dont have a camera thing yet
	if (cam == 0) return;

	// do nothing if we don't have a postive uid for the camera
	if (cam->getUID() < 1) return;

	if (cam->lastUpdateTime() + CONT("THING_UPDATE_PERIOD")/2
		< Clock::getSingleton().getTime())
	{
		// notify the server of our camerathing
		BitStream bs;
		cam->generateBitStream(bs);
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
		WizardThing* wiz = static_cast<WizardThing*>(getThing(player.wizardUID));

		// notify clients of all things
		for (int i=0; i<(int)mAllThings.size(); i++)
		{
			Thing* thing = mAllThings[i];

			if (thing->getUpdateType() == CONTINUOUS)
			{
				if (thing->lastUpdateTime() + CONT("THING_UPDATE_PERIOD")
						< Clock::getSingleton().getTime() // only send periodically
					&& thing->getUID() != player.wizardUID // dont send their own wizard to them
					)
				{
					BitStream bs;
					thing->generateBitStream(bs);
					Multiplayer::getSingleton().serverSend(&bs, player.id, false); // send unreliably
				}
			}
			else if (thing->getUpdateType() == PERIODIC)
			{
				if (thing->getUpdateFlag())
				{
					BitStream bs;
					thing->generateBitStream(bs);
					Multiplayer::getSingleton().serverSend(&bs, player.id);
				}
			}
		}
	}

	// clear all the update flags
	for (int i=0; i<(int)mAllThings.size(); i++)
	{
		Thing* thing = mAllThings[i];
		if (thing->getUpdateType() != CONTINUOUS)
			thing->clearUpdateFlag();
	}
}

//----------------------------------------------------------------------------

void Physics::syncClient(PlayerInfo player)
{
	for (int i=0; i<(int)mAllThings.size(); i++)
	{
		Thing* thing = mAllThings[i];

		if (thing->getUpdateType() == PERIODIC)
		{
			BitStream bs;
			thing->generateBitStream(bs);
			Multiplayer::getSingleton().serverSend(&bs, player.id);
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
		BitStream bitstream((const char*)packet->data, packet->length, true);
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

			if (thing == 0) return true;

			// log it
			//LogManager::getSingleton().logMessage(String("Making New Thing for client: #") << uid);

			// send the bitstream to the thing
			thing->interpretBitStream(bitstream);

			// add it to physics
			clientAddThing(thing, uid);

			if (getThing(uid) == 0) 
			{
				std::ostringstream num("");
				num << uid;
				LogManager::getSingleton().logMessage(String("Add Failure on thing #") + num.str());
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
		BitStream bitstream((const char*)packet->data, packet->length, true);
		int id, uid;

		bitstream.Read(id);
		bitstream.Read(uid);
		bitstream.ResetReadPointer();

		Thing* thing = getThing(uid);

		if (thing != 0)
		{
			// update it
			thing->interpretBitStream(bitstream);

			// destroy it
			thing->destroy();
		}
		else 
		{
			std::ostringstream num("");
			num << uid;
			LogManager::getSingleton().logMessage(String("Remove Failure on thing #") + num.str());
			//listThings();
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
		BitStream bitstream((const char*)packet->data, packet->length, true);
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
				std::ostringstream num("");
				num << uid;
				LogManager::getSingleton().logMessage(String("wizardthing not found: #") + num.str());
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

			if (thing == 0) return true;

			// send the bitstream to the thing
			thing->interpretBitStream(bitstream);

			// add it to the physics
			addThing(thing);

			if (thing->isAlive())
			{
				// send it to the clients immediately
				BitStream bs;
				thing->generateBitStream(bs);
				Multiplayer::getSingleton().serverSendAll(&bs, false);
			}
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

		case WIZARDTHING: 
		case CAMERATHING: return new WizardThing();

		case HUTTHING: return new HutThing();
		case HUTBALLTHING: return new HutBallThing(0);

		case TOWERTHING: return new TowerThing(0);
		case TOWERBALLTHING: return new TowerBallThing(0);

		case PORTALTHING: return new PortalThing();

		case CASTLEKEEPTHING: return new CastleKeepThing(0);
		case CASTLETURRETTHING: return new CastleTurretThing(0);
		case CASTLEFLAGTHING: return new CastleFlagThing();
		case CASTLEHEARTTHING: return new CastleHeartThing(0);

		case CLAIMTHING: return new ClaimSpellThing(teamNum);

		case BUILDTHING: return new BuildSpellThing(teamNum);

		case SUMMONTHING: return new SummonSpellThing(teamNum);
		case SENTINELSUMMONTHING: return new SentinelSummonSpellThing(teamNum);
		case TICKSUMMONTHING: return new TickSummonSpellThing(teamNum);
		case GNOMESUMMONTHING: return new GnomeSummonSpellThing(teamNum);

		case CRANETHING: return new CraneThing(teamNum);
		case ALBATROSSTHING: return new AlbatrossThing(teamNum);
		case SENTINELTHING: return new SentinelThing(teamNum);
		case TICKTHING: return new TickThing(teamNum);
		case GNOMETHING: return new GnomeThing(teamNum);

		case TELEPORTSPELLTHING: return new TeleportSpellThing(teamNum);

		default:
			std::ostringstream num("");
			num << type;
			LogManager::getSingleton().logMessage(String("ERROR: Thing Unknown: ") + num.str());
			return 0;
	}
}

//----------------------------------------------------------------------------

void Physics::listThings()
{
	LogManager::getSingleton().logMessage("All Things:");

	for (int i=0; i<(int)mAllThings.size(); i++)
	{
		LogManager::getSingleton().logMessage(String(" thing: ") + mAllThings[i]->getString());
	}
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

Thing* Physics::getThingByIndex(int index)
{
	if (mAllThings.size() == 0) return 0;

	return mAllThings[index];
}

//----------------------------------------------------------------------------

// checks to see if a thing is in the list mAllThings
bool Physics::containsThing(Thing* thing)
{
	for (int i=0; i<(int)mAllThings.size(); i++)
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
	if (Multiplayer::getSingleton().isClient() && thing->getType() != CAMERATHING)
	{
		if (thing->getUpdateType() != NEVER)
		{
			// send a message to the server telling it about the new thing
			BitStream bs;
			thing->generateBitStream(bs);
			Multiplayer::getSingleton().clientSend(&bs);

			// discard the thing
			_delete(thing);
		}
	}
	else // if its a server or singleplayer
	{
		// make sure it's not too big
		if (thing->getWidth() / 2.0 > mWorldSize / PHYSICS_GRID_SIZE)
		{
			Except( Exception::ERR_INTERNAL_ERROR, "Error: Thing Too Big for Grid. Make the world bigger, the thing smaller, or the grid coarser.",
					"Physics::addThing" );
		}

		// add to grid
		Vector3 pos = thing->getPosition();
		_addThing(thing, getGridU(pos), getGridV(pos));

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

	// add buildings to mBuildings
	if (thing->isBuilding())
	{
		mBuildings.push_back(thing);
	}

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
		LogManager::getSingleton().logMessage("ERROR: THING OUTSIDE WORLD");
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
		for (int i=0; i<(int)mThingGrid[grid_u][grid_v].size(); i++)
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
		LogManager::getSingleton().logMessage(String("Error Removing Thing, not found in grid: (") 
			+ /*grid_u + "," + grid_v +*/ ") " + thing->getString());
		return false;
	}

	return false;
}

//----------------------------------------------------------------------------

void Physics::deleteEffect(Thing* thing)
{
	// remove it from mEffects
	for (int i=0; i<(int)mEffects.size(); i++)
	{
		if (mEffects[i] == thing)
		{
			// erase it
			mEffects.erase(mEffects.begin()+i);
			break;
		}
	}

	_delete(thing);
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
		thing->generateBitStream(bs,ID_REMOVE_THING);

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
	bool removed = true;

	// it's not in the grid if its a client
	if (!Multiplayer::getSingleton().isClient())
		removed = _removeThing(thing, getGridU(pos), getGridV(pos));

	// remove it from allThings
	//size_t s = mAllThings.size();
	for (int i=0; i<(int)mAllThings.size(); i++)
	{
		if (mAllThings[i] == thing)
		{
			// erase it
			mAllThings.erase(mAllThings.begin()+i);
			break;
		}
	}

	// remove it from buildigns
	for (int i=0; i<(int)mBuildings.size(); i++)
	{
		if (mBuildings[i] == thing)
		{
			// erase it
			mBuildings.erase(mBuildings.begin()+i);
			break;
		}
	}

	// delete it
	if (removed)
		_delete(thing);
	else LogManager::getSingleton().logMessage(String("ERROR: THING NOT DELETED: ") + thing->getString());
}

//----------------------------------------------------------------------------

void Physics::updateThing(Thing* thing, Vector3 oldPos, Vector3 newPos)
{
	if (Multiplayer::getSingleton().isClient()) return;

	int from_u = getGridU(oldPos);
	int from_v = getGridV(oldPos);
	int to_u = getGridU(newPos);
	int to_v = getGridV(newPos);

	// if it crossed a boundary, move it to the new cell
	if (from_u != to_u || from_v != to_v)
	{
		_removeThing(thing, from_u, from_v);
		_addThing(thing, to_u, to_v);
	}
}

//----------------------------------------------------------------------------

int Physics::getGridU(Vector3 pos)
{
	Real c = CONR("COASTLINE");
	return ((pos.x+c)/(mWorldSize+2*c)) * PHYSICS_GRID_SIZE;
}

//----------------------------------------------------------------------------

int Physics::getGridV(Vector3 pos)
{
	Real c = CONR("COASTLINE");
	return ((pos.z+c)/(mWorldSize+2*c)) * PHYSICS_GRID_SIZE;
}

//----------------------------------------------------------------------------

void Physics::_delete(Thing* thing)
{
	//// confirm that it is not in physics
	//if (getThing(thing->getUID()))
	//	((Thing*)0)->isAlive(); // stop!

	//for (int i=0; i<(int)mBuildings.size(); i++)
	//	if (mBuildings[i] == thing)
	//		((Thing*)0)->isAlive(); // stop!

	//Vector3 pos = thing->getPosition();
	//int x = getGridU(pos);
	//int y = getGridV(pos);
	//for (int i=0; i<(int)mThingGrid[x][y].size(); i++)
	//	if (thing == mThingGrid[x][y][i])
	//		((Thing*)0)->isAlive(); // stop!

	thing->setDeleteFlag();
	delete thing;
}

//----------------------------------------------------------------------------

// remove and delete all things
void Physics::reset()
{
	if (!Multiplayer::getSingleton().isClient())
	{
		// clear the grid
		for (int i=0; i<PHYSICS_GRID_SIZE; i++)
			for (int j=0; j<PHYSICS_GRID_SIZE; j++)
				while (!mThingGrid[i][j].empty())
					mThingGrid[i][j].pop_back();
	}

	// clear mBuildings
	mBuildings.clear();

	// delete each thing from mAllThings
	while(!mAllThings.empty())
	{
		Thing* thing = mAllThings[mAllThings.size()-1];
		if (thing->getType() != CAMERATHING)
		{
			mAllThings.pop_back();
			_delete(thing);
		}
		else mAllThings.pop_back();
	}

	// delete each effect from mEffects
	while (!mEffects.empty())
	{
		_delete(mEffects[mEffects.size()-1]);
		mEffects.pop_back();
	}

	// clear the teams
	mTeams.clear();

}

//----------------------------------------------------------------------------

// move all things, delete the ones not alive
void Physics::moveAll(Real time)
{
	// move all things
	for (int i=0; i<(int)mAllThings.size(); i++)
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
	for (int i=0; i<(int)mEffects.size(); i++)
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
	unsigned int i,j,t,u, starti, stopi;
	i = j = t = u = 0;

	if (mLastI >= PHYSICS_GRID_SIZE-1) mLastI = 0;
	starti = mLastI;

	// divide up the grid and be sure to round up
	stopi = 1 + mLastI + PHYSICS_GRID_SIZE / CONR("PHYSICS_GRID_SPLIT");
	if (stopi >= PHYSICS_GRID_SIZE) stopi = PHYSICS_GRID_SIZE-1;

	mLastI = stopi;

	// check the grid for collisions among itself
	for (i=starti; i<=stopi; i++)
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
}

//----------------------------------------------------------------------------

void Physics::pairCollisionCheck(Thing* a, Thing* b)
{
	// dont collide with dead things
	if (!a->isAlive() || !b->isAlive()) return;

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

int Physics::newTeam(ColourValue colour, int wizardUID)
{
	mTeams.push_back(new Team((int)mTeams.size(), colour));

	if (wizardUID >= 0) mTeams[mTeams.size()-1]->setWizardUID(wizardUID);

	return (int)mTeams.size()-1;
}

//----------------------------------------------------------------------------

int Physics::numTeams()
{
	return (int)mTeams.size();
}

//----------------------------------------------------------------------------

Team* Physics::getTeam(int index)
{
	if (index < 0 || index >= (int)mTeams.size())
	{
		return 0;
	}

	Team* team = mTeams[index];
	if (team->getTeamNum() == index)
		return mTeams[index];

	return 0;
}

//----------------------------------------------------------------------------

void Physics::removeTeam(int teamNum)
{
	if (teamNum < 0 || teamNum >= (int)mTeams.size())
	{
		delete mTeams[teamNum];
		mTeams[teamNum] = 0;
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
	reset();
}

//----------------------------------------------------------------------------

}
