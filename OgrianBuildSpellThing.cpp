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
OgrianBuildSpellThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This makes a castle or a tower

/*------------------------------------*/

#include "OgrianBuildSpellThing.h"
#include "OgrianTowerThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianHud.h"
#include "OgrianBuildingHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

void BuildSpellThing::collidedGround()
{
	Team* team = Physics::getSingleton().getTeam(getTeamNum());

	// make sure its not in water
	if (getGroundY() < CONR("BUILDING_MIN_GROUNDY"))
	{
		// report the problem
		if (team->getWizardUID() == 0)
		{
			// send it to the HUD
			Hud::getSingleton().setMessage(CONS("BUILD_FAIL_WATER"), true);
		}
		else
		{
			// send a message to the right player
			PlayerID player = Multiplayer::getSingleton().getPlayerID(team->getWizardUID());
			Multiplayer::getSingleton().serverSendHudText(CONS("BUILD_FAIL_WATER"), player);
		}

		destroy();
		return;
	}
	

	if (team && !team->hasCastle())
	{
		// make sure its not too close to other buildings
		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);
			if (thing->isBuilding() && axisDistance(thing) < 6 * CONR("CASTLE_WIDTH"))
			{
				// report the problem
				if (team->getWizardUID() == 0)
				{
					// send it to the HUD
					Hud::getSingleton().setMessage(CONS("BUILD_FAIL_PROXIMITY"), true);
				}
				else
				{
					// send a message to the right player
					PlayerID player = Multiplayer::getSingleton().getPlayerID(team->getWizardUID());
					Multiplayer::getSingleton().serverSendHudText(CONS("BUILD_FAIL_PROXIMITY"), player);
				}

				destroy();
				return;
			}
		}

		// make a castle
		Vector3 pos = getPosition();
		pos = BuildingHeightMap::getSingleton().alignPosition(pos);
		Castle* castle = new Castle(getTeamNum(), pos);
		team->setCastle(castle);
	}
	
	else if (team && team->getCastle() && team->getCastle()->getMana() >= CONI("TOWER_COST"))
	{
		// make sure its not too close to other buildings
		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);
			if (thing->isBuilding() && axisDistance(thing) < 2*CONR("TOWER_WIDTH") + CONR("CASTLE_WIDTH"))
			{
				// report the problem
				if (team->getWizardUID() == Renderer::getSingleton().getCameraThing()->getUID())
				{
					// send it to the HUD
					Hud::getSingleton().setMessage(CONS("BUILD_FAIL_PROXIMITY"), true);
				}
				else
				{
					// send a message to the right player
					if (Multiplayer::getSingleton().isServer())
					{
						PlayerID player = Multiplayer::getSingleton().getPlayerID(team->getWizardUID());
						Multiplayer::getSingleton().serverSendHudText(CONS("BUILD_FAIL_PROXIMITY"), player);
					}
				}

				destroy();
				return;
			}
		}

		// make a tower
		Vector3 pos = getPosition();
		pos = BuildingHeightMap::getSingleton().alignPosition(pos);
		Physics::getSingleton().addThing(new TowerThing(getTeamNum(), pos));

		// remove the mana from the castle
		Castle* castle = team->getCastle();
		castle->setMana(castle->getMana() - CONI("TOWER_COST"));
	}

	// self destruct
	destroy();
}

void BuildSpellThing::collided(Thing* e)
{
	if (!isAlive()) return;

	if (e->getType() == TOWERTHING && e->getTeamNum() == getTeamNum())
	{
		// unbuild the tower
		((TowerThing*)e)->unbuild();

		destroy();
	}
}

}