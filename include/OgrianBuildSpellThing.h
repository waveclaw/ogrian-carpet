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
OgrianBuildSpellThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This makes a castle or a tower

 *------------------------------------*/

#ifndef __OgrianBuildSpellThing_H__
#define __OgrianBuildSpellThing_H__

#include <Ogre.h>
#include "OgrianTimedThing.h"
#include "OgrianGame.h"
#include "OgrianConst.h"
#include "OgrianTowerThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianHud.h"
#include "OgrianBuildingHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class BuildSpellThing : public TimedThing
{
public:
	BuildSpellThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) 
		: TimedThing("Ogrian/Rock", SPRITE, "BuildSpell", false, CONR("BUILDSPELL_SCALE"), pos, SPHERE)
	{
		setTeamNum(teamNum);

		setVelocity(vel);
		playSound(Game::getSingleton().SOUND_WHOOSH);
		setFlickerPeriod(CONR("BUILDSPELL_FLICKER_PERIOD"));
		setRelativeExpirationTime(CONR("BUILDSPELL_LIFETIME"));

		setGroundScan(true);
	}

	virtual ThingType getType() { return BUILDTHING; }

	virtual void collidedGround();

	virtual void collided(Thing* e);
};

}

#endif
