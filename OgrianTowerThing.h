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
OgrianTower.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a tower to stand on. it also claims nearby mana and has
a flock of cranes

/*------------------------------------*/

#ifndef __OgrianTower_H__
#define __OgrianTower_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianConst.h"
#include "OgrianCraneThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class TowerBeaconThing : public Thing
{
public:
	TowerBeaconThing()
		: Thing("Ogrian/TowerBeacon", SPRITE, "TowerBeacon", true, CONR("TOWER_BEACON_WIDTH"), Vector3(0,0,0), SPHERE)
	{
		setHeight(CONR("TOWER_BEACON_HEIGHT"));
	}
	
	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////

class TowerBallThing : public Thing
{
public:
	TowerBallThing(int team);

	// heal our wizard
	virtual void think();
	virtual void collided(Thing* e);
	
	virtual ThingType getType()	{ return TOWERBALLTHING; }

private:
	bool mHeal;
};

/////////////////////////////////////////////////////////////////////////////
class TowerThing : public DamageableThing
{
public:
	TowerThing(int teamNum, Vector3 pos=Vector3(0,0,0));

	// make the beacon the same colour as us
	virtual void setColour(ColourValue& colour);

	virtual void setPosition(Vector3 pos);

	// take the tower down without dropping mana
	virtual void unbuild();

	virtual void die();

	virtual void destroy();

	// set how far up this block should go to
	virtual void setPercentage(Real per);

	// grow and shrink smoothly
	virtual void move(Real time);

	// claim nearby mana
	virtual void think();	

	// set the percentage to match the health
	virtual void setHealth(int health);

	// exit unbuild mode if damaged
	virtual void damage(int amount, int source);

	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return TOWERTHING; }
	
private:
	Real mTargetY;
	Real mGroundY;

	bool mUnbuildMode;

	ColourValue mColour;

	Time mLastCastTime;

	std::vector<CraneThing*> mCranes;

	TowerBeaconThing* mBeacon;
	TowerBallThing* mBall;
};

}
#endif