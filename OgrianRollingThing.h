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
OgrianRollingThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: RollingThing is a thing that follows the terrain strictly. 

/*------------------------------------*/

#ifndef __OgrianRollingThing_H__
#define __OgrianRollingThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class RollingThing : public Thing
{
public:
	RollingThing(String material, String prefix="RollingThing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0) 
		: Thing(material, prefix, fixed_y, scale, x, y, z)
	{
		
	}
	virtual ThingType getType()
	{
		return ROLLINGTHING;
	}

	virtual void setPosition(const Vector3 pos)
	{
		setPosition(pos.x, pos.y, pos.z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		Thing::setPosition(x,
			HeightMap::getSingleton().getHeightAt(x,z) + getRadius(),
			z);
	}
};

}

#endif