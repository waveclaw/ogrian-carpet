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

using namespace Ogre;

namespace Ogrian
{

class BaloonThing : public Thing
{
public:
	BaloonThing(int team, Vector3 pos = Vector3(0,0,0), int amount=1);

	// change the colour to reflect team ownership
	virtual void setTeamNum(int teamNum);

	virtual void setTarget(Thing* target);

	virtual Thing* getTarget();

	virtual void move(Real time);

	virtual void think();

	virtual void collided(Thing* thing);

	virtual bool needsOrders();

	virtual void load(int amount);

	virtual int unload();

	virtual int getAmount();

	virtual ThingType getType() { return BALOONTHING; }

private:
	Thing* mTarget;
	bool mNeedOrders;
	int mAmount;
	int mState; 

	Vector3 lastPos;

	// state changes
	inline void setStateWait();
	inline void setStateAscend();
	inline void setStateDescend();
	inline void setStateTravel();
};

}

#endif