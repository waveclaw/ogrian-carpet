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
OgrianAkimboFireballSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a faster-firing fireball spell

/*------------------------------------*/


#ifndef __OgrianAkimboFireballSpell_H__
#define __OgrianAkimboFireballSpell_H__

#include "OgrianSpell.h"
#include "OgrianFireballThing.h"

using namespace Ogre;

namespace Ogrian
{

class AkimboFireballSpell : public Spell
{
public:
	AkimboFireballSpell()
	{
		mLastSide = false;
	}

	// make an instance of this spell
	virtual void cast(Vector3 pos, Vector3 dir)
	{
		Vector3 offset = dir.crossProduct(Vector3::UNIT_Y);
		offset.normalise();
		offset*= CONR("WIZARD_SCALE")/2;
		if (mLastSide) offset *= -1;

		pos += offset;

		dir.normalise();
		dir *= CONR("FIREBALL_SPEED");
	
		FireballThing* thing = new FireballThing(0, Renderer::getSingleton().getCameraThing()->getColour(), pos,dir);
		Physics::getSingleton().addThing(thing);

		mLastSide = !mLastSide;
	}

	virtual String getReadyMaterial() { return String("Ogrian/SpellIcon/AkimboFireball/Ready"); }; 

	virtual String getEnabledMaterial() { return String("Ogrian/SpellIcon/AkimboFireball/Enabled"); }; 

	virtual String getDisabledMaterial() { return String("Ogrian/SpellIcon/AkimboFireball/Disabled"); }; 

	virtual Real getCastPeriod() { return CONR("FIREBALL_CAST_PERIOD")/2; }

	virtual int getManaCost() { return CONI("FIREBALL_MANA_COST"); }

	virtual String getString() { return String("Akimbo    Fireball"); }

private:
	bool mLastSide;
};

}

#endif