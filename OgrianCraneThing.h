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
OgrianCraneThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the ManaThing is a ball of mana that derives from FloatingThing. 
They drift downhill and combine when they touch. 
They have some amount of mana.
When they combine, one transfers all of its amount to the other.

/*------------------------------------*/


#ifndef __OgrianCraneThing_H__
#define __OgrianCraneThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

// extend floatingThing for floating behavior
class CraneThing : public DamageableThing
{
public:
	CraneThing(int teamNum, Vector3 orbitPos) 
		: DamageableThing("Ogrian/Clear", ORIENTEDSPRITE, "CraneThing", false, 1, pos, SPHERE)
	{
		mOrbitPos = orbitPos;

	}
private:
	Vector3 mOrbitPos;
};

}

#endif