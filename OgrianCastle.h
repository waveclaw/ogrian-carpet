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
	CastleBlockThing(DamageableThing* castle, Vector3 pos) 
		: DamageableThing("Ogrian/Tower", MODEL, "CastleTower", false, CONR("CASTLE_WIDTH"), pos, CUBE)
	{
		mCastle = castle;

		// start at zero
		setPosY(getGroundY() - CONR("CASTLE_OFFSET") - getHeight()*2);
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
		Real newTargetY = getGroundY() - CONR("CASTLE_OFFSET") - getHeight()/2 + getHeight()*per;

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

	virtual bool isBuilding() { return true; }
	
private:
	DamageableThing* mCastle;

	Real mTargetY;
};

/////////////////////////////////////////////////////////////////////////////
class CastleTowerThing : public CastleBlockThing
{
public:
	CastleTowerThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos)
	{
		mCrane = 0;

		setMaterial("Ogrian/Tower");
		static_cast<Model*>(getVisRep())->setMesh("tower.mesh",
			CONR("CASTLETOWER_MESH_SCALE"), CONR("CASTLETOWER_MESH_RATIO"));

		setHeight(CONR("CASTLETOWER_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLETOWER; }
	
	virtual void setPercentage(Real per);

private:
	CraneThing* mCrane;

};

/////////////////////////////////////////////////////////////////////////////
class CastleWallThing : public CastleBlockThing
{
public:
	CastleWallThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: CastleBlockThing(castle, pos)
	{
		setMaterial("Ogrian/Wall");
		static_cast<Model*>(getVisRep())->setMesh("wall.mesh",
			CONR("CASTLEWALL_MESH_SCALE"), CONR("CASTLEWALL_MESH_RATIO"));

		setHeight(CONR("CASTLEWALL_HEIGHT"));
	}

	virtual ThingType getType()	{ return CASTLEWALL; }
};


/////////////////////////////////////////////////////////////////////////////
// only used by clients
class CastleFlagThing : public Thing
{
public:
	CastleFlagThing()
		: Thing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), Vector3(0,0,0), SPHERE)
	{
	}
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

	virtual ThingType getType()
	{
		return CASTLEFLAG;
	}

private:
	int mMana;

	Real mLevel;

	bool mRubble;

	// a vector holding all the claimed manathings
	std::vector<Thing*> mManaThings;

	// an array of the baloons
	BaloonThing* mBaloons[NUM_BALOONS];

	// an array of all the towers and walls for convenient looping
	CastleBlockThing* mBlocks[49];

	CastleTowerThing* mCenterTower; // 0
	
	CastleTowerThing* mCornerTowerNE; // 1
	CastleTowerThing* mCornerTowerSE;
	CastleTowerThing* mCornerTowerSW;
	CastleTowerThing* mCornerTowerNW;

	CastleWallThing* mInnerWallN1; // 5
	CastleWallThing* mInnerWallN2;
	CastleWallThing* mInnerWallN3;
	CastleWallThing* mInnerWallE1;
	CastleWallThing* mInnerWallE2;
	CastleWallThing* mInnerWallE3;
	CastleWallThing* mInnerWallS1;
	CastleWallThing* mInnerWallS2;
	CastleWallThing* mInnerWallS3;
	CastleWallThing* mInnerWallW1;
	CastleWallThing* mInnerWallW2;
	CastleWallThing* mInnerWallW3;

	CastleTowerThing* mFarCornerTowerN; // 17
	CastleTowerThing* mFarCornerTowerS;
	CastleTowerThing* mFarCornerTowerE;
	CastleTowerThing* mFarCornerTowerW;
	CastleTowerThing* mFarCornerTowerNE;
	CastleTowerThing* mFarCornerTowerSE;
	CastleTowerThing* mFarCornerTowerSW;
	CastleTowerThing* mFarCornerTowerNW;

	CastleWallThing* mOuterWallN1; // 25
	CastleWallThing* mOuterWallN2;
	CastleWallThing* mOuterWallN3;
	CastleWallThing* mOuterWallN5;
	CastleWallThing* mOuterWallN6;
	CastleWallThing* mOuterWallN7;
	CastleWallThing* mOuterWallE1;
	CastleWallThing* mOuterWallE2;
	CastleWallThing* mOuterWallE3;
	CastleWallThing* mOuterWallE5;
	CastleWallThing* mOuterWallE6;
	CastleWallThing* mOuterWallE7;
	CastleWallThing* mOuterWallS1;
	CastleWallThing* mOuterWallS2;
	CastleWallThing* mOuterWallS3;
	CastleWallThing* mOuterWallS5;
	CastleWallThing* mOuterWallS6;
	CastleWallThing* mOuterWallS7;
	CastleWallThing* mOuterWallW1;
	CastleWallThing* mOuterWallW2;
	CastleWallThing* mOuterWallW3;
	CastleWallThing* mOuterWallW5;
	CastleWallThing* mOuterWallW6;
	CastleWallThing* mOuterWallW7;

	// the current number of baloons
	int mNumBaloons;

	// set the castle level
	void setLevel(Real level);

	// set the number of baloons
	void setNumBaloons(int num);

	// genearate a target for a baloon
	Thing* generateTarget(BaloonThing* baloon);
};

}

#endif