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
OgrianFireballSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a fireball spell

 *------------------------------------*/


#ifndef __OgrianFireballSpell_H__
#define __OgrianFireballSpell_H__

#include "OgrianSpell.h"
#include "OgrianFireballThing.h"
#include "OgrianPhysics.h"

// akimbo fireball level
#define AKLEV 3

// firestorm level
#define STLEV 8

using namespace Ogre;

namespace Ogrian
{

class FireballSpell : public Spell
{
public:
	FireballSpell()
	{
		mLastSide = false;
	}

	// make an instance of this spell
	virtual void cast(Vector3 pos, Vector3 dir, WizardThing* caster, int level)
	{
		if (level < AKLEV) // normal fireball
		{
			dir.normalise();
			dir *= CONR("FIREBALL_SPEED");
		
			FireballThing* thing = new FireballThing(caster->getTeamNum(), caster->getColour(), pos, dir);
			Physics::getSingleton().addThing(thing);
		}
		else if (level < STLEV) // akimbo fireball
		{
			Vector3 offset = dir.crossProduct(Vector3::UNIT_Y);
			offset.normalise();
			offset*= CONR("WIZARD_SCALE")/2;
			if (mLastSide) offset *= -1;

			pos += offset;

			dir.normalise();
			dir *= CONR("FIREBALL_SPEED");
		
			FireballThing* thing = new FireballThing(caster->getTeamNum(), caster->getColour(), pos,dir);
			Physics::getSingleton().addThing(thing);

			mLastSide = !mLastSide;
		}
		else // firestorm
		{
			Real angle = Math::RangeRandom(0,2*Math::PI);
			Real uoff = Math::Cos(angle);
			Real voff = Math::Sin(angle);

			Vector3 u = Vector3::UNIT_Y;
			Vector3 v = dir.crossProduct(u);
			v.normalise();

			Vector3 offset = u*uoff + v*voff;
			offset*= CONR("WIZARD_SCALE")/2;

			pos += offset;

			dir.normalise();
			dir *= CONR("FIREBALL_SPEED");
		
			FireballThing* thing = new FireballThing(caster->getTeamNum(), caster->getColour(), pos,dir);
			Physics::getSingleton().addThing(thing);
		}
	}

	virtual String getReadyMaterial(int level) 
	{ 
		if (level < AKLEV) 
			return String("Ogrian/SpellIcon/Fireball/Ready"); 
		else if (level < STLEV) 
			return String("Ogrian/SpellIcon/AkimboFireball/Ready"); 
		else 
			return String("Ogrian/SpellIcon/Firestorm/Ready"); 
	}; 

	virtual String getEnabledMaterial(int level) 
	{ 
		if (level < AKLEV) 
			return String("Ogrian/SpellIcon/Fireball/Enabled"); 
		else if (level < STLEV) 
			return String("Ogrian/SpellIcon/AkimboFireball/Enabled"); 
		else
			return String("Ogrian/SpellIcon/Firestorm/Enabled"); 
	}; 

	virtual String getDisabledMaterial(int level) 
	{ 
		if (level < AKLEV) 
			return String("Ogrian/SpellIcon/Fireball/Disabled"); 
		else if (level < STLEV) 
			return String("Ogrian/SpellIcon/AkimboFireball/Disabled"); 
		else
			return String("Ogrian/SpellIcon/Firestorm/Disabled");
	}; 

	virtual Real getCastPeriod(int level) 
	{ 
		if (level < AKLEV) 
			return CONR("FIREBALL_CAST_PERIOD"); 
		else if (level < STLEV) 
			return CONR("FIREBALL_CAST_PERIOD")/2; 
		else
			return CONR("FIREBALL_CAST_PERIOD")/4; 
	}

	virtual int getManaCost(int level) 
	{ 
		return CONI("FIREBALL_MANA_COST"); 
	}

	virtual String getString(int level) 
	{ 
		if (level < AKLEV) 
			return CONS("NAME_FIREBALL"); 
		else if (level < STLEV) 
			return CONS("NAME_AKIMBO_FIREBALL");
		else
			return CONS("NAME_FIRESTORM");
	}


private:
	bool mLastSide;
};

}

#endif
