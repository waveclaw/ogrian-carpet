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
OgrianHeightMap.h
Original Author: Mike Prosser
Additional Authors: 

Description:

/*------------------------------------*/

#ifndef __OgrianFoliageThing_H__
#define __OgrianFoliageThing_H__

#include <Ogre.h>
#include "OgrianRollingThing.h"

using namespace Ogre;

namespace Ogrian
{

class FoliageThing : public RollingThing
{
public:
	FoliageThing(String material, Real scale, Real x=0, Real y=0, Real z=0) 
		: RollingThing(material, "Foliage", true, scale, x, y, z)
	{
	}

	virtual ThingType getType()
	{
		return FOLIAGETHING;
	}

	virtual void collided(Thing* e)
	{
		// dies if it touches mana
		if (e->getType() == MANATHING)
			setMaterial("Ogrian/PalmTreeDead");
	}	
};

}

#endif