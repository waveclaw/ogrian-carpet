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
OgrianPortalThing.cpp
Original Author: Mike Prosser
Additional Authors: 


Description: the Portal teleports its wizard to the next tower

 *------------------------------------*/


#include "OgrianPortalThing.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

PortalThing::PortalThing(Thing* tower, Vector3 pos)
	: Thing("Ogrian/Swirl", SPRITE, "PortalThing", false, CONR("PORTAL_SCALE"), pos, SPHERE)
{
	if (!Multiplayer::getSingleton().isClient())
	{
		mTower = tower;
		setTeamNum(mTower->getTeamNum());
		setColour(mTower->getColour());
	}
}


//----------------------------------------------------------------------------

// When it touches its wizard, teleport it
void PortalThing::collided(Thing* e)
{
	if (e->getType() == WIZARDTHING || e->getType() == CAMERATHING)
		if (e->getColour() == getColour())
		{
			// find the next tower
			Thing* nextTower = 0;

			// build a list of our towers
			std::vector<Thing*> towers;
			Physics* physics = &Physics::getSingleton();
			for (int i=0; i<physics->numThings(); i++)
			{
				Thing* thing = physics->getThingByIndex(i);
				if ((thing->getType() == TOWERTHING || thing->getType() == CASTLEFLAGTHING)
					&& thing->getColour() == getColour())
				{
					towers.push_back(thing);
				}
			}

			// if there are not enough towers, return
			if (towers.size() <= 1) return;

			// find this tower
			for (int i=0; i<(int)towers.size()-1; i++)
			{
				if (towers[i] == mTower)
				{
					nextTower = towers[i+1];
					break;
				}
			}

			// if this is the last tower
			if (nextTower == 0)
			{
				// go to the first tower
				nextTower = towers[0];
			}

			// teleport the wizard to it
			if (e->getType() == CAMERATHING)
				e->setPosition(nextTower->getPosition());
			else if (Multiplayer::getSingleton().isServer())
				Multiplayer::getSingleton().teleportWizard(e, nextTower->getPosition());

			// play a teleport sound
			playSound(Game::getSingleton().SOUND_TELEPORT, true);
			e->playSound(Game::getSingleton().SOUND_TELEPORT, true);
		}
}

//----------------------------------------------------------------------------

}
