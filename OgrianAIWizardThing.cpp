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

/*------------------------------------*/

#include <Ogre.h>
#include "OgrianAIWizardThing.h"
#include "OgrianManaThing.h"
#include "OgrianPhysics.h"
#include "OgrianHeightMap.h"
#include "OgrianGame.h"

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

	setSkin(skin);
	setPosition(pos);
	setColour(colour);
	setHealth(CONR("WIZARD_HEALTH"));

	setThinkPeriod(thinkPeriod);
}

//----------------------------------------------------------------------------

// think
void AIWizardThing::think()
{
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (!team) return;
	
	Thing* target = team->getNearestEnemy(this, mSightRange*100);

	// find nearest mana

	// choose mode

	if (target) 
	{
		think_faceTarget(target);

		if (sphereDistance(target) < mSightRange)
		{
			think_attack(target);
		}

		if (sphereDistance(target) < mSightRange/2)
		{
			think_circleStrafe(target);
		}
		else
		{
			think_moveTo(target);
		}
	}
}

//----------------------------------------------------------------------------

void AIWizardThing::think_faceTarget(Thing* target)
{
	// face the enemy //

	// determine the direction
	Vector3 pos = getPosition();
	Vector3 epos = target->getPosition();
	float dir = atan2(epos.x - pos.x, epos.z - pos.z);
	
	// constrain the offset
	while (dir > Math::PI) dir -= 2*Math::PI;
	while (dir < -Math::PI) dir += 2*Math::PI;

	// set orientation
	setOrientation(dir);
}

//----------------------------------------------------------------------------

void AIWizardThing::think_moveTo(Thing* target)
{
	// determine the direction
	Vector3 pos = getPosition();
	Vector3 epos = target->getPosition();

	// move towards the target
	Vector3 vel = epos-pos;
	vel.normalise();
	vel *= CONR("CAMERA_MOVE_SPEED");

	setVelocity(vel);
}

//----------------------------------------------------------------------------

void AIWizardThing::think_circleStrafe(Thing* target)
{
	// circle strafe randomly //

	// determine the direction
	Vector3 pos = getPosition();
	Vector3 epos = target->getPosition();

	// set perpindicular velocity
	Vector3 vel = pos-epos;
	vel = vel.crossProduct(Vector3::UNIT_Y);
	vel.normalise();
	vel *= CONR("CAMERA_MOVE_SPEED");

	// randomize direction
	if (Math::RangeRandom(-1,1) < 0) vel *= -1;

	setVelocity(vel);
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

	subtractActiveMana(CONI("FIREBALL_MANA_COST"));
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
