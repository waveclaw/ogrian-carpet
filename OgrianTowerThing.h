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
#include "OgrianClaimSpellThing.h"
#include "OgrianMultiplayer.h"
#include "OgrianConst.h"
#include "OgrianModel.h"
#include "OgrianCraneThing.h"
#include "OgrianPhysics.h"
#include "OgrianManaThing.h"

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
		mLastCastTime = 0;

		// set the mesh
		setMaterial("Ogrian/Tower");
		static_cast<Model*>(getVisRep())->setMesh("tower.mesh",
			CONR("CASTLETOWER_MESH_SCALE"), CONR("CASTLETOWER_MESH_RATIO"));

		// set the height
		setHeight(CONR("TOWER_HEIGHT"));

		// start at zero and rise
		setPosY(getGroundY() - CONR("TOWER_OFFSET"));
		mTargetY = getPosY();

		// set the health
		setHealth(CONI("TOWER_HEALTH"));

		// set the team
		setTeamNum(teamNum);

		// make the crane flock
		if (!Multiplayer::getSingleton().isClient())
		{
			Vector3 pos = getPosition();
			pos.y += getHeight() - CONR("TOWER_OFFSET");
			for (int i=0; i<CONI("TOWER_NUM_CRANES"); i++)
			{
				CraneThing* crane = new CraneThing(teamNum,pos);
				mCranes.push_back(crane);
				Physics::getSingleton().addThing(crane);
			}
		}
	}

	virtual void destroy()
	{
		// drop a manathing
		ManaThing* mana = new ManaThing(CONI("TOWER_COST"), getPosition());
		mana->setTeamNum(getTeamNum());
		Physics::getSingleton().addThing(mana);;

		// remove the cranes
		for (int i=0; i<(int)mCranes.size(); i++)
			mCranes[i]->destroy();

		mCranes.clear();

		DamageableThing::destroy();
	}

	// set how far up this block should go to
	virtual void setPercentage(Real per)
	{
		if (per >= 1) per = 1;
		if (per <= 0) per = 0;
		Real newTargetY = getGroundY() - CONR("TOWER_OFFSET") - getHeight()/2 + getHeight()*per;

		if (newTargetY == mTargetY) return;

		mTargetY = newTargetY;

		if (mTargetY > getPosY())
		{
			setVelY(CONR("TOWER_RISE_SPEED"));
		}
		else
		{
			setVelY(0-CONR("TOWER_RISE_SPEED"));
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

	virtual void think()
	{
		// if its time, claim a nearby mana
		if (Time::getSingleton().getTime() > mLastCastTime + CONR("TOWER_CAST_PERIOD")*1000)
		{
			mLastCastTime = Time::getSingleton().getTime();

			// cast a mana at the nearest manathing in range
			Thing* target = 0;
			Real bestDist = CONR("TOWER_RANGE");;
			for (int i=0; i<Physics::getSingleton().numThings(); i++)
			{
				Thing* candidate = Physics::getSingleton().getThingByIndex(i);
				if (candidate 
					&& candidate->getType() == MANATHING 
					&& cylinderDistance(candidate) < bestDist
					&& candidate->getTeamNum() != getTeamNum()
					&& candidate->isAlive() )
				{
					target = candidate;
					bestDist = cylinderDistance(candidate);
				}
			}

			if (target) 
			{
				Vector3 startPos = getPosition();
				startPos.y += getHeight()/2 - CONR("TOWER_OFFSET");

				Vector3 vel = target->getPosition() - startPos;
				vel.normalise();
				vel *= CONR("CLAIMSPELL_SPEED");

				Physics::getSingleton().addThing(new ClaimSpellThing(getTeamNum(), startPos, 
					vel, CONR("TOWER_CLAIM_LIFETIME")));
			}
		}
	}

	virtual void setHealth(int health)
	{
		DamageableThing::setHealth(health);

		if (health < 0) health = 0;

		setPercentage((health + CONR("TOWER_RUBBLE"))
			/ (CONR("TOWER_HEALTH") + CONR("TOWER_RUBBLE")));
	}

	virtual void die()
	{
		destroy();
	}


	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return TOWER; }
	
private:
	Real mTargetY;

	unsigned long mLastCastTime;

	std::vector<CraneThing*> mCranes;
};

}
#endif