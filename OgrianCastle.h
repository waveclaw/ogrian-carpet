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
#include "OgrianBaloonThing.h"
#include "OgrianCraneThing.h"

#define NUM_BALOONS 5

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class CastleBlockThing : public DamageableThing
{
public:
	CastleBlockThing(DamageableThing* castle, Vector3 pos, Real width=CONR("CASTLE_WIDTH")) 
		: DamageableThing("Ogrian/Tower", MODEL, "CastleTower", false, width, pos, CUBE)
	{
		mCastle = castle;

		// find the ground	
		Real w = getWidth()/2;
		Real ground00 = getGroundY(pos + Vector3(-w,0,-w));
		Real ground01 = getGroundY(pos + Vector3(-w,0, w));
		Real ground10 = getGroundY(pos + Vector3( w,0,-w));
		Real ground11 = getGroundY(pos + Vector3( w,0, w));

		Real ground = ground00;
		if (ground01 < ground) ground = ground01;
		if (ground10 < ground) ground = ground10;
		if (ground11 < ground) ground = ground11;
		mGroundY = ground;

		// start at zero
		setPosY(mGroundY - CONR("CASTLE_OFFSET") - getHeight()*2);
		mTargetY = getPosY();
		setPercentage(0);

		// set the team
		if (castle)
			setTeamNum(castle->getTeamNum());
	}

	// set how far up this block should go to
	virtual void setPercentage(Real per)
	{
		if (per >= 1) per = 1;
		if (per <= 0) per = -0.1;
		Real newTargetY = mGroundY - CONR("CASTLE_OFFSET") - getHeight()/2 + getHeight()*per;

		if (newTargetY == mTargetY) return;

		mTargetY = newTargetY;

		if (mTargetY > getPosY())
		{
			setVelY(CONR("CASTLE_RISE_SPEED"));
		}
		else
		{
			setVelY(0-CONR("CASTLE_RISE_SPEED"));
		}

		setUpdateFlag();
	}

	virtual void move(Real time)
	{
		// if it has reached its target pos.y, stop
		if (!Multiplayer::getSingleton().isClient() && (
			(getVelY() < 0 && mTargetY > getPosY()) ||
			(getVelY() > 0 && mTargetY < getPosY())))
		{
			setPosY(mTargetY);
			setVelY(0);
			setUpdateFlag();
		}

		DamageableThing::move(time);
	}

	virtual void damage(int amount, int sourceTeamNum)
	{
		if (mCastle)
			mCastle->damage(amount, sourceTeamNum);
	}

	virtual ThingType getType()	{ return CASTLETOWER; }
	
	virtual bool isBuilding() { return true; }
	
private:
	DamageableThing* mCastle;

	Real mTargetY;

	Real mGroundY;
};

/////////////////////////////////////////////////////////////////////////////
class CastleTowerThing : public CastleBlockThing
{
public:
	CastleTowerThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos, CONR("CASTLETOWER_WIDTH"))
	{
		mCrane = 0;

		setMaterial("Ogrian/Tower");
		static_cast<Model*>(getVisRep())->setMesh("tower1.mesh",
			CONR("CASTLETOWER_MESH_SCALE"), CONR("CASTLETOWER_MESH_RATIO"));

		setHeight(CONR("CASTLETOWER_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLETOWER; }
	
	virtual void setPercentage(Real per);

private:
	CraneThing* mCrane;

};

/////////////////////////////////////////////////////////////////////////////
class CastleKeepThing : public CastleBlockThing
{
public:
	CastleKeepThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos, CONR("CASTLEKEEP_WIDTH"))
	{
		mCrane = 0;

		setMaterial("Ogrian/Tower");
		static_cast<Model*>(getVisRep())->setMesh("keep.mesh",
			CONR("CASTLEKEEP_MESH_SCALE"), CONR("CASTLEKEEP_MESH_RATIO"));

		setHeight(CONR("CASTLEKEEP_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLEKEEP; }
	
	virtual void setPercentage(Real per);

private:
	CraneThing* mCrane;

};

/////////////////////////////////////////////////////////////////////////////
class CastleWallNSThing : public CastleBlockThing
{
public:
	CastleWallNSThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos)
	{
		setMaterial("Ogrian/Wall");
		static_cast<Model*>(getVisRep())->setMesh("wall2.mesh",
			CONR("CASTLEWALL_MESH_SCALE"), CONR("CASTLEWALL_MESH_RATIO"));

		setHeight(CONR("CASTLEWALL_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLEWALLNS; }
};

/////////////////////////////////////////////////////////////////////////////
class CastleWallEWThing : public CastleBlockThing
{
public:
	CastleWallEWThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos)
	{
		setMaterial("Ogrian/Wall");
		static_cast<Model*>(getVisRep())->setMesh("wall1.mesh",
			CONR("CASTLEWALL_MESH_SCALE"), CONR("CASTLEWALL_MESH_RATIO"));

		setHeight(CONR("CASTLEWALL_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLEWALLEW; }
};


/////////////////////////////////////////////////////////////////////////////

class CastleBeaconThing : public Thing
{
public:
	CastleBeaconThing()
		: Thing("Ogrian/Beacon", SPRITE, "CastleBeacon", true, CONR("CASTLE_BEACON_WIDTH"), Vector3(0,0,0), SPHERE)
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

	// set the amount of mana this castle contains
	virtual void setMana(int amount);

	// get the amount of mana this castle contains
	virtual int getMana();

	// add to the amount of mana this castle contains
	virtual void addMana(int amount);

	// drop this amount of mana
	virtual void dropMana(int amount);

	// notify the castle of a newly claimed manathing
	virtual void claimedManaThing(Thing* mana);

	// notify the castle of a newly unclaimed or destroyed manathing
	virtual void unclaimedManaThing(Thing* mana);

	// take damage
	virtual void damage(int amount, int sourceTeamNum);

	// set the health
	virtual void setHealth(int health);

	// ignore  destruction
	virtual void destroy() {}

	// is this castle rubble yet. If the wizard dies while his castle is rubble, he is ghosted 
	virtual bool isRubble();

	// the castle doesn't move, but it does do stuff every frame
	virtual void move(Real time);

	virtual ThingType getType()	{ return CASTLEFLAG; }

private:
	int mMana;

	Real mLevel;

	bool mRubble;

	CastleBeaconThing* mBeacon;

	// a vector holding all the claimed manathings
	std::vector<Thing*> mManaThings;

	// an array of the baloons
	BaloonThing* mBaloons[NUM_BALOONS];

	// an array of all the towers and walls for convenient looping
	CastleBlockThing* mBlocks[49];

	CastleKeepThing* mCenterTower; // 0
	
	CastleTowerThing* mCornerTowerNE; // 1
	CastleTowerThing* mCornerTowerSE;
	CastleTowerThing* mCornerTowerSW;
	CastleTowerThing* mCornerTowerNW;

	CastleWallEWThing* mInnerWallN1; // 5
	CastleWallEWThing* mInnerWallN2;
	CastleWallEWThing* mInnerWallN3;
	CastleWallNSThing* mInnerWallE1;
	CastleWallNSThing* mInnerWallE2;
	CastleWallNSThing* mInnerWallE3;
	CastleWallEWThing* mInnerWallS1;
	CastleWallEWThing* mInnerWallS2;
	CastleWallEWThing* mInnerWallS3;
	CastleWallNSThing* mInnerWallW1;
	CastleWallNSThing* mInnerWallW2;
	CastleWallNSThing* mInnerWallW3;

	CastleTowerThing* mFarCornerTowerN; // 17
	CastleTowerThing* mFarCornerTowerS;
	CastleTowerThing* mFarCornerTowerE;
	CastleTowerThing* mFarCornerTowerW;
	CastleTowerThing* mFarCornerTowerNE;
	CastleTowerThing* mFarCornerTowerSE;
	CastleTowerThing* mFarCornerTowerSW;
	CastleTowerThing* mFarCornerTowerNW;

	CastleWallEWThing* mOuterWallN1; // 25
	CastleWallEWThing* mOuterWallN2;
	CastleWallEWThing* mOuterWallN3;
	CastleWallEWThing* mOuterWallN5;
	CastleWallEWThing* mOuterWallN6;
	CastleWallEWThing* mOuterWallN7;
	CastleWallNSThing* mOuterWallE1;
	CastleWallNSThing* mOuterWallE2;
	CastleWallNSThing* mOuterWallE3;
	CastleWallNSThing* mOuterWallE5;
	CastleWallNSThing* mOuterWallE6;
	CastleWallNSThing* mOuterWallE7;
	CastleWallEWThing* mOuterWallS1;
	CastleWallEWThing* mOuterWallS2;
	CastleWallEWThing* mOuterWallS3;
	CastleWallEWThing* mOuterWallS5;
	CastleWallEWThing* mOuterWallS6;
	CastleWallEWThing* mOuterWallS7;
	CastleWallNSThing* mOuterWallW1;
	CastleWallNSThing* mOuterWallW2;
	CastleWallNSThing* mOuterWallW3;
	CastleWallNSThing* mOuterWallW5;
	CastleWallNSThing* mOuterWallW6;
	CastleWallNSThing* mOuterWallW7;

	// the current number of baloons
	int mNumBaloons;

	// set the castle level
	void setLevel(Real level);

	// set the number of baloons
	void setNumBaloons(int num);

	// set the spells for this castle's wizard
	void setSpells(int level);

	// genearate a target for a baloon
	Thing* generateTarget(BaloonThing* baloon);

	// make the levels
	void makeLevel1();
	void makeLevel2();
	void makeLevel3();
	void makeLevel4();

	// destroy the levels
	void destroyLevel1();
	void destroyLevel2();
	void destroyLevel3();
	void destroyLevel4();
};

}

#endif