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
OgrianAlbatrossThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The albatross is like a cruise missile, it just flies straight until it hits something.

/*------------------------------------*/


#include "OgrianAlbatrossThing.h"
#include "OgrianSummonSpellThing.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

AlbatrossThing::AlbatrossThing(int teamNum, Vector3 pos, Vector3 vel) 
	: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "AlbatrossThing", false, CONR("TICK_SCALE"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setThinkPeriod(CONR("ALBATROSS_THINK_PERIOD"));
	
	if (!Multiplayer::getSingleton().isClient())
	{
		// remove the mana from the castle
		Team* team = Physics::getSingleton().getTeam(teamNum);
		Castle* castle = team->getCastle();
		castle->setMana(castle->getMana() - CONI("ALBATROSS_COST"));

		setColour(team->getColour());
	}

	getVisRep()->addPose("Ogrian/Crane/Up/");
	getVisRep()->addPose("Ogrian/Crane/Down/");
	getVisRep()->setPose(0);

	setHealth(CONI("ALBATROSS_HEALTH"));

	setGroundScan(true);
	
	setVelocity(vel);


	Radian orientation = Math::ATan2(vel.x, vel.z);
	setOrientation(orientation.valueRadians());

	mLastFlap = false;
	mDeathTime = Clock::getSingleton().getTime() + CONT("ALBATROSS_LIFETIME");

	playSound(Game::getSingleton().SOUND_CHIRP);
}

//----------------------------------------------------------------------------

void AlbatrossThing::clientThink()
{
	getVisRep()->setPose(mLastFlap ? 0 : 1);
	mLastFlap = !mLastFlap;
}

//----------------------------------------------------------------------------

void AlbatrossThing::think()
{
	getVisRep()->setPose(mLastFlap ? 0 : 1);
	mLastFlap = !mLastFlap;

	if (Clock::getSingleton().getTime() > mDeathTime) die();
}
	
//----------------------------------------------------------------------------
	
void AlbatrossThing::collided(Thing* e)
{
	// explode stuff
	if (e->isDamageable() && e->getTeamNum() != getTeamNum())
	{
		e->damage(CONI("TICK_DAMAGE"), getTeamNum());
		die();
	}
}

//----------------------------------------------------------------------------
	
void AlbatrossThing::collidedGround()
{
	die();
}

//----------------------------------------------------------------------------
	
void AlbatrossThing::die()
{
	// drop our mana
	ManaThing* mana = new ManaThing(CONI("ALBATROSS_DROP"), getPosition());
	Physics::getSingleton().addThing(mana);
	mana->setTeamNum(getTeamNum());

	Team* team = Physics::getSingleton().getTeam(getTeamNum());

	// add the mana to the castle
	if (team && team->getCastle())
		team->getCastle()->addMana(CONI("ALBATROSS_COST") - CONI("ALBATROSS_DROP"));

	// self destruct
	destroy();
}
	
//----------------------------------------------------------------------------
}