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
OgrianBaloonThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the Baloon collects mana and brings it back to the castle

/*------------------------------------*/


#ifndef __OgrianBaloonThing_H__
#define __OgrianBaloonThing_H__

#include <Ogre.h>
#include "OgrianThing.h"

#define BAL_STATE_ASCEND 1
#define BAL_STATE_DESCEND 2
#define BAL_STATE_TRAVEL 3

using namespace Ogre;

namespace Ogrian
{

class BaloonThing : public Thing
{
public:
	BaloonThing(unsigned int amount=1, Vector3 pos = Vector3(0,0,0)) 
		: Thing("Ogrian/Baloon", SPRITE, "ManaThing", false, 1, pos, SPHERE)
	{
		mTaret = 0;
		mNeedOrders = true;
	}

	// change the colour to reflect team ownership
	virtual void setTeamNum(int teamNum)
	{
		Team* team = Physics::getSingleton().getTeam(teamNum);

		setColour(team->getColour());

		Thing::setTeamNum(teamNum);

		setUpdateFlag();
	}

	virtual void setTarget(Thing* target)
	{
		mTarget = target;
		mState = BAL_STATE_ASCENDING;
		setVelocity(Vector3(0,CONR("BALOON_SPEED"),0));
	}

	virtual Thing* getTarget()
	{
		return mTarget;
	}

	virtual void move(Real time)
	{
		if (mTarget && mTarget->getTeamNum() == getTeamNum())
		{
			mNeedOrders = false;

			//  if it's ascending and has gotten high enough
			if (mState == BAL_STATE_ASCENDING && getPosY() > getGroundY() + CONR("BALOON_ALTITUDE"))
			{
				// change to travel mode
				mState = BAL_STATE_TRAVEL;
			}
			
			// if its travelling
			if (mState == BAL_STATE_TRAVEL
				

		}
		else
		{
			mTarget = 0;
			mNeedOrders = true;
			setVelocity(Vector3(0,0,0));
		}
	}

	virtual bool needOrders()
	{
		return mNeedOrders;
	}

	virtual void load(int amount)
	{
		mAmount += amount;
	}

	virtual int unload()
	{
		int a = mAmount;
		mAmount = 0;
		return a;
	}

private:
	ManaThing* mTarget;
	bool mNeedOrders;
	int mAmount;
	int mState; 
};

}

#endif