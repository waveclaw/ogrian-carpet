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
OgrianManaThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the ManaThing is a ball of mana
They drift downhill and combine when they touch. 
They have some amount of mana.
When they combine, one transfers all of its amount to the other.
When they are claimed, they move to the castle

/*------------------------------------*/

#include <Ogre.h>
#include "OgrianPhysics.h"
#include "OgrianManaThing.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

// extend floatingThing for floating behavior
ManaThing::ManaThing(int amount, Vector3 pos) 
	: Thing("Ogrian/Mana", SPRITE, "ManaThing", false, 1, pos, SPHERE)
{
	mStopped = false;

	setAmount(amount);
	setTeamNum(-1);
	setColour(ColourValue(.9,.9,.9));
	setUpdateType(PERIODIC);
	setThinkPeriod(CONR("MANA_THINK_PERIOD"));
}

//----------------------------------------------------------------------------

// change the colour to reflect team ownership
void ManaThing::setTeamNum(int teamNum)
{
	// change the colour
	Team*  newTeam = Physics::getSingleton().getTeam(teamNum);
	if (newTeam) setColour(newTeam->getColour());
	setUpdateFlag();

	Thing::setTeamNum(teamNum);
}

//----------------------------------------------------------------------------

void ManaThing::claim(int teamNum)
{
	mStopped = false;
	playSound(Game::getSingleton().SOUND_HUM, true);

	setTeamNum(teamNum);
}

//----------------------------------------------------------------------------

// setting the amount automatically sets the scale
void ManaThing::setAmount(int amount)
{
	if (amount < 0) amount = 0;

	mAmount=amount;

	Real a = sqrt(amount);

	// max scale
	if (a > CONR("MANA_MAX_SCALE"))
		setScale(CONR("MANA_MAX_SCALE"));

	// min scale
	else if (a < CONR("MANA_MIN_SCALE"))
		setScale(CONR("MANA_MIN_SCALE"));

	else 
		setScale(a);
}

//----------------------------------------------------------------------------

// get the amount of mana this represents
int ManaThing::getAmount()
{
	return mAmount;
}

//----------------------------------------------------------------------------

// When two manas collide, they combine - one takes on all of the amount of the other
// and the other disapears
void ManaThing::collided(Thing* e)
{
	if (e->getType() == MANATHING && e->isAlive() && isAlive())
	{
		ManaThing* m = static_cast<ManaThing*>(e);

		// if this one is the smaller of the two
		if (getAmount() <= m->getAmount())
		{
			// unload into the other
			m->setAmount(m->getAmount() + getAmount());

			setAmount(0);

			// go away
			destroy();
		}
	}

	
	else if (e->getType() == CASTLEFLAGTHING && e->getTeamNum() == getTeamNum())
	{
		Physics::getSingleton().getTeam(getTeamNum())->getCastle()->addMana(mAmount);
		destroy();
	}
}

//----------------------------------------------------------------------------

// move downhill at a constant slow velocity if unclaimed
void ManaThing::think()
{
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team && team->hasCastle())
	{
		Vector3 vel = team->getCastle()->getPosition() - getPosition();

		if (cylinderDistance(team->getCastle()) > CONR("MANA_RISE_RANGE"))
			vel.y = getVelY();

		vel.normalise();
		vel *= CONR("MANA_MOVE_SPEED");
		setVelocity(vel);
	}
	else if (!mStopped)
	{
		Vector3 vel;
		Vector3 pos = getPosition();
		vel.x = HeightMap::getSingleton().getXSlopeAt(pos.x, pos.z);
		vel.y = getVelY();
		vel.z = HeightMap::getSingleton().getZSlopeAt(pos.x, pos.z);
		vel.normalise();
		vel *= CONR("MANA_DRIFT_SPEED");

		// check to see if we're stuck
		if (Vector3(vel + getVelocity()).length() < CONR("MANA_DRIFT_SPEED")/4)
		{
			mStopped = true;
			setVelocity(Vector3(0,0,0));
		}
		else
			setVelocity(vel);
	}

	setUpdateFlag();
}

//----------------------------------------------------------------------------

// fall
void ManaThing::move(Real time)
{
	if (getVelY() > -CONR("MANA_FALL_MAX"))
	{
		setVelY(getVelY() - CONR("MANA_GRAV")*time);
	}

	Thing::move(time);
}

//----------------------------------------------------------------------------

// stay above ground
void ManaThing::setPosition(Vector3 pos)
{
	// start by setting the new position
	Thing::setPosition(pos);

	// if we are now underground, go up to the surface
	Real ground = getGroundY() + getHeight()/2;
	if (ground > getPosY()) 
	{
		setVelY(0);
		pos.y = ground;
		Thing::setPosition(pos);
	}
}

//----------------------------------------------------------------------------

void ManaThing::destroy()
{
	setTeamNum(-1);

	Thing::destroy();
}

//----------------------------------------------------------------------------

}
