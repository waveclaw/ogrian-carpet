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
OgrianSummonSpellThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a base class to be used by monsters for summoning

/*------------------------------------*/

#ifndef __OgrianSummonSpellThing_H__
#define __OgrianSummonSpellThing_H__

#include <Ogre.h>
#include "OgrianTimedThing.h"
#include "OgrianManaThing.h"
#include "OgrianCastle.h"
#include "OgrianTeam.h"
#include "OgrianGame.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianConst.h"

using namespace Ogre;

namespace Ogrian
{

class SummonSpellThing : public TimedThing
{
public:
	SummonSpellThing(int teamNum, ColourValue colour=ColourValue::White, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0)) 
		: TimedThing("Ogrian/Clay", SPRITE, "SummonSpell", false, CONR("SUMMONSPELL_SCALE"), pos, SPHERE)
	{
		setTeamNum(teamNum);
		setColour(colour);

		setVelocity(vel);
		playSound(Game::getSingleton().SOUND_WHOOSH);
		setFlickerPeriod(CONR("SUMMONSPELL_FLICKER_PERIOD"));
		setRelativeExpirationTime(CONR("SUMMONSPELL_LIFETIME"));

		setGroundScan(true);
	}

	virtual ThingType getType() { return SUMMONTHING; }

	// override this to summon a monster
	virtual void collidedGround() {}
};

}

#endif