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
OgrianPortalThing.h
Original Author: Mike Prosser
Additional Authors: 


Description: the Portal teleports its wizard to the next tower

 *------------------------------------*/


#ifndef __OgrianPortalThing_H__
#define __OgrianPortalThing_H__

#include <Ogre.h>
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

class PortalThing : public Thing
{
public:
	PortalThing(Thing* tower=0, Vector3 pos = Vector3(0,0,0));

	// When it touches its wizard, teleport it
	virtual void collided(Thing* e);

	virtual ThingType getType()	{	return PORTALTHING; }

private:
	Thing* mTower;
};

}

#endif
