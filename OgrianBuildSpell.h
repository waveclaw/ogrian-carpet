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
OgrianBuildSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a fireball spell

/*------------------------------------*/


#ifndef __OgrianBuildSpell_H__
#define __OgrianBuildSpell_H__

#include "OgrianSpell.h"
#include "OgrianBuildSpellThing.h"

using namespace Ogre;

namespace Ogrian
{

class BuildSpell : public Spell
{
public:
	// make an instance of this spell
	virtual int makeThing(Vector3 pos, Vector3 dir)
	{
		dir.normalise();
		dir *= CONR("BUILDSPELL_SPEED");
	
		BuildSpellThing* thing = new BuildSpellThing(0, pos,dir);
		Physics::getSingleton().addThing(thing);
		return thing->getUID();
	}

	virtual String getReadyMaterial() { return String("Ogrian/SpellIcon/BuildSpell/Ready"); }; 

	virtual String getEnabledMaterial() { return String("Ogrian/SpellIcon/BuildSpell/Enabled"); }; 

	virtual String getDisabledMaterial() { return String("Ogrian/SpellIcon/BuildSpell/Disabled"); }; 
	
	virtual Real getCastPeriod() { return CONR("BUILDSPELL_CAST_PERIOD"); }

	virtual String getString() { return String("Build"); }
};

}

#endif