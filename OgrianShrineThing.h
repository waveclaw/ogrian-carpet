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
OgrianShrineThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a shrine which can be claimed to increase mana regen for a wizard

/*------------------------------------*/

#ifndef __OgrianShrineThing_H__
#define __OgrianShrineThing_H__

#include <Ogre.h>
#include "OgrianConst.h"
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class ShrineBallThing : public Thing
{
public:
	ShrineBallThing(Thing* shrine);

	virtual void setColour(ColourValue& colour);

	virtual void claim(int teamNum);
	
	virtual ThingType getType()	{ return SHRINEBALLTHING; }

private:
	Thing* mShrine;
};

/////////////////////////////////////////////////////////////////////////////
class ShrineThing : public Thing
{
public:
	ShrineThing(Vector3 pos=Vector3(0,0,0), int skin=0);

	virtual void setSkin(int skin);

	virtual void destroy();

	virtual void claim(int teamNum);

	virtual void setTeamNum(int teamNum);

	virtual void setColour(ColourValue& colour);

	virtual void setPosition(Vector3 pos);

	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return SHRINETHING; }
	
private:
	ShrineBallThing* mBall;
};

}
#endif