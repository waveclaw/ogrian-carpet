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

 *------------------------------------*/

#ifndef __OgrianFireballThing_H__
#define __OgrianFireballThing_H__

#include <Ogre.h>
#include "OgrianTimedThing.h"
#include "OgrianRenderer.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class FireballSmokeEffect : public Thing
{
public:
	FireballSmokeEffect(Vector3 pos, ColourValue colour) 
		: Thing("Ogrian/Smoke", SPRITE, "FireballSmoke", false, CONR("FIREBALL_SCALE")*.8, pos, SPHERE)
	{
		setVelocity(Vector3(0,1,0));
		setColour(colour);
	}

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////

class FireballBlastEffect : public TimedThing
{
public:
	FireballBlastEffect(Vector3 pos);

	virtual void move(Real time);

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////
class FireballThing : public TimedThing
{
public:
	FireballThing(int teamNum, ColourValue colour=ColourValue::White, 
		Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0),
		int damage=CONR("FIREBALL_DAMAGE"), bool simple=false);

	virtual ThingType getType()	{ return FIREBALLTHING; }	

	virtual void move(Real time);

	virtual void collided(Thing* e);

	virtual void collidedGround();

	// go boom when destroyed
	virtual void destroy();

	// generate a bitstream from this thing
	virtual void generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING);

	// interpret a bitstream for this thing
	virtual void interpretBitStream(BitStream& bitstream);

private:
	bool mSimple;
	int mDamage;
	ColourValue mColour;
	Vector3 mLastPos;
	Time mLastSmokeTime;
	int mLastSmokeIndex;
	std::vector<FireballSmokeEffect*> mSmokeList;

};

}

#endif
