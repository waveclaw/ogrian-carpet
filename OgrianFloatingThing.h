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
OgrianFloatingThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a type of thing that floats over the landscape.

/*------------------------------------*/

#ifndef __OgrianFloatingThing_H__
#define __OgrianFloatingThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianConstants.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class FloatingThing : public Thing
{
public:
	FloatingThing(String material, String prefix="FloatingThing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0) 
		: Thing(material, prefix, fixed_y, scale, x, y, z)
	{
		
	}
	virtual ThingType getType()
	{
		return FLOATINGTHING;
	}

	virtual void setPosition(const Vector3 pos)
	{
		setPosition(pos.x, pos.y, pos.z);
	}

	virtual void setVelocity(Real x, Real y, Real z)
	{
		Thing::setVelocity(x,mVel.y,z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		Real ground = HeightMap::getSingleton().getHeightAt(x,z) + getRadius();
		if (ground > mPos.y) 
		{
			Thing::setVelocity(mVel.x,0,mVel.y);
			Thing::setPosition(x,ground,z);
		}
		else 
		{
			Thing::setPosition(x,y,z);
		}
	}

	virtual void move(Real time)
	{
		// fall
		if (mVel.y > -FLOAT_FALL_MAX)
			Thing::setVelocity(mVel.x, mVel.y-FLOAT_GRAV*time, mVel.z);

		Thing::move(time);
	}
};

}

#endif