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

// extend Thing
class FloatingThing : public Thing
{
public:
	FloatingThing(String material, String prefix="FloatingThing", bool fixed_y=false, Real scale=1, Vector3 pos=Vector3(0,0,0)) 
		: Thing(material, prefix, fixed_y, scale, pos)
	{
		
	}

	virtual ThingType getType()
	{
		return FLOATINGTHING;
	}

	// stay above ground
	virtual void setPosition(Vector3 pos)
	{
		Real ground = HeightMap::getSingleton().getHeightAt(pos.x,pos.z) + getRadius();
		if (ground > mPos.y) 
		{
			mVel.y = 0;
			pos.y = ground;
			Thing::setVelocity(mVel);
			Thing::setPosition(pos);
		}
		else 
		{
			Thing::setPosition(pos);
		}
	}

	// ignore external changes in up/down velocity
	virtual void setVelocity(Vector3 vel)
	{
		vel.y = mVel.y;
		Thing::setVelocity(vel);
	}


	// fall
	virtual void move(Real time)
	{
		if (mVel.y > -FLOAT_FALL_MAX)
		{
			mVel.y -= FLOAT_GRAV*time;
			Thing::setVelocity(mVel);
		}

		Thing::move(time);
	}
};

}

#endif