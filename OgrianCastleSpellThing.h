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
OgrianCastleSpellThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a simple ball that creates a castle when it hits the ground 

/*------------------------------------*/

#ifndef __OgrianCastleSpellThing_H__
#define __OgrianCastleSpellThing_H__

#include <Ogre.h>
#include "OgrianTimedThing.h"
#include "OgrianCastleTowerThing.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class CastleSpellThing : public TimedThing
{
public:
	CastleSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) 
		: TimedThing("Ogrian/Rock", SPRITE, "CastleSpell", false, CONR("CASTLESPELL_SCALE"), pos, SPHERE)
	{
		mTeamNum = teamNum;

		setVelocity(vel);
		playSound(Game::getSingleton().SOUND_WHOOSH);
		setFlickerPeriod(CONR("CASTLESPELL_FLICKER_PERIOD"));
		setRelativeExpirationTime(CONR("CASTLESPELL_LIFETIME"));
	}

	virtual void collidedGround()
	{
		// make a castle
		Physics::getSingleton().addThing(new CastleTowerThing(mTeamNum, getPosition()));

		// self destruct
		destroy();
	}

private:
	int mTeamNum;
};

}

#endif
