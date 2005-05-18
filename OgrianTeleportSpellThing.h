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
OgrianTeleportSpellThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: TeleportSpellThing is a thing that helps the teleport spell work properly in multiplayer

/*------------------------------------*/

#ifndef __OgrianTeleportSpellThing_H__
#define __OgrianTeleportSpellThing_H__

#include <Ogre.h>
#include "OgrianClock.h"
#include "OgrianThing.h"
#include "OgrianMultiplayer.h"

using namespace Ogre;

namespace Ogrian
{

class TeleportSpellThing : public TimedThing
{
public:

	TeleportSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0))
		: TimedThing("Ogrian/Swirl", SPRITE, "PortalThing", false, CONR("PORTAL_SCALE"), pos, SPHERE)
	{
		if (!Multiplayer::getSingleton().isClient())  
		{
			// get the castle
			Team* team = Physics::getSingleton().getTeam(teamNum);
			Thing* castle = team->getCastle();
			Thing* wiz = Physics::getSingleton().getThing(team->getWizardUID());

			setColour(team->getColour());

			// teleport the wizard back to his castle=
			if (wiz->getType() == CAMERATHING)
				wiz->setPosition(castle->getPosition());
			else if (Multiplayer::getSingleton().isServer())
				Multiplayer::getSingleton().teleportWizard(wiz, castle->getPosition());
			else // bot
				wiz->setPosition(castle->getPosition());

			// play the teleport sound
			wiz->playSound(Game::getSingleton().SOUND_TELEPORT, true);

			// teleport his posse back to his castle
			for (int i=0; i<Physics::getSingleton().numThings(); i++)
			{
				Thing* thing = Physics::getSingleton().getThingByIndex(i);
				if (thing->getTeamNum() == wiz->getTeamNum() 
					&& (thing->getType() == GNOMETHING || thing->getType() == TICKTHING))
				{
					thing->setPosition(castle->getPosition());
				}
			}
		}
		
		setRelativeExpirationTime(CONR("TELEPORTSPELL_PORTAL_TIME"));
		setFlickerPeriod(CONR("TELEPORTSPELL_PORTAL_FLICKER_PERIOD"));

		// play a teleport sound
		playSound(Game::getSingleton().SOUND_TELEPORT);
	}

	virtual ThingType getType()	{ return TELEPORTSPELLTHING; }

};

}

#endif