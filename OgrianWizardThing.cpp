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
	visible?"WizardThing":"CameraThing", true, CAMERA_HEIGHT)
{
	if (visible)
	{
		getVisRep()->addPose("Ogrian/Wizard/");
		getVisRep()->setPose(0);
	}
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
		Physics::getSingleton().getTeam(getLastDamageSourceTeamNum())->incrementScore();

		if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
		{
			// find the wizard's player
			PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

			// kill it
			Multiplayer::getSingleton().serverSendText(" " , ID_DIE, player);
		}			
	}
	
	setHealth(WIZARD_HEALTH);
}

//----------------------------------------------------------------------------
	
}