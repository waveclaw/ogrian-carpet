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

CraneThing::CraneThing(int teamNum, Vector3 orbitPos) 
	: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "CraneThing", false, CONR("CRANE_SCALE"), orbitPos, SPHERE)
{
	mOrbitPos = orbitPos;
	mTarget = 0;
	mLastFlapTime = 0;
	mLastFlap = false;

	setTeamNum(teamNum);
	setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	getVisRep()->addPose("Ogrian/Crane/Up/");
	getVisRep()->addPose("Ogrian/Crane/Down/");

	getVisRep()->setPose(0);

	setStateSpawnFlyOut();
}

void CraneThing::move(Real time)
{
	DamageableThing::move(time);

	// stay above a minimum altitude
	if (getPosY() < getGroundY() + CONR("CRANE_ALTITUDE_MIN")) 
		setPosY(getGroundY() + CONR("CRANE_ALTITUDE_MIN"));

	// orbit
	if (mState == CRANE_STATE_ORBIT)
	{
		Vector3 dir = getPosition() - mOrbitPos;
		dir.y = 0;
		dir = dir.crossProduct( Vector3::UNIT_Y );
		dir.normalise();
		setVelocity(dir * CONR("CRANE_SPEED"));
	}
}

void CraneThing::think()
{
	// flap
	if (Time::getSingleton().getTime() > mLastFlapTime + CONR("CRANE_FLAP_PERIOD")*1000)
	{
		mLastFlap = !mLastFlap;
		mLastFlapTime = Time::getSingleton().getTime();

		getVisRep()->setPose( mLastFlap ? 0 : 1 );
	}

	// state operation
	if (mState == CRANE_STATE_FLY_IN)
	{
		// stay in the orbit zone
		if (orbitDistance() < CONR("CRANE_ORBIT_MIN"))
			setStateFlyOut();

		// check for orbit
		if (orbitDistance() < CONR("CRANE_ORBIT_MAX"))
			setStateOrbit();
	}
	else if (mState == CRANE_STATE_FLY_OUT)
	{
		// stay in the orbit zone
		if (orbitDistance() > CONR("CRANE_ORBIT_MAX"))
			setStateFlyIn();

		// check for orbit
		if (orbitDistance() > CONR("CRANE_ORBIT_MIN"))
			setStateOrbit();
	}
	else if (mState == CRANE_STATE_SPAWN_FLY_OUT)
	{
		// stay in the orbit zone
		if (orbitDistance() > CONR("CRANE_ORBIT_MAX"))
			setStateFlyIn();

		// check for orbit
		if (orbitDistance() > CONR("CRANE_ORBIT_MIN"))
			setStateOrbit();
	}
	else if (mState == CRANE_STATE_ORBIT)
	{
		// orbit
		Vector3 dir = getPosition() - mOrbitPos;
		dir.y = 0;
		dir = dir.crossProduct( Vector3::UNIT_Y );
		dir.normalise();
		setVelocity(dir * CONR("CRANE_SPEED"));

		// stay in the orbit zone
		if (orbitDistance() > CONR("CRANE_ORBIT_MAX"))
			setStateFlyIn();

		if (orbitDistance() < CONR("CRANE_ORBIT_MIN"))
			setStateFlyOut();

		// look for enemies to attack

		// TODO
		mTarget = 0;

	}
	else if (mState == CRANE_STATE_ATTACK)
	{
		if (mTarget && mTarget->isAlive())
		{
			// kamakazi!!!
			Vector3 dir = mTarget->getPosition() - getPosition();
			dir.normalise();
			setVelocity(dir * CONR("CRANE_SPEED"));
		}
		else
		{
			// return to base
			mTarget = 0;
			setStateFlyIn();
		}
	}

	setUpdateFlag();
}

void CraneThing::collided(Thing* e)
{
	if (e->isDamageable() && e->isAlive() && e->getTeamNum() != getTeamNum())
	{
		e->damage(CONI("CRANE_DAMAGE"), getTeamNum());
		Physics::getSingleton().addEffect(new CraneBlastEffect(getPosition()));
		setPosition(mOrbitPos);
		setStateSpawnFlyOut();
	}
}

void CraneThing::setStateFlyIn()
{
	mState = CRANE_STATE_FLY_IN;
				
	Vector3 dir = mOrbitPos - getPosition();
	dir.normalise();
	setVelocity(dir * CONR("CRANE_SPEED"));

	LogManager::getSingleton().logMessage("crane state fly in");
}

void CraneThing::setStateFlyOut()
{
	mState = CRANE_STATE_FLY_OUT;
				
	Vector3 dir = getPosition() - mOrbitPos;
	dir.normalise();
	setVelocity(dir * CONR("CRANE_SPEED"));

	LogManager::getSingleton().logMessage("crane state fly in");
}

void CraneThing::setStateSpawnFlyOut()
{
	mState = CRANE_STATE_SPAWN_FLY_OUT;
				
	Vector3 dir;
	dir.x = Math::RangeRandom(-1,1);
	dir.y = 0;
	dir.z = Math::RangeRandom(-1,1);
	dir.normalise();
	setVelocity(dir * CONR("CRANE_SPEED"));

	LogManager::getSingleton().logMessage("crane state fly out");
}

void CraneThing::setStateOrbit()
{
	mState = CRANE_STATE_ORBIT;
	
	setVelocity(Vector3(0,0,0));

	LogManager::getSingleton().logMessage("crane state orbit");
}

void CraneThing::setStateAttack()
{
	mState = CRANE_STATE_ATTACK;
	
	// kamakazi!!!
	Vector3 dir = mTarget->getPosition() - getPosition();
	dir.normalise();
	setVelocity(dir * CONR("CRANE_SPEED"));

	LogManager::getSingleton().logMessage("crane state attack");
}

}