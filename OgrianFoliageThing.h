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
OgrianFoliageThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: FoliageThing has the Foliage material, and dies when it touches mana. 
This is a very simple Thing, mainly used to demonstrate that collision detection works. 

/*------------------------------------*/

#ifndef __OgrianFoliageThing_H__
#define __OgrianFoliageThing_H__

#include <Ogre.h>
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

// extend RollingThing to automatically be placed on the ground
class FoliageThing : public Thing
{
public:
	FoliageThing(Real scale, Vector3 pos=Vector3(0,0,0)) 
		: Thing("Ogrian/PalmTree", "Foliage", true, scale, pos)
	{
		setShape(CYLINDER);
		setHeight(scale*1.5);
		setPosY(getGroundY() + scale*.45);
	}

	virtual ThingType getType()
	{
		return FOLIAGETHING;
	}

	virtual void collided(Thing* e)
	{
		// dies if it touches mana
		if (e->getType() == MANATHING)
		{
			setMaterial("Ogrian/PalmTreeDead");
			playSound("OgrianMedia/sounds/treedeath.wav");
			e->destroy();
		}
	}	
};

}

#endif