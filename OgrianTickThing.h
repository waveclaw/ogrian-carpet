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
OgrianTickThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The tick attacks enemies by jumping at them

/*------------------------------------*/


#ifndef __OgrianTickThing_H__
#define __OgrianTickThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianSummonSpellThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class TickThing : public DamageableThing
{
public:
	TickThing(int teamNum, Vector3 pos=Vector3(0,0,0));

	virtual ThingType getType() { return TICKTHING; }

	virtual void collidedGround();
	virtual void move(Real time);
	virtual void think();
	virtual void die();

private:
	Vector3 mFormationOffset; 
};

/////////////////////////////////////////////////////////////////////////////

class TickSummonSpellThing : public SummonSpellThing
{
public:
	TickSummonSpellThing::TickSummonSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) :
		SummonSpellThing(teamNum, pos, vel) 
	{}

	virtual ThingType getType() { return TICKSUMMONTHING; }

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
		if (team && team->getCastle() && team->getCastle()->getMana() >= CONI("TICK_COST"))
		{
			// make a sentinel
			Physics::getSingleton().addThing(new TickThing(getTeamNum(), getPosition()));

			// remove the mana from the castle
			Castle* castle = team->getCastle();
			castle->setMana(castle->getMana() - CONI("TICK_COST"));
		}

		// self destruct
		destroy();
	}
};

}

#endif