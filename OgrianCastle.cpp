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
OgrianCastle.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a castle

/*------------------------------------*/

#include "OgrianCastle.h"
#include "OgrianPhysics.h"

namespace Ogrian
{

//----------------------------------------------------------------------------
Castle::Castle(int teamNum, Vector3 pos) 
{
	mPos = pos;
	mTeamNum = teamNum;

	//mCenterTower = new CastleTowerThing(mTeamNum, mPos);
	//Physics::getSingleton().addThing(mCenterTower);

	Physics::getSingleton().addThing(new CastleWallThing(mTeamNum, mPos));
}
//----------------------------------------------------------------------------

void Castle::setMana(int amount)
{
	mMana = amount;
}

//----------------------------------------------------------------------------

int Castle::getMana()
{
	return mMana;
}

//----------------------------------------------------------------------------

void Castle::addMana(int amount)
{
	mMana += amount;
}

//----------------------------------------------------------------------------

void Castle::frame(Real time)
{

}

//----------------------------------------------------------------------------

}