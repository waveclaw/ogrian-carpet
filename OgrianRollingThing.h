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
	RollingThing(String material, String prefix="RollingThing", bool fixed_y=false, Real scale=1, Vector3 pos=Vector3(0,0,0))
		: Thing(material, prefix, fixed_y, scale, pos)
	{
		
	}
	virtual ThingType getType()
	{
		return ROLLINGTHING;
	}

	// stay on the ground
	virtual void setPosition(Vector3 pos)
	{
		pos.y = HeightMap::getSingleton().getHeightAt(pos.x,pos.z) + getHeight()/2;
		Thing::setPosition(pos);
	}
};

}

#endif