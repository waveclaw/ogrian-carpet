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
OgrianDamageableThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: DamageableThing is a thing that can take damage, and will die

/*------------------------------------*/

#include "OgrianDamageableThing.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

DamageableThing::DamageableThing(String material, ThingVisRep visrep, String prefix, bool fixed_y, 
	Real scale, Vector3 pos, ThingShape shape)
	: Thing(material, visrep, prefix, fixed_y, scale, pos, shape)
{
	mHealth = 0;
}

void DamageableThing::setHealth(int health)
{
	mHealth = health;
}

int DamageableThing::getHealth()
{
	return mHealth;
}

void DamageableThing::damage(int amount, int sourceTeamNum)
{
	mLastDamageSource = sourceTeamNum;

	setHealth(mHealth - amount);
	if (mHealth <= 0) die();
}

int DamageableThing::getLastDamageSourceTeamNum()
{
	return mLastDamageSource;
}

void DamageableThing::setTeamNum(int teamNum)
{
	int oldTeamNum = getTeamNum();

	Thing::setTeamNum(teamNum);

	if (Multiplayer::getSingleton().isClient()) return;

	// remove it from the teams enemy lists
	if (oldTeamNum >= 0)
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
			Physics::getSingleton().getTeam(i)->removeEnemy(this);

	// add it to the teams enemy lists
	for (int i=0; i<Physics::getSingleton().numTeams(); i++)
		Physics::getSingleton().getTeam(i)->addEnemy(this);
}

void DamageableThing::destroy()
{
	Thing::destroy();

	// remove it from the teams enemy lists
	if (getTeamNum() >= 0)
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
			if (i != getTeamNum())
				Physics::getSingleton().getTeam(i)->removeEnemy(this);
}

}
