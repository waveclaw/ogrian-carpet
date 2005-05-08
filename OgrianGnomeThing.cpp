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
OgrianGnomeThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The gnome attacks enemies by shooting fireballs at them. it also follows its wizard.

/*------------------------------------*/


#include "OgrianGnomeThing.h"
#include "OgrianFireballThing.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

GnomeThing::GnomeThing(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "GnomeThing", false, CONR("GNOME_SCALE"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setThinkPeriod(CONR("GNOME_THINK_PERIOD"));
	
	if (!Multiplayer::getSingleton().isClient())
	{
		setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

		// notify our wizard
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		WizardThing* wiz = (WizardThing*)Physics::getSingleton().getThing(team->getWizardUID());
		if (wiz) wiz->addGnome();

		// set up our formation offset
		if (wiz)
		{
			Real distance = CONR("GNOME_FORMATION_OFFSET");

			mFormationOffset = pos - wiz->getPosition();
			mFormationOffset.y = 0;

			if (mFormationOffset.length() > distance)
			{
				mFormationOffset.normalise();
				mFormationOffset *= distance;
			}
		}
	}	

	getVisRep()->addPose("Ogrian/Gnome/Sitting/");
	getVisRep()->addPose("Ogrian/Gnome/Attacking/");
	getVisRep()->setPose(0);

	setHealth(CONI("GNOME_HEALTH"));
	setMaxHealth(CONI("GNOME_HEALTH"));

	setPosY(getGroundY()+CONR("GNOME_SCALE")/2);

	setUpdateType(CONTINUOUS);
}

//----------------------------------------------------------------------------

// stay on the ground and in formation
void GnomeThing::move(Real time)
{
	DamageableThing::move(time);

	setPosY(getGroundY() + getHeight()/2);

	if (Multiplayer::getSingleton().isClient()) return;

	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team)
	{
		// stay in formation
		Thing* wiz = Physics::getSingleton().getThing(team->getWizardUID());
		if (wiz)
		{
			Vector3 vel(0,0,0);
			vel = (wiz->getPosition() + mFormationOffset) - getPosition();
			vel.y = 0;

			if (vel.length() > getWidth())
			{
				vel.normalise();
				vel *= CONR("GNOME_SPEED");
			}
			else vel = Vector3(0,0,0);

			setVelocity(vel);
		}
	}
}

//----------------------------------------------------------------------------

void GnomeThing::think()
{
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team)
	{
		// target the nearest enemy
		Thing* target = team->getNearestEnemy(this, CONR("GNOME_SIGHT_RANGE"));
		if (target) 
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

			// attack the enemy //

			// account for target movement
			Real travelTime = sphereDistance(target) / CONR("FIREBALL_SPEED");
			Vector3 targetOffset = target->getVelocity()*travelTime;

			// ignore the falling of wizards
			if (target->getType() == CAMERATHING || target->getType() == WIZARDTHING)
				targetOffset.y = 0;

			// shoot at the tips of buildings
			if (target->isBuilding())
				epos.y = epos.y + target->getHeight()/2;

			// calculate the trajectory
			Vector3 vel = (epos + targetOffset) - pos;
			vel.normalise();
			vel *= CONR("FIREBALL_SPEED");

			// shoot at it
			Physics::getSingleton().addThing(new FireballThing(getTeamNum(), team->getColour(),
				pos, vel, CONI("GNOME_DAMAGE"), true));

			// set the attack pose
			getVisRep()->setPose(1);
		}
		else
		{
			getVisRep()->setPose(0);			
		}
	}
}
	
//----------------------------------------------------------------------------
	
void GnomeThing::die()
{
	Team* team = Physics::getSingleton().getTeam(getTeamNum());

	if (team && team->getCastle())
	{
		// return the mana to the castle
		team->getCastle()->addMana(CONI("GNOME_COST") - CONI("GNOME_DROP"));
				
		// drop the rest of our mana
		ManaThing* mana = new ManaThing(CONI("GNOME_DROP"), getPosition());
		Physics::getSingleton().addThing(mana);
		mana->setTeamNum(getTeamNum());
	}
	else
	{
		// drop all of our mana
		ManaThing* mana = new ManaThing(CONI("GNOME_COST"), getPosition());
		Physics::getSingleton().addThing(mana);
		mana->setTeamNum(getTeamNum());
	}

	// notify our wizard
	WizardThing* wiz = (WizardThing*)team->getWizard();
	if (wiz) wiz->removeGnome();

	// self destruct
	destroy();
}
	
//----------------------------------------------------------------------------
}