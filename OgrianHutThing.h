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
OgrianHut.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a hut which can be claimed to increase mana regen for a wizard

/*------------------------------------*/

#ifndef __OgrianHut_H__
#define __OgrianHut_H__

#include <Ogre.h>
#include "OgrianMultiplayer.h"
#include "OgrianConst.h"
#include "OgrianModel.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class HutBall : public Thing
{
public:
	HutBall()
		: Thing("Ogrian/HutBall", SPRITE, "HutBall", true, CONR("HUT_BALL_WIDTH"), Vector3(0,0,0), SPHERE)
	{

	}
	
	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////
class Hut : public DamageableThing
{
public:
	Hut(int teamNum, Vector3 pos=Vector3(0,0,0)) 
		: DamageableThing("Ogrian/Tower", MODEL, "Hut", false, CONR("HUT_WIDTH"), pos, CUBE)
	{

	}


	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return HUT; }
	
private:
	int mTeamNum;
};

}
#endif