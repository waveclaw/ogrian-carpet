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

Description: Fireballs fly in an arc and destroy() whatever they touch.
They self destruct when they hit the ground or another thing. 

/*------------------------------------*/

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
	FireballSmokeEffect(Vector3 pos) 
		: Thing("Ogrian/Smoke", SPRITE, "FireballSmoke", false, CONR("FIREBALL_SCALE")*.8, pos, SPHERE)
	{
		setVelocity(Vector3(0,1,0));
	}

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////

class FireballBlastEffect : public TimedThing
{
public:
	FireballBlastEffect(Vector3 pos) 
		: TimedThing("Ogrian/FireballBlast", SPRITE, "FireballBlast", false, CONR("FIREBALL_SCALE")*2, pos, SPHERE)
	{
		playSound("OgrianMedia/sounds/boom1.wav");
		setRelativeExpirationTime(1);
		setFlickerPeriod(CONR("FIREBALL_FLICKER_PERIOD"));
	}

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////
class FireballThing : public TimedThing
{
public:
	FireballThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) 
		: TimedThing("Ogrian/Fireball", SPRITE, "Fireball", false, CONR("FIREBALL_SCALE"), pos, SPHERE)
	{
		mTeamNum = teamNum;

		setVelocity(vel);
		playSound("OgrianMedia/sounds/whoosh1.wav");
		setFlickerPeriod(CONR("FIREBALL_FLICKER_PERIOD"));
		setRelativeExpirationTime(CONR("FIREBALL_LIFETIME"));

		// init the smoke
		mLastSmokeTime = 0;
		mLastSmokeIndex = 0;
		mLastPos = getPosition();
		
		for (int i=0; i<CONR("FIREBALL_SMOKE_NUM"); i++)
		{
			FireballSmokeEffect* fse = new FireballSmokeEffect(Vector3(0,-100,0));
			mSmokeList.push_back(fse);
			Physics::getSingleton().addEffect(fse);
		}
	}

	virtual ThingType getType()	{ return FIREBALLTHING; }	

	virtual void move(Real time)
	{
		// fall
		setVelocity(getVelocity() + Vector3(0, -CONR("FIREBALL_FALL_RATE") * time, 0));
		TimedThing::move(time);

		// emit smoke
		if (isAlive() && mLastSmokeTime + CONR("FIREBALL_SMOKE_PERIOD")*1000 < Time::getSingleton().getTime())
		{
			if (mLastSmokeIndex == mSmokeList.size()) mLastSmokeIndex = 0;
			mSmokeList[mLastSmokeIndex++]->setPosition(mLastPos);
			mLastSmokeTime = Time::getSingleton().getTime();
		}

		// die when it hits the ground
		if (getGroundY() > getPosition().y) destroy();

		mLastPos = getPosition();
	}

	virtual void collided(Thing* e)
	{
		// damage it
		if (e->isDamageable())	e->damage(CONR("FIREBALL_DAMAGE"), mTeamNum);

		// self destruct
		destroy();
	}

	// go boom when destroyed
	virtual void destroy()
	{
		// make a blast
		if (!isAlive()) return;

		Physics::getSingleton().addEffect(new FireballBlastEffect(getPosition()));

		// clean up the smoke
		for (int i=0; i<(int)mSmokeList.size(); i++)
			mSmokeList[i]->destroy();

		Thing::destroy();
	}

private:
	int mTeamNum;
	Vector3 mLastPos;
	unsigned long mLastSmokeTime;
	int mLastSmokeIndex;
	std::vector<FireballSmokeEffect*> mSmokeList;

};

}

#endif