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
OgrianClaimSpellThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This claims a hut or mana

/*------------------------------------*/

#ifndef __OgrianClaimSpellThing_H__
#define __OgrianClaimSpellThing_H__

#include <Ogre.h>
#include "OgrianTimedThing.h"
#include "OgrianCastle.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////
class ClaimSpellThing : public TimedThing
{
public:
	ClaimSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0), Real lifetime=CONR("CLAIMSPELL_LIFETIME")) 
		: TimedThing("Ogrian/Rock", SPRITE, "ClaimSpell", false, CONR("CLAIMSPELL_SCALE"), pos, SPHERE)
	{
		setTeamNum(teamNum);

		setVelocity(vel);
		playSound(Game::getSingleton().SOUND_WHOOSH);
		setFlickerPeriod(CONR("CLAIMSPELL_FLICKER_PERIOD"));
		setRelativeExpirationTime(lifetime);
	}

	virtual ThingType getType() { return CLAIMTHING; }

	virtual void collided(Thing* e)
	{
		// grow a castle (for testing)
		if (e->getType() == CASTLEFLAG)
		{
			Castle* castle = static_cast<Castle*>(e);
			castle->setMana(castle->getMana()+1);
			destroy();
		}

		// claim mana
		if (e->getType() == MANATHING)
		{
			e->setTeamNum(getTeamNum());
			destroy();
		}
	}
};

}

#endif
