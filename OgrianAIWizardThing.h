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


#ifndef __OgrianAIWizardThing_H__
#define __OgrianAIWizardThing_H__

#include <Ogre.h>
#include "OgrianWizardThing.h"

using namespace Ogre;

namespace Ogrian
{

class AIWizardThing : public WizardThing
{
public:
	AIWizardThing(Vector3 pos) : WizardThing(true) 
	{
		mLastTime = 0;

		setPosition(pos);
		setColour(ColourValue::Red);
		setHealth(CONR("WIZARD_HEALTH"));

		Vector3 offset;
		Real wdo = CONR("WIZARD_DEATH_OFFSET");
		offset.x = Math::RangeRandom(-wdo, wdo);
		offset.z = Math::RangeRandom(-wdo, wdo);
		setPosition(getPosition() + offset);
	}

	// think
	virtual void move(Real time)
	{
		WizardThing::move(time);

		// only think periodically
		if (Time::getSingleton().getTime() < mLastTime + CONR("FIREBALL_CAST_PERIOD")*1000) 
			return;

		mLastTime = Time::getSingleton().getTime();

		WizardThing* enemy = static_cast<WizardThing*>(Renderer::getSingleton().getCameraThing());

		// face the enemy //
		
		// determine the direction
		Vector3 pos = getPosition();
		Vector3 epos = enemy->getPosition();
		float dir = atan2(epos.x - pos.x, epos.z - pos.z);
		
		// constrain the offset
		while (dir > Math::PI) dir -= 2*Math::PI;
		while (dir < -Math::PI) dir += 2*Math::PI;

		// set orientation
		setOrientation(dir);

		// circle strafe randomly //

		// set perpindicular velocity
		Vector3 vel = pos-epos;
		Vector3 fvel = vel; // note the original for fireballs
		vel = vel.crossProduct(Vector3::UNIT_Y);
		vel.normalise();
		vel *= CONR("CAMERA_MOVE_SPEED");

		// randomize direction
		if (Math::RangeRandom(-1,1) < 0) vel *= -1;

		setVelocity(vel);

		// fire fireballs //
		if (Math::RangeRandom(-1,1) < 0)
		{
			Vector3 fpos = getPosition();
			fvel.normalise();
			fvel *= -1;

			fpos += fvel*(CONR("WIZARD_SCALE") + CONR("FIREBALL_SCALE"))*1.1;
			fvel *= CONR("FIREBALL_SPEED");
		
			Physics::getSingleton().addThing(new FireballThing(-1, fpos,fvel));
		}
	}

	virtual void die()
	{
		WizardThing::die();

		Vector3 offset;
		Real wdo = CONR("WIZARD_DEATH_OFFSET");
		offset.x = Math::RangeRandom(-wdo, wdo);
		offset.z = Math::RangeRandom(-wdo, wdo);
		setPosition(getPosition() + offset);
	}

private:
	unsigned long mLastTime;
};

}

#endif