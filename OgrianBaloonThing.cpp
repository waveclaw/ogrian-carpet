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
OgrianBaloonThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the Baloon collects mana and brings it back to the castle
/*------------------------------------*/

#include "OgrianCastle.h"
#include "OgrianPhysics.h"
#include "OgrianBaloonThing.h"
#include "OgrianManaThing.h"

#define BAL_STATE_WAIT 0
#define BAL_STATE_ASCEND 1
#define BAL_STATE_DESCEND 2
#define BAL_STATE_TRAVEL 3

namespace Ogrian
{

//----------------------------------------------------------------------------

BaloonThing::BaloonThing(int team, Vector3 pos, unsigned int amount) 
	: Thing("Ogrian/Baloon", SPRITE, "ManaThing", false, CONR("BALOON_SCALE"), pos, SPHERE)
{
	mState = -1;
	setTeamNum(team);
	setTarget(0);
	setUpdateType(CONTINUOUS);
	unload();
}

//----------------------------------------------------------------------------

// change the colour to reflect team ownership
void BaloonThing::setTeamNum(int teamNum)
{
	Team* team = Physics::getSingleton().getTeam(teamNum);

	setColour(team->getColour());

	Thing::setTeamNum(teamNum);
}

//----------------------------------------------------------------------------

void BaloonThing::setTarget(Thing* target)
{
	mTarget = target;
	
	if (mTarget == 0)
	{
		setStateWait();
	}
	else
	{
		setStateAscend();
		mNeedOrders = false;
	}
}

//----------------------------------------------------------------------------

Thing* BaloonThing::getTarget()
{
	return mTarget;
}

//----------------------------------------------------------------------------

void BaloonThing::move(Real time)
{
	Thing::move(time);

	if (mTarget && mTarget->getTeamNum() == getTeamNum())
	{
		mNeedOrders = false;

		//  if it's ascending and has gotten high enough
		if (mState == BAL_STATE_ASCEND && getPosY() > getGroundY() + CONR("BALOON_ALTITUDE"))
		{
			// change to travel mode
			setStateTravel();
		}
		
		// if its travelling
		if (mState == BAL_STATE_TRAVEL)
		{
			// move towards the target
			Vector3 dir = mTarget->getPosition() - getPosition();
			dir.y = 0;
			dir.normalise();
			setVelocity(dir * CONR("BALOON_SPEED"));

			// stay above the minimum altitude
			if (getPosY() < getGroundY() + CONR("BALOON_ALTITUDE"))
				setPosY(getGroundY() + CONR("BALOON_ALTITUDE"));

			// if it is close enough to its target
			if (cylinderDistance(mTarget) < getWidth())
				setStateDescend(); // descend
		}
			
		// if it is descending
		if (mState == BAL_STATE_DESCEND)
		{
			// move down to the target
			Vector3 dir = mTarget->getPosition() - getPosition();
			dir.normalise();
			setVelocity(dir * CONR("BALOON_SPEED"));
		}
	}
	else if (mTarget > 0)
	{
		setTarget(0);
	}
}

//----------------------------------------------------------------------------

void BaloonThing::collided(Thing* thing)
{
	if (thing->getType() == MANATHING && thing == mTarget)
	{
		// pick up the mana
		load(((ManaThing*)mTarget)->getAmount());
		mTarget->destroy();
	}

	if (thing->getType() ==	CASTLEFLAG && thing == mTarget)
	{
		// drop off our mana
		((Castle*)mTarget)->addMana(unload());

		// wait for new orders
		setTarget(0);
	}
}

//----------------------------------------------------------------------------

void BaloonThing::setStateWait()
{
	mState = BAL_STATE_WAIT;
	mNeedOrders = true;
	setVelocity(Vector3(0,0,0));
}

//----------------------------------------------------------------------------

void BaloonThing::setStateAscend()
{
	mState = BAL_STATE_ASCEND;
	setVelocity(Vector3(0,CONR("BALOON_SPEED"),0));
}

//----------------------------------------------------------------------------

void BaloonThing::setStateDescend()
{
	mState = BAL_STATE_DESCEND;
}

//----------------------------------------------------------------------------

void BaloonThing::setStateTravel()
{
	mState = BAL_STATE_TRAVEL;
}

//----------------------------------------------------------------------------

bool BaloonThing::needsOrders()
{
	return mNeedOrders;
}

//----------------------------------------------------------------------------

void BaloonThing::load(int amount)
{
	mAmount += amount;
}

//----------------------------------------------------------------------------

int BaloonThing::unload()
{
	int a = mAmount;
	mAmount = 0;
	return a;
}

//----------------------------------------------------------------------------

int BaloonThing::getAmount()
{
	return mAmount;
}

//----------------------------------------------------------------------------

}