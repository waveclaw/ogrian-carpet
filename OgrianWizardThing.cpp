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
OgrianWizardThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The wizard thing is the superclass of the CameraThing

/*------------------------------------*/


#include "OgrianWizardThing.h"
#include "OgrianPhysics.h"
#include "OgrianMultiplayer.h"


using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------
	
WizardThing::WizardThing(bool visible) 
	: DamageableThing("Ogrian/Clear", visible?ORIENTEDSPRITE:SPRITE, 
	visible?"WizardThing":"CameraThing", true, CONR("CAMERA_HEIGHT"))
{
	mBar = 0;

	if (visible)
	{
		getVisRep()->addPose("Ogrian/Wizard/");
		getVisRep()->setPose(0);
		
		mBar = new HealthBarEffect(getPosition(), getHeight());
		Physics::getSingleton().addEffect(mBar);
	}

	setUpdateType(CONTINUOUS);
}

//----------------------------------------------------------------------------

void WizardThing::setColour(ColourValue& colour)
{
	DamageableThing::setColour(colour);
	if (mBar) mBar->setColour(colour);
}

//----------------------------------------------------------------------------

void WizardThing::setHealth(int health)
{
	DamageableThing::setHealth(health);

	if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
	{
		// find the wizard's player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

		// update it
		Multiplayer::getSingleton().serverSendText(String("Health: ") << health , ID_SETHEALTH, player);
	}
}
//----------------------------------------------------------------------------

void WizardThing::die()
{
	DamageableThing::die();

	if (!Multiplayer::getSingleton().isClient())
	{
		Team* team = Physics::getSingleton().getTeam(getLastDamageSourceTeamNum());
		if (team > 0) team->incrementScore();
		else LogManager::getSingleton().logMessage(String("ERROR: damage source not found: ") 
			<< getLastDamageSourceTeamNum());

		if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
		{
			// find the wizard's player
			PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

			// kill it
			Multiplayer::getSingleton().serverSendText(" " , ID_DIE, player);
		}			
	}
	
	setHealth(CONI("WIZARD_HEALTH"));
}

//----------------------------------------------------------------------------

// ignore external up/down velocity changes
void WizardThing::setVelocity(Vector3 vel)
{
	vel.y = getVelY();
	Thing::setVelocity(vel);
}

//----------------------------------------------------------------------------
	
void WizardThing::move(Real time)
{
	// fall
	if (getVelY() > -CONR("CAMERA_FALL_MAX"))
	{
		setVelY(getVelY() - CONR("CAMERA_GRAV")*time);
	}

	// follow the landscape
	Vector3 pos = getPosition();
	float ground = getGroundY(pos) + CONR("CAMERA_HEIGHT");
			
	if (ground > getPosY()) 
	{
		setVelY(0);
		pos.y = ground;
	}
	else 
	{
		pos.y = getPosY();
	}
	setPosition(pos);

	DamageableThing::move(time);

	// update health bar
	if (mBar)
		mBar->update(getPosition(), getHealth()/100.0*getWidth());
}

//----------------------------------------------------------------------------

void WizardThing::destroy()
{
	if (mBar) mBar->destroy();
	mBar = 0;
}

//----------------------------------------------------------------------------

}