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
OgrianCraneThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the crane is a flying bird that orbits a point until it detects an enemy, 
at which point, it flys towards it. when it reaches its target, it blows up, dealing damage,
and respawns back at its home point (around which it orbits).

/*------------------------------------*/


#include "OgrianCraneThing.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

CraneThing::CraneThing(int teamNum, Vector3 orbitPos) 
	: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "CraneThing", false, CONR("CRANE_SCALE"), orbitPos, SPHERE, false)
{
	mOrbitPos = orbitPos;
	mLastFlap = false;
	mUnIdleTime = 0;

	setTeamNum(teamNum);
	setThinkPeriod(CONR("CRANE_THINK_PERIOD"));
	
	if (!Multiplayer::getSingleton().isClient())
		setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	getVisRep()->addPose("Ogrian/Crane/Up/");
	getVisRep()->addPose("Ogrian/Crane/Down/");

	getVisRep()->setPose(0);

	setHealth(CONI("CRANE_HEALTH"));
	setStateFlyOut();
}

//----------------------------------------------------------------------------

void CraneThing::setVelocity(Vector3 vel)
{
	Vector3 vvel = vel;
	DamageableThing::setVelocity(vvel);

	vel.y = 0;
	vel.normalise();

	Radian orientation = Math::ATan2(vel.x, vel.z);
	setOrientation(orientation.valueRadians());
}

//----------------------------------------------------------------------------

void CraneThing::move(Real time)
{
	DamageableThing::move(time);

	// stay above a minimum altitude
	if (getPosY() < getGroundY() + CONR("CRANE_ALTITUDE_MIN")) 
		setPosY(getGroundY() + CONR("CRANE_ALTITUDE_MIN"));
}

//----------------------------------------------------------------------------

void CraneThing::clientThink()
{
	// flap
	mLastFlap = !mLastFlap;
	getVisRep()->setPose( mLastFlap ? 0 : 1 );
}

//----------------------------------------------------------------------------

void CraneThing::think()
{
	// flap
	mLastFlap = !mLastFlap;
	getVisRep()->setPose( mLastFlap ? 0 : 1 );

	// state operation
	if (mState == CRANE_STATE_FLY_IN)
	{
		// stay in the orbit zone
		if (orbitDistance() > CONR("CRANE_ORBIT_MAX"))
			setStateFlyIn();

		// stay in the orbit zone
		if (orbitDistance() < CONR("CRANE_ORBIT_MIN"))
			setStateFlyOut();
		
		// look for enemies
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		if (team && team->getNearestEnemy(this, CONR("CRANE_SIGHT_RANGE")))
			setStateAttack();
	}
	else if (mState == CRANE_STATE_FLY_OUT)
	{
		// stay in the orbit zone
		if (orbitDistance() > CONR("CRANE_ORBIT_MAX"))
			setStateFlyIn();
		
		// look for enemies
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		if (team && team->getNearestEnemy(this, CONR("CRANE_SIGHT_RANGE")))
			setStateAttack();
	}
	else if (mState == CRANE_STATE_ATTACK)
	{
		setStateAttack();
	}
	else if (mState == CRANE_STATE_IDLE)
	{
		if (Clock::getSingleton().getTime() > mUnIdleTime)
			setStateFlyOut();
	}
}

//----------------------------------------------------------------------------

void CraneThing::handleMessage(int msg, Vector3 vec)
{
	Physics::getSingleton().addEffect(new CraneBlastEffect(vec));
}

//----------------------------------------------------------------------------

void CraneThing::collided(Thing* e)
{
	if (e->isDamageable() && e->isAlive()
		&& e->getTeamNum() != getTeamNum() // dont hurt allies
		&& e->getColour() != getColour()) // dont hurt teammates
	{
		e->damage(CONI("CRANE_DAMAGE"), getTeamNum());
		Physics::getSingleton().addEffect(new CraneBlastEffect(getPosition()));
		sendMessage(1, getPosition());
		die();
	}
}

//----------------------------------------------------------------------------

void CraneThing::die()
{
	DamageableThing::die();

	setHealth(CONI("CRANE_HEALTH"));
	setPosition(mOrbitPos);
	setStateIdle();
}

//----------------------------------------------------------------------------

void CraneThing::setStateFlyIn()
{
	mState = CRANE_STATE_FLY_IN;
				
	Vector3 dir = mOrbitPos - getPosition();
	dir.normalise();

	// randomize the direction by a little bit
	Vector3 odir;
	odir.x = Math::RangeRandom(-1,1);
	odir.y = Math::RangeRandom(-1,1);
	odir.z = Math::RangeRandom(-1,1);
	odir.normalise();

	Real v = CONR("CRANE_VARIANCE");
	dir = dir*(1-v) + odir*v;

	setVelocity(dir * CONR("CRANE_SPEED"));

	setUpdateFlag();
}

//----------------------------------------------------------------------------

void CraneThing::setStateFlyOut()
{
	mState = CRANE_STATE_FLY_OUT;
		
	// randomize the direction
	Vector3 dir;
	dir.x = Math::RangeRandom(-1,1);
	dir.y = Math::RangeRandom(-.5,.5);
	dir.z = Math::RangeRandom(-1,1);
	dir.normalise();
	setVelocity(dir * CONR("CRANE_SPEED"));

	setUpdateFlag();
}

//----------------------------------------------------------------------------

void CraneThing::setStateAttack()
{
	mState = CRANE_STATE_ATTACK;
	
	// look for enemies
	Team* team = Physics::getSingleton().getTeam(getTeamNum());

	Thing* target = team->getNearestEnemy(this, CONR("CRANE_SIGHT_RANGE"));
	
	if (target)
	{
		// kamakazi!!!
		Vector3 dir = target->getPosition() - getPosition();
		dir.normalise();
		setVelocity(dir * CONR("CRANE_SPEED"));
	
		playSound(Game::getSingleton().SOUND_CHIRP, true);

		setUpdateFlag();
	}
	else
	{
		// return to base
		setStateFlyIn();
	}
}

//----------------------------------------------------------------------------

void CraneThing::setStateIdle()
{
	mState = CRANE_STATE_IDLE;

	setVelocity(Vector3(0,0,0));
	
	mUnIdleTime = Clock::getSingleton().getTime() + CONR("CRANE_IDLE_TIME") * 1000;

	setUpdateFlag();
}

//----------------------------------------------------------------------------

}