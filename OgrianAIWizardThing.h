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
#include "OgrianFireballThing.h"
#include "OgrianManaThing.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

class AIWizardThing : public WizardThing
{
public:
	AIWizardThing(Vector3 pos, int skin) : WizardThing(true, skin) 
	{
		mNextTime = 0;

		setPosition(pos);

		ColourValue colour;
		colour.r = Math::RangeRandom(0,.95);
		colour.g = Math::RangeRandom(0,.95);
		colour.b = Math::RangeRandom(0,.95);
		setColour(colour);

		setHealth(CONR("WIZARD_HEALTH"));
	}

	// think
	virtual void move(Real time)
	{
		WizardThing::move(time);

		// only think periodically
		if (Clock::getSingleton().getTime() < mNextTime) 
			return;

		mNextTime = Clock::getSingleton().getTime() + CONT("FIREBALL_CAST_PERIOD")
			*Math::RangeRandom(1,1.3);

		// find the nearest wizard // 

		WizardThing* enemy = 0;
		Real bestDist = -1;
		int bestWuid = -1;

		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
		{
			int wuid = Physics::getSingleton().getTeam(i)->getWizardUID();
			if (wuid != getUID()) // if its not me
			{
				enemy = static_cast<WizardThing*>(Physics::getSingleton().getThing(wuid));
				if (enemy == 0)
				{
					std::ostringstream num("");
					num << wuid;
					LogManager::getSingleton().logMessage(String("ERROR, wuid not found for AI:") + num.str());
				}
				else
				{
					Real dist = axisDistance(enemy);
					if (dist < bestDist || bestWuid == -1) // if its the closest so far, or the first
					{
						bestDist = dist; // its the new closest 
						bestWuid = wuid;
					}
				}
			}
		}

		if (bestWuid == -1) return;
		enemy = static_cast<WizardThing*>(Physics::getSingleton().getThing(bestWuid));

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

		Vector3 vel;

		if (bestDist < CONR("FIREBALL_SPEED") * CONR("FIREBALL_LIFETIME"))
		{
			// circle strafe randomly //

			// set perpindicular velocity
			vel = pos-epos;
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
			
				Physics::getSingleton().addThing(new FireballThing(-1, getColour(), fpos,fvel));
			}
		}
		else
		{
			// move towards the target
			vel = epos-pos;
			vel.normalise();
			vel *= CONR("CAMERA_MOVE_SPEED");

			setVelocity(vel);
		}
	}

	virtual void die()
	{
		WizardThing::die();

		// drop a manathing
		if (CONI("BOT_MANA_DROP") > 0)
			Physics::getSingleton().addThing(new ManaThing(CONI("BOT_MANA_DROP"), getPosition()));

		// respawn at a random location
		Vector3 pos;
		Real size = HeightMap::getSingleton().getWorldSize();
		pos.x = Math::RangeRandom(0, size);
		pos.z = Math::RangeRandom(0, size);
		pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z) + getHeight();
		setPosition(pos);
	}

private:
	Time mNextTime;
};

}

#endif