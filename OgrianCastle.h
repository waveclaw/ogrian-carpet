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
OgrianCastle.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a castle

/*------------------------------------*/

#ifndef __OgrianCastle_H__
#define __OgrianCastle_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianModel.h"
#include "OgrianConst.h"
#include "OgrianMultiplayer.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianCraneThing.h"
#include "OgrianPortalThing.h"

#define NUM_BLOCKS 9

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class CastleBlockThing : public DamageableThing
{
public:
	CastleBlockThing(DamageableThing* castle, Vector3 pos, Real width, Real height);

	// set how far up this block should go to
	virtual void setPercentage(Real per);

	// rise smoothly
	virtual void move(Real time);

	// send damage to our host castle
	virtual void damage(int amount, int sourceTeamNum);

	// get how far up we are
	virtual Real getCurrentLevel();

	virtual ThingType getType()	{ return CASTLETURRETTHING; }
	
	virtual bool isBuilding() { return true; }
	
private:
	DamageableThing* mCastle;
	Real mTargetY;
	Real mGroundY;
	Real mPercentage;
};

/////////////////////////////////////////////////////////////////////////////
class CastleTurretThing : public CastleBlockThing
{
public:
	CastleTurretThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0));

	virtual void destroy();

	virtual ThingType getType()	{ return CASTLETURRETTHING; }
	
	// set how far up we are
	virtual void setPercentage(Real per);

private:
	CraneThing* mCrane;

};

/////////////////////////////////////////////////////////////////////////////
class CastleKeepThing : public CastleBlockThing
{
public:
	CastleKeepThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0));

	virtual void destroy();

	virtual ThingType getType()	{ return CASTLEKEEPTHING; }
	
	// set how far up we are
	virtual void setPercentage(Real per);

private:
	CraneThing* mCrane;

};

/////////////////////////////////////////////////////////////////////////////

class CastleBeaconThing : public Thing
{
public:
	CastleBeaconThing()
		: Thing("Ogrian/CastleBeacon", SPRITE, "CastleBeacon", true, CONR("CASTLE_BEACON_WIDTH"), Vector3(0,0,0), SPHERE)
	{
		setHeight(CONR("CASTLE_BEACON_HEIGHT"));
	}
	
	virtual ThingType getType()	{ return EFFECT; }
};


/////////////////////////////////////////////////////////////////////////////
// only used by clients
class CastleFlagThing : public Thing
{
public:
	CastleFlagThing();

	virtual void setColour(ColourValue& colour)
	{
		Thing::setColour(colour);
		mBeacon->setColour(colour);
	}

	virtual void setPosition(Vector3 pos)
	{
		Thing::setPosition(pos);

		Vector3 bpos = getPosition();
		bpos.y += CONR("CASTLE_BEACON_ALTITUDE");
		mBeacon->setPosition(bpos);
	}

private:
	CastleBeaconThing* mBeacon;
};

/////////////////////////////////////////////////////////////////////////////
class Castle : public DamageableThing
{
public:
	Castle(int teamNum, Vector3 pos);
	virtual ~Castle();

	// set the amount of mana this castle contains
	virtual void setMana(int amount);

	// get the amount of mana this castle contains
	virtual int getMana();

	// add to the amount of mana this castle contains
	virtual void addMana(int amount);

	// drop this amount of mana
	virtual void dropMana(int amount);

	// take damage
	virtual void damage(int amount, int sourceTeamNum);

	// set the health
	virtual void setHealth(int health);

	// destruction
	virtual void destroy();

	// is this castle rubble yet. If the wizard dies while his castle is rubble, he is ghosted 
	virtual bool isRubble();

	// the castle doesn't move, but it does do stuff every frame
	virtual void move(Real time);

	virtual ThingType getType()	{ return CASTLEFLAGTHING; }

private:
	int mMana;
	Real mLevel;
	bool mRubble;

	// an array of all the turrets and the keep
	CastleBlockThing* mBlocks[NUM_BLOCKS];

	// our beacon
	CastleBeaconThing* mBeacon;

	// our portal
	PortalThing* mPortal;

	// set the castle level
	void setLevel(Real level);

	// make a turret
	CastleTurretThing* newCastleTurret(int level);

	// set the spells for this castle's wizard
	void setSpells(int level);
};

}

#endif