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
#include "OgrianMultiplayer.h"
#include "OgrianConst.h"
#include "OgrianModel.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class TowerThing : public DamageableThing
{
public:
	TowerThing(int teamNum, Vector3 pos=Vector3(0,0,0)) 
		: DamageableThing("Ogrian/Tower", MODEL, "Tower", false, CONR("CASTLE_WIDTH"), pos, CUBE)
	{
		// set the mesh
		setMaterial("Ogrian/Tower");
		static_cast<Model*>(getVisRep())->setMesh("tower.mesh",
			CONR("CASTLETOWER_MESH_SCALE"), CONR("CASTLETOWER_MESH_RATIO"));

		setPosition(pos);

		// set the height
		setHeight(CONR("CASTLETOWER_HEIGHT"));

		// start at zero
		setPosY(getGroundY() - CONR("CASTLE_OFFSET") - getHeight()/2);
		mTargetY = getPosY();
		setPercentage(0);

		// rise up
		setPercentage(1);

		// set the team
		setTeamNum(teamNum);
	}

	// set how far up this block should go to
	virtual void setPercentage(Real per)
	{
		if (per >= 1) per = 1;
		if (per <= 0) per = 0;
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
		DamageableThing::damage(amount, sourceTeamNum);

	}

	virtual bool isBuilding() { return true; }
	
private:
	Real mTargetY;
};

}
#endif