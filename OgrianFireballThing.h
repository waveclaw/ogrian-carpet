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

Description: Fireballs fly in an arc and destroy() whatever they touch.
They self destruct when they hit the ground or another thing. 

/*------------------------------------*/

#ifndef __OgrianFireballThing_H__
#define __OgrianFireballThing_H__

#include <Ogre.h>
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

class FireballThing : public Thing
{
public:
	FireballThing(Vector3 pos, Vector3 vel) 
		: Thing("Ogrian/Fireball", SPRITE, "Fireball", false, FIREBALL_SCALE, pos, SPHERE)
	{
		setVelocity(vel);
		playSound("OgrianMedia/sounds/whoosh1.wav");
	}

	virtual ThingType getType()
	{
		return FIREBALLTHING;
	}	

	virtual void move(Real time)
	{
		// fall
		setVelocity(getVelocity() + Vector3(0, -FIREBALL_FALL_RATE * time, 0));
		Thing::move(time);

		// die when it hits the ground
		if (getGroundY() > getPosition().y) destroy();
	}

	virtual void collided(Thing* e)
	{
		// destroy it
		e->destroy();

		// self destruct
		destroy();
	}

	// go boom when destroyed
	virtual void destroy()
	{
		playSound("OgrianMedia/sounds/boom1.wav");

		Thing::destroy();
	}
};

}

#endif