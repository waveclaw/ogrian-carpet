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

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class CastleTowerThing : public DamageableThing
{
public:
	CastleTowerThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: DamageableThing("Ogrian/Tower", MODEL, "CastleTower", false, CONR("CASTLE_WIDTH"), pos, CUBE)
	{
		mCastle = castle;

		static_cast<Model*>(getVisRep())->setMesh("tower.mesh",
			CONR("CASTLETOWER_MESH_SCALE"), CONR("CASTLETOWER_MESH_RATIO"));

		setHeight(CONR("CASTLETOWER_HEIGHT"));
		setPercentage(1);
		setHealth(CONI("CASTLE_BASE_HEALTH"));
	}

	virtual void setPercentage(Real per)
	{
		setPosY(getGroundY()+getHeight()*CONR("CASTLETOWER_OFFSET")*per);
	}

	virtual void damage(int amount, int sourceTeamNum)
	{
		mCastle->damage(amount, sourceTeamNum);
	}

	virtual ThingType getType()
	{
		return CASTLETOWER;
	}
	
private:
	DamageableThing* mCastle;
};

/////////////////////////////////////////////////////////////////////////////
class CastleWallThing : public DamageableThing
{
public:
	CastleWallThing(DamageableThing* castle, Vector3 pos=Vector3(0,0,0)) 
		: DamageableThing("Ogrian/Wall", MODEL, "CastleWall", false, CONR("CASTLE_WIDTH"), pos, CUBE)
	{
		mCastle = castle;

		static_cast<Model*>(getVisRep())->setMesh("wall.mesh",
			CONR("CASTLEWALL_MESH_SCALE"), CONR("CASTLEWALL_MESH_RATIO"));

		setHeight(CONR("CASTLEWALL_HEIGHT"));
		setPercentage(1);
		setHealth(CONI("CASTLE_BASE_HEALTH"));
	}

	virtual void setPercentage(Real per)
	{
		setPosY(getGroundY()+getHeight()*CONR("CASTLEWALL_OFFSET")*per);
	}

	virtual void damage(int amount, int sourceTeamNum)
	{
		mCastle->damage(amount, sourceTeamNum);
	}

	virtual ThingType getType()
	{
		return CASTLEWALL;
	}

private:
	DamageableThing* mCastle;
};

/////////////////////////////////////////////////////////////////////////////
class Castle : public DamageableThing
{
public:
	Castle(int teamNum, Vector3 pos);

	// set the amount of mana this castle contains
	void setMana(int amount);

	// get the amount of mana this castle contains
	int getMana();

	// add to the amount of mana this castle contains
	void addMana(int amount);

	// call this every frame
	void frame(Real time);

private:
	int mTeamNum;
	Vector3 mPos;
	int mMana;

	// an array of all the towers and walls for convenient looping
	DamageableThing* mBlocks[49];

	CastleTowerThing* mCenterTower;
	
	CastleTowerThing* mCornerTowerNE;
	CastleTowerThing* mCornerTowerSE;
	CastleTowerThing* mCornerTowerSW;
	CastleTowerThing* mCornerTowerNW;

	CastleWallThing* mInnerWallN1;
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

	CastleTowerThing* mFarCornerTowerN;
	CastleTowerThing* mFarCornerTowerS;
	CastleTowerThing* mFarCornerTowerE;
	CastleTowerThing* mFarCornerTowerW;
	CastleTowerThing* mFarCornerTowerNE;
	CastleTowerThing* mFarCornerTowerSE;
	CastleTowerThing* mFarCornerTowerSW;
	CastleTowerThing* mFarCornerTowerNW;

	CastleWallThing* mOuterWallN1;
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
};

}

#endif