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
OgrianAlbatrossSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This summons an albatross

 *------------------------------------*/


#ifndef __OgrianAlbatrossSpell_H__
#define __OgrianAlbatrossSpell_H__

#include "OgrianSpell.h"
#include "OgrianRenderer.h"
#include "OgrianPhysics.h"
#include "OgrianAlbatrossThing.h"

using namespace Ogre;

namespace Ogrian
{

class AlbatrossSpell : public Spell
{
public:

	// make an instance of this spell
	virtual void cast(Vector3 pos, Vector3 dir, WizardThing* caster, int level)
	{
		dir.normalise();
		dir *= CONR("ALBATROSS_SPEED");

		AlbatrossThing* thing = new AlbatrossThing(caster->getTeamNum(), caster->getColour(), pos, dir);
		Physics::getSingleton().addThing(thing);
	}

	virtual String getReadyMaterial(int level) { return String("Ogrian/SpellIcon/Albatross/Ready"); }; 

	virtual String getEnabledMaterial(int level) { return String("Ogrian/SpellIcon/Albatross/Enabled"); }; 

	virtual String getDisabledMaterial(int level) { return String("Ogrian/SpellIcon/Albatross/Disabled"); }; 

	virtual Real getCastPeriod(int level) { return CONR("SUMMONSPELL_CAST_PERIOD"); }

	virtual int getManaCost(int level) { return CONI("ALBATROSS_COST"); }

	virtual String getString(int level) { return CONS("NAME_ALBATROSS"); }
};

}

#endif