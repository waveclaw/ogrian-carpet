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
OgrianCraneThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the crane is a flying bird that orbits a point until it detects an enemy, 
at which point, it flys towards it. when it reaches its target, it blows up, dealing damage,
and respawns back at its home point (around which it orbits).

 *------------------------------------*/


#ifndef __OgrianCraneThing_H__
#define __OgrianCraneThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"
#include "OgrianTimedThing.h"
#include "OgrianConst.h"
#include "OgrianGame.h"

using namespace Ogre;

#define CRANE_STATE_IDLE 0
#define CRANE_STATE_FLY_OUT 1
#define CRANE_STATE_FLY_IN 2
#define CRANE_STATE_ATTACK 3

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class CraneBlastEffect : public TimedThing
{
public:
	CraneBlastEffect(Vector3 pos) 
		: TimedThing("Ogrian/CraneBlast", SPRITE, "CraneBlast", false, CONR("CRANE_BLAST_SCALE"), pos, SPHERE)
	{
		playSound(Game::getSingleton().SOUND_BANG);
		setRelativeExpirationTime(CONR("CRANE_BLAST_LIFETIME"));
		setFlickerPeriod(CONR("CRANE_BLAST_FLICKER_PERIOD"));
		setColour(ColourValue(1,0,0));
	}

	virtual void move(Real time)
	{
		TimedThing::move(time);

		setScale(getWidth() + CONR("CRANE_BLAST_EXPANSION_SPEED") * time);
	}

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////

class CraneThing : public DamageableThing
{
public:
	CraneThing(int teamNum, Vector3 orbitPos=Vector3(0,0,0));

	virtual ThingType getType() { return CRANETHING; }

	virtual void move(Real time);
	virtual void think();
	virtual void clientThink();
	virtual void setVelocity(Vector3 vel);
	virtual void collided(Thing* e);
	virtual void die();

	virtual void setStateIdle();
	virtual void setStateFlyIn();
	virtual void setStateFlyOut();
	virtual void setStateAttack();

	virtual void handleMessage(int msg, Vector3 vec, int val);

private:
	Vector3 mOrbitPos;
	int mState;
	bool mLastFlap;
	Time mUnIdleTime;

	Real orbitDistance()
	{
		Vector3 pos = getPosition();
		return sqrt( (mOrbitPos.x - pos.x) * (mOrbitPos.x - pos.x) +
					 (mOrbitPos.y - pos.y) * (mOrbitPos.y - pos.y) +
					 (mOrbitPos.z - pos.z) * (mOrbitPos.z - pos.z) );
	}
};

}

#endif
