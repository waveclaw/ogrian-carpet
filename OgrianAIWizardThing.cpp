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
OgrianAIWizardThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: A computer controlled Wizard

 *------------------------------------*/

#include <Ogre.h>
#include "OgrianAIWizardThing.h"
#include "OgrianManaThing.h"
#include "OgrianPhysics.h"
#include "OgrianHeightMap.h"
#include "OgrianGame.h"

#define MODE_NULL 0
#define MODE_GATHER 1
#define MODE_ATTACK 2

#define MOVE_NONE 0
#define MOVE_FORWARD 1
#define MOVE_BACK 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

AIWizardThing::AIWizardThing(Vector3 pos, ColourValue colour, String brain) 
	: WizardThing(true, 0) 
{
	mConfig.load(String("Media/brains/") + brain + ".txt");
	int skin = atoi(mConfig.getSetting("BOT_SKIN").c_str());
	Real thinkPeriod = atof(mConfig.getSetting("BOT_THINK_PERIOD").c_str());
	mSightRange = atof(mConfig.getSetting("BOT_SIGHT_RANGE").c_str());
	mNextCastTime = 0;
	mMovingDirection = MOVE_NONE;

	setSkin(skin);
	setPosition(pos);
	setColour(colour);
	setHealth(CONR("WIZARD_HEALTH"));

	setThinkPeriod(thinkPeriod);
}

//----------------------------------------------------------------------------

Vector3 AIWizardThing::getDirection()
{
	return mDir;
}

//----------------------------------------------------------------------------

// move
void AIWizardThing::move(Real time)
{
	switch (mMovingDirection)
	{
		case MOVE_FORWARD: moveForward(); break;
		case MOVE_BACK: moveBack(); break;
		case MOVE_LEFT: moveLeft(); break;
		case MOVE_RIGHT: moveRight(); break;
	}

	WizardThing::move(time);
}

//----------------------------------------------------------------------------

// think
void AIWizardThing::think()
{
	if (isGhost()) return;

	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (!team) return;
	
	// find nearest enemy
	Thing* enemy = team->getNearestEnemy(this, mSightRange*100);

	// find nearest mana
	Thing* mana = think_findNearestMana(mSightRange*100);

	// choose mode
	if (!enemy && !mana)
	{
		mMode = MODE_NULL;
	}
	else if (enemy && !mana)
	{
		// if there's an enemy, but no mana, attack
		mMode = MODE_ATTACK;
	}
	else if (mana && !enemy)
	{
		// if there's mana, but no enemy, gather
		mMode = MODE_GATHER;
	}
	else if (mana && enemy)
	{
		// if there's both, choose the closest
		if (sphereDistance(mana) > sphereDistance(enemy))
			mMode = MODE_ATTACK;
		else
			mMode = MODE_GATHER;
	}


	// peform the chosen mode
	if (mMode == MODE_GATHER)
	{
		think_faceTarget(mana);

		// cast when in range
		if (sphereDistance(mana) < mSightRange/2)
			think_castClaim(mana);
		else
			think_moveTo(mana);
	}

	else if (mMode == MODE_ATTACK) 
	{
		think_faceTarget(enemy);

		// attack if in range
		if (sphereDistance(enemy) < mSightRange)
			think_attack(enemy);

		// circle strafe or chase
		if (sphereDistance(enemy) < mSightRange/2)
			think_circleStrafe(enemy);
		else
			think_moveTo(enemy);
	}
}

//----------------------------------------------------------------------------

void AIWizardThing::think_castClaim(Thing* target)
{
	// see if we are able to cast yet
	if (Clock::getSingleton().getTime() < mNextCastTime) return;

	Vector3 startPos = getPosition();

	// account for target movement
	Real claimTravelTime = sphereDistance(target) / CONR("CLAIMSPELL_SPEED");
	Vector3 targetOffset = target->getVelocity()*claimTravelTime;

	Vector3 vel = (target->getPosition() + targetOffset) - startPos;
	vel.normalise();

	mClaimSpell.cast(startPos, vel, this, 0);
	mNextCastTime = Clock::getSingleton().getTime() + mClaimSpell.getCastPeriod(0)*1000;
}

//----------------------------------------------------------------------------

Thing* AIWizardThing::think_findNearestMana(Real range)
{
	Thing* target = 0;
	Real bestDist = range;
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* candidate = Physics::getSingleton().getThingByIndex(i);
		int type = candidate->getType();
		if (candidate 
			&& (type == MANATHING || type == SHRINETHING)
			&& cylinderDistance(candidate) < bestDist
			&& candidate->isAlive() )
		{
			// only claim mana and shrines of a different colour
			if (candidate->getColour() != getColour())
			{
				target = candidate;
				bestDist = cylinderDistance(candidate);
			}
		}
	}

	return target;
}

//----------------------------------------------------------------------------

void AIWizardThing::think_faceTarget(Thing* target)
{
	// face the target //

	// determine the direction
	Vector3 pos = getPosition();
	Vector3 epos = target->getPosition();
	float dir = atan2(epos.x - pos.x, epos.z - pos.z);
	
	// constrain the offset
	while (dir > Math::PI) dir -= 2*Math::PI;
	while (dir < -Math::PI) dir += 2*Math::PI;

	// set orientation
	setOrientation(dir);

	mDir.x = sin(dir);
	mDir.z = cos(dir);
}

//----------------------------------------------------------------------------

void AIWizardThing::think_moveTo(Thing* target)
{
	// move forward
	mMovingDirection = MOVE_FORWARD;

	// TODO: A* pathfinding 
}

//----------------------------------------------------------------------------

void AIWizardThing::think_circleStrafe(Thing* target)
{
	// circle strafe randomly //

	// randomize direction
	if (Math::RangeRandom(-1,1) < 0)
	{
		mMovingDirection = MOVE_LEFT;
	}
	else
	{
		mMovingDirection = MOVE_RIGHT; 
	}

}

//----------------------------------------------------------------------------

void AIWizardThing::think_attack(Thing* target)
{
	// attack the enemy //

	// see if we have enough mana
	if (getActiveMana() < CONI("FIREBALL_MANA_COST")) return;

	// see if we are able to cast yet
	if (Clock::getSingleton().getTime() < mNextCastTime) return;

	Vector3 pos = getPosition();
	Vector3 epos = target->getPosition();

	// account for target movement
	Real claimTravelTime = sphereDistance(target) / CONR("FIREBALL_SPEED");
	Vector3 targetOffset = target->getVelocity()*claimTravelTime;

	// ignore the falling of wizards
	if (target->getType() == CAMERATHING || target->getType() == WIZARDTHING)
		targetOffset.y = 0;

	// shoot at the tips of buildings
	if (target->isBuilding())
		epos.y = epos.y + target->getHeight()/2;

	// calculate the trajectory
	Vector3 vel = (epos + targetOffset) - pos;

	// cast it
	mFireballSpell.cast(pos, vel, this, getLevel());
	subtractActiveMana(mFireballSpell.getManaCost(getLevel()));
	mNextCastTime = Clock::getSingleton().getTime() + mFireballSpell.getCastPeriod(getLevel())*1000;
}

//----------------------------------------------------------------------------

void AIWizardThing::die()
{
	WizardThing::die();

	Castle* castle = 0;
	Team* team = getTeam();

	if (team) 
		castle = team->getCastle();

	if (castle)
		setPosition(castle->getPosition());
}

//----------------------------------------------------------------------------

}
