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
OgrianSentinelSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This summons a sentinel

/*------------------------------------*/


#ifndef __OgrianSentinelSpell_H__
#define __OgrianSentinelSpell_H__

#include "OgrianSpell.h"
#include "OgrianSentinelThing.h"

using namespace Ogre;

namespace Ogrian
{

class SentinelSpell : public Spell
{
public:
	// make an instance of this spell
	virtual void cast(Vector3 pos, Vector3 dir)
	{
		dir.normalise();
		dir *= CONR("SUMMONSPELL_SPEED");
	
		SentinelSummonSpellThing* thing = new SentinelSummonSpellThing(0, pos,dir);
		Physics::getSingleton().addThing(thing);
	}

	virtual String getReadyMaterial() { return String("Ogrian/SpellIcon/Sentinel/Ready"); }; 

	virtual String getEnabledMaterial() { return String("Ogrian/SpellIcon/Sentinel/Enabled"); }; 

	virtual String getDisabledMaterial() { return String("Ogrian/SpellIcon/Sentinel/Disabled"); }; 
	
	virtual Real getCastPeriod() { return CONR("SUMMONSPELL_CAST_PERIOD"); }

	virtual int getManaCost() { return 0; }

	virtual String getString() { return String("Summon Sentinel"); }
};

}

#endif