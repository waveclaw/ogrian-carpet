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
OgrianSentinelThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The sentinal is a monster that is stationary at fires fireballs at 
nearby enemies.

/*------------------------------------*/


#ifndef __OgrianSentinelThing_H__
#define __OgrianSentinelThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianSummonSpellThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class SentinelThing : public DamageableThing
{
public:
	SentinelThing(int teamNum, Vector3 pos=Vector3(0,0,0));

	virtual ThingType getType() { return SENTINELTHING; }

	virtual void think();
	virtual void die();
};

/////////////////////////////////////////////////////////////////////////////

class SentinelSummonSpellThing : public SummonSpellThing
{
public:
	SentinelSummonSpellThing::SentinelSummonSpellThing(int teamNum, ColourValue colour=ColourValue::White, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) :
		SummonSpellThing(teamNum, colour, pos, vel) 
	{}

	virtual ThingType getType() { return SENTINELSUMMONTHING; }

	virtual void collidedGround() 
	{
		Team* team = Physics::getSingleton().getTeam(getTeamNum());

		// make sure its not in water
		if (getGroundY() < CONR("BUILDING_MIN_GROUNDY"))
		{
			destroy();
			return;
		}

		// if there's enough mana
		if (team && team->getCastle() && team->getCastle()->getMana() >= CONI("SENTINEL_COST"))
		{
			// make a sentinel
			Physics::getSingleton().addThing(new SentinelThing(getTeamNum(), getPosition()));

			// remove the mana from the castle
			Castle* castle = team->getCastle();
			castle->setMana(castle->getMana() - CONI("SENTINEL_COST"));
		}

		// self destruct
		destroy();
	}
};

}

#endif