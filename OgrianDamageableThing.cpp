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
It also has an optional health bar

/*------------------------------------*/

#include "OgrianDamageableThing.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

DamageableThing::DamageableThing(String material, ThingVisRep visrep, String prefix, bool fixed_y, 
	Real scale, Vector3 pos, ThingShape shape, bool hasBar)
	: Thing(material, visrep, prefix, fixed_y, scale, pos, shape)
{
	mHealth = 0;
	mBar = 0;
	mHasBar = hasBar;
	mMaxHealth = 100;

	reset();
}

//----------------------------------------------------------------------------

void DamageableThing::reset()
{
	// make the health bar
	if (mHasBar)
	{
		//if (mBar) mBar->destroy();

		mBar = new HealthBarEffect(getPosition(), getHeight(), getWidth());
		Physics::getSingleton().addEffect(mBar);
	}
	else mBar = 0;
}

//----------------------------------------------------------------------------

void DamageableThing::setColour(ColourValue& colour)
{
	Thing::setColour(colour);
	if (mBar) mBar->setColour(colour);
}

//----------------------------------------------------------------------------

void DamageableThing::move(Real time)
{
	Thing::move(time);
	
	// update health bar
	if (mBar)
		mBar->update(getPosition(), ((float)mHealth/(float)mMaxHealth)*getWidth());
}

//----------------------------------------------------------------------------

void DamageableThing::setHealth(int health)
{
	mHealth = health;
}

//----------------------------------------------------------------------------

void DamageableThing::setMaxHealth(int maxHealth)
{
	mMaxHealth = maxHealth;
}

//----------------------------------------------------------------------------

int DamageableThing::getHealth()
{
	return mHealth;
}

//----------------------------------------------------------------------------

void DamageableThing::damage(int amount, int sourceTeamNum)
{
	mLastDamageSource = sourceTeamNum;

	setHealth(mHealth - amount);
	if (mHealth <= 0) die();

	if (mHasBar)
		setUpdateFlag();
}

//----------------------------------------------------------------------------

int DamageableThing::getLastDamageSourceTeamNum()
{
	return mLastDamageSource;
}

//----------------------------------------------------------------------------

void DamageableThing::setTeamNum(int teamNum)
{
	int oldTeamNum = getTeamNum();

	Thing::setTeamNum(teamNum);

	if (Multiplayer::getSingleton().isClient()) return;

	// remove it from the teams enemy lists (if it was on them)
	if (oldTeamNum >= 0)
	{
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
		{
			Team* team = Physics::getSingleton().getTeam(i);
			if (team) team->removeEnemy(this);
		}
	}

	// add it to the teams enemy lists
	for (int i=0; i<Physics::getSingleton().numTeams(); i++)
	{
		Team* team = Physics::getSingleton().getTeam(i);
		if (team) team->addEnemy(this);
	}
}

//----------------------------------------------------------------------------

void DamageableThing::destroy()
{
	Thing::destroy();

	if (mBar) mBar->destroy();
	mBar = 0;

	// remove it from the teams enemy lists
	if (getTeamNum() >= 0)
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
			if (i != getTeamNum())
			{
				Team* team = Physics::getSingleton().getTeam(i);
				if (team) team->removeEnemy(this);
			}
}

//----------------------------------------------------------------------------

bool DamageableThing::hasHealthBar()
{
	return mHasBar;
}

//----------------------------------------------------------------------------

void DamageableThing::generateBitStream(BitStream& bitstream, int pid)
{
	Thing::generateBitStream(bitstream,pid);

	bitstream.Write(mHealth);
}

//----------------------------------------------------------------------------

void DamageableThing::interpretBitStream(BitStream& bitstream)
{
	Thing::interpretBitStream(bitstream);

	int health;

	bitstream.Read(health);

	// servers tell teh clients what the health is, not the other way around
	if (!Multiplayer::getSingleton().isServer())
		mHealth = health;
}

//----------------------------------------------------------------------------

}
