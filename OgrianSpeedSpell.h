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
OgrianSpeedSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a speed spell

/*------------------------------------*/


#ifndef __OgrianSpeedSpell_H__
#define __OgrianSpeedSpell_H__

#include "OgrianSpell.h"
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

class SpeedSpell : public Spell
{
public:

	// make an instance of this spell
	virtual void cast(Vector3 pos, Vector3 dir)
	{
		Renderer::getSingleton().getCameraThing()->speed(CONR("SPEEDSPELL_DURATION"));
	}

	virtual String getReadyMaterial() { return String("Ogrian/SpellIcon/Speed/Ready"); }; 

	virtual String getEnabledMaterial() { return String("Ogrian/SpellIcon/Speed/Enabled"); }; 

	virtual String getDisabledMaterial() { return String("Ogrian/SpellIcon/Speed/Disabled"); }; 

	virtual Real getCastPeriod() { return CONR("SPEEDSPELL_CAST_PERIOD"); }

	virtual int getManaCost() { return CONI("SPEEDSPELL_MANA_COST"); }

	virtual String getString() { return CONS("NAME_SPEED"); }
};

}

#endif