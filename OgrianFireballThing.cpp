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
OgrianFireballThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: Fireballs are the basic form of ranged attack. There are two modes.
In simple=true mode, they fly straight with no smoke and dont hurt teammates.
In simple=false mode, they fly in an arc, leave a smoke trail, and hurt teammates. 

/*------------------------------------*/


#include <Ogre.h>
#include "OgrianFireballThing.h"
#include "OgrianRenderer.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

FireballBlastEffect::FireballBlastEffect(Vector3 pos) 
	: TimedThing("Ogrian/FireballBlast", SPRITE, "FireballBlast", false, CONR("FIREBALL_BLAST_SCALE"), pos, SPHERE)
{
	playSound(Game::getSingleton().SOUND_BANG);
	setRelativeExpirationTime(CONR("FIREBALL_BLAST_LIFETIME"));
	setFlickerPeriod(CONR("FIREBALL_FLICKER_PERIOD"));
	setColour(ColourValue(1,0,0));
}

//----------------------------------------------------------------------------

void FireballBlastEffect::move(Real time)
{
	TimedThing::move(time);

	setScale(getWidth() + CONR("FIREBALL_BLAST_EXPANSION_SPEED") * time);
}

//----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

FireballThing::FireballThing(int teamNum, ColourValue colour, Vector3 pos, Vector3 vel,
	int damage, bool simple) 
	: TimedThing("Ogrian/Fireball", SPRITE, "Fireball", false, CONR("FIREBALL_SCALE"), pos, SPHERE)
{
	mSimple = simple;
	mDamage = damage;

	setTeamNum(teamNum);
	mColour = colour;
	setColour(mColour);
	setGroundScan(true);

	setVelocity(vel);
	playSound(Game::getSingleton().SOUND_WHOOSH);
	setFlickerPeriod(CONR("FIREBALL_FLICKER_PERIOD"));
	setRelativeExpirationTime(CONR("FIREBALL_LIFETIME"));

	// init the smoke
	mLastSmokeTime = 0;
	mLastSmokeIndex = 0;
	mLastPos = getPosition();
}

//----------------------------------------------------------------------------

void FireballThing::move(Real time)
{
	// fall
	if (!mSimple)
		setVelocity(getVelocity() + Vector3(0, -CONR("FIREBALL_FALL_RATE") * time, 0));

	TimedThing::move(time);

	// emit smoke
	if (isAlive() && !mSimple && mSmokeList.size() == 0)
	{
		for (int i=0; i<CONR("FIREBALL_SMOKE_NUM"); i++)
		{
			FireballSmokeEffect* fse = new FireballSmokeEffect(Vector3(0,-100,0), getColour());
			mSmokeList.push_back(fse);
			Physics::getSingleton().addEffect(fse);
		}
	}
	if (isAlive() && mLastSmokeTime + CONT("FIREBALL_SMOKE_PERIOD") < Clock::getSingleton().getTime() 
		&& mSmokeList.size() > 0)
	{
		if (mLastSmokeIndex == mSmokeList.size()) mLastSmokeIndex = 0;
		mSmokeList[mLastSmokeIndex++]->setPosition(mLastPos);
		mLastSmokeTime = Clock::getSingleton().getTime();
	}

	mLastPos = getPosition();
}

//----------------------------------------------------------------------------

void FireballThing::collided(Thing* e)
{
	// damage it
	if (e->isDamageable() 
		&& (!mSimple || getColour() != e->getColour()) // dont let simple fireballs hurt allies
		&& !((e->getType() == CAMERATHING || e->getType() == WIZARDTHING) && getTeamNum() == e->getTeamNum())) // dont let any fireballs hurt allied wizards
	{
		e->damage(mDamage, getTeamNum());

		// self destruct
		destroy();
	}
}

//----------------------------------------------------------------------------

void FireballThing::collidedGround()
{
	destroy();
}

//----------------------------------------------------------------------------

// go boom when destroyed
void FireballThing::destroy()
{
	// make a blast
	if (!isAlive()) return;

	Physics::getSingleton().addEffect(new FireballBlastEffect(getPosition()));

	// clean up the smoke
	for (int i=0; i<(int)mSmokeList.size(); i++)
		mSmokeList[i]->destroy();

	Thing::destroy();
}

//----------------------------------------------------------------------------

// generate a bitstream from this thing
void FireballThing::generateBitStream(BitStream& bitstream, int pid)
{
	TimedThing::generateBitStream(bitstream, pid);
	
	// include mSimple
	bitstream.Write(mSimple);
}

//----------------------------------------------------------------------------

// interpret a bitstream for this thing
void FireballThing::interpretBitStream(BitStream& bitstream)
{
	TimedThing::interpretBitStream(bitstream);
	
	// read mSimple
	bool simple;
	bitstream.Read(simple);

	mSimple = simple;
}


//----------------------------------------------------------------------------

}
