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
OgrianTimedThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: TimedThing self-destructs after a specified time

 *------------------------------------*/

#ifndef __OgrianTimedThing_H__
#define __OgrianTimedThing_H__

#include <Ogre.h>
#include "OgrianClock.h"
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

class TimedThing : public Thing
{
public:

	TimedThing(String material, ThingVisRep visrep=SPRITE, String prefix="Thing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE)
		: Thing(material, visrep, prefix, fixed_y, scale, pos, shape)
	{
		mExpirationTime = 0;
	}

	// set expiration time in absolute time in microseconds
	virtual void setAbsoluteExpirationTime(Time time)
	{
		mExpirationTime = time;
	}

	// set expiration time in seconds from now
	virtual void setRelativeExpirationTime(Real time)
	{
/* ahem.  matching parameters? Try again. -- jdpowell 20050602 */
		mExpirationTime = ((Time) time*1000) + Clock::getSingleton().getTime();
	}

	// check to see if the time is up
	virtual void move(Real time)
	{
		Thing::move(time);

		if (mExpirationTime > 0 && Clock::getSingleton().getTime() > mExpirationTime && isAlive()) 
			destroy();
	}

private:
	Time mExpirationTime;
};

}

#endif
