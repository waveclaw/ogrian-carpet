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
OgrianGnomeThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The gnome attacks enemies by shooting fireballs at them. it also follows its wizard.

/*------------------------------------*/


#ifndef __OgrianGnomeThing_H__
#define __OgrianGnomeThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianSummonSpellThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class GnomeThing : public DamageableThing
{
public:
	GnomeThing(int teamNum, Vector3 pos=Vector3(0,0,0));

	virtual ThingType getType() { return GNOMETHING; }

	virtual void move(Real time);
	virtual void think();
	virtual void die();

private:
	Vector3 mFormationOffset; 
	Vector3 mFormation;
};

/////////////////////////////////////////////////////////////////////////////

class GnomeSummonSpellThing : public SummonSpellThing
{
public:
	GnomeSummonSpellThing::GnomeSummonSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) :
		SummonSpellThing(teamNum, pos, vel) 
	{}

	virtual ThingType getType() { return GNOMESUMMONTHING; }

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
		if (team && team->getCastle() && team->getCastle()->getMana() >= CONI("GNOME_COST"))
		{
			// make a sentinel
			Physics::getSingleton().addThing(new GnomeThing(getTeamNum(), getPosition()));

			// remove the mana from the castle
			Castle* castle = team->getCastle();
			castle->setMana(castle->getMana() - CONI("GNOME_COST"));
		}

		// self destruct
		destroy();
	}
};

}

#endif