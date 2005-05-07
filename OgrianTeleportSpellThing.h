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

class TeleportSpellThing : public Thing
{
public:

	TeleportSpellThing(int teamNum)
		: Thing("Ogrian/Clear")
	{
		// get the castle
		Team* team = Physics::getSingleton().getTeam(teamNum);
		Thing* castle = team->getCastle();
		Thing* wiz = Physics::getSingleton().getThing(team->getWizardUID());

		// teleport the wizard back to his castle=
		if (wiz->getType() == CAMERATHING)
			wiz->setPosition(castle->getPosition());
		else if (Multiplayer::getSingleton().isServer())
			Multiplayer::getSingleton().teleportWizard(wiz, castle->getPosition());
		else // bot
			wiz->setPosition(castle->getPosition());


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
		
		destroy();
	}

	virtual ThingType getType()	{ return TELEPORTSPELLTHING; }

};

}

#endif