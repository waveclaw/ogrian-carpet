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
	FloatingThing(String material, ThingVisRep visrep=SPRITE, String prefix="FloatingThing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE) 
		: Thing(material, visrep, prefix, fixed_y, scale, pos, shape)
	{
		
	}

	virtual ThingType getType()
	{
		return FLOATINGTHING;
	}

	// stay above ground
	virtual void setPosition(Vector3 pos)
	{
		// start by setting the new position
		Thing::setPosition(pos);

		// if we are now underground, go up to the surface
		Real ground = getGroundY() + getHeight()/2;
		if (ground > getPosY()) 
		{
			setVelY(0);
			pos.y = ground;
			Thing::setPosition(pos);
		}
	}

	// ignore external changes in up/down velocity
	virtual void setVelocity(Vector3 vel)
	{
		vel.y = getVelY();
		Thing::setVelocity(vel);
	}


	// fall
	virtual void move(Real time)
	{
		if (getVelY() > -CONR("FLOAT_FALL_MAX"))
		{
			Thing::setVelY(getVelY() - CONR("FLOAT_GRAV")*time);
		}

		Thing::move(time);
	}
};

}

#endif