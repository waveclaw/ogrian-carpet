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
OgrianAlbatrossThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The albatross is like a cruise missile, it just flies straight until it hits something.

/*------------------------------------*/


#ifndef __OgrianAlbatrossThing_H__
#define __OgrianAlbatrossThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class AlbatrossThing : public DamageableThing
{
public:
	AlbatrossThing(int teamNum, Vector3 pos=Vector3(0,0,0), Vector3 vel=Vector3(0,0,0));

	virtual ThingType getType() { return ALBATROSSTHING; }

	virtual void collided(Thing* e);
	virtual void collidedGround();
	virtual void think();
	virtual void clientThink();
	virtual void die();

private:
	bool mLastFlap;
	Time mDeathTime;
};

}

#endif