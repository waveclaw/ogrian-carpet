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

Description: This makes a castle, or claims a hut or mana

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
	ClaimSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) 
		: TimedThing("Ogrian/Rock", SPRITE, "CastleSpell", false, CONR("CLAIMSPELL_SCALE"), pos, SPHERE)
	{
		setTeamNum(teamNum);

		setVelocity(vel);
		playSound(Game::getSingleton().SOUND_WHOOSH);
		setFlickerPeriod(CONR("CLAIMSPELL_FLICKER_PERIOD"));
		setRelativeExpirationTime(CONR("CLAIMSPELL_LIFETIME"));

		setGroundScan(true);
	}

	virtual ThingType getType() { return CLAIMTHING; }

	virtual void collidedGround()
	{
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
			WizardThing* wizard = (WizardThing*)Physics::getSingleton().getThing(team->getWizardUID());

		if (getGroundY() > CONR("BUILDING_MIN_GROUNDY") && team && !team->hasCastle())
		{
			// make sure its not too close to other buildings
			for (int i=0; i<Physics::getSingleton().numThings(); i++)
			{
				Thing* thing = Physics::getSingleton().getThingByIndex(i);
				if (thing->isBuilding() && axisDistance(thing) < 6 * CONR("CASTLE_WIDTH"))
				{
					destroy();
					return;
				}
			}

			// make a castle
			Castle* castle = new Castle(getTeamNum(), getPosition());
			team->setCastle(castle);
		}

		// self destruct
		destroy();
	}

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
