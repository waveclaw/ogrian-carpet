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
OgrianTickThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The tick attacks enemies by jumping at them

/*------------------------------------*/


#include "OgrianTickThing.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

TickThing::TickThing(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "TickThing", false, CONR("TICK_SCALE"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setThinkPeriod(CONR("TICK_THINK_PERIOD"));
	
	if (!Multiplayer::getSingleton().isClient())
		setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	getVisRep()->addPose("Ogrian/Tick/Sitting/");
	getVisRep()->addPose("Ogrian/Tick/Jumping/");
	getVisRep()->setPose(0);

	setHealth(CONI("TICK_HEALTH"));

	setPosY(getGroundY()+CONR("TICK_SCALE")/2);
}

//----------------------------------------------------------------------------

void TickThing::think()
{
	if (getVelocity().length() > 0) return;

	// jump at the nearest enemy
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team)
	{
		Thing* target = team->getNearestEnemy(this, CONR("SENTINEL_SIGHT_RANGE"));
		if (target) 
		{
			// face the enemy //
		
			// determine the direction
			Vector3 pos = getPosition();
			Vector3 epos = target->getPosition();
			float dir = atan2(epos.x - pos.x, epos.z - pos.z);
			
			// constrain the offset
			while (dir > Math::PI) dir -= 2*Math::PI;
			while (dir < -Math::PI) dir += 2*Math::PI;

			// set orientation
			setOrientation(dir);

			// attack the enemy //

			// account for target movement
			Real claimTravelTime = sphereDistance(target) / CONR("FIREBALL_SPEED");
			Vector3 targetOffset = target->getVelocity()*claimTravelTime;

			// shoot at the tips of buildings
			if (target->isBuilding())
				epos.y = epos.y + target->getHeight()/2;

			// calculate the trajectory
			Vector3 vel = (epos + targetOffset) - pos;
			vel.normalise();
			vel *= CONR("FIREBALL_SPEED");

			// jump at it
		}
	}
}
	
//----------------------------------------------------------------------------
	
void TickThing::die()
{
	// drop our mana
	ManaThing* mana = new ManaThing(CONI("TICK_DROP"), getPosition());
	Physics::getSingleton().addThing(mana);
	mana->setTeamNum(getTeamNum());

	Team* team = Physics::getSingleton().getTeam(getTeamNum());

	// add the mana to the castle
	if (team && team->getCastle())
		team->getCastle()->addMana(CONI("TICK_COST") - CONI("TICK_DROP"));

	// self destruct
	destroy();
}
	
//----------------------------------------------------------------------------
}