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

	Real W = CONR("CASTLE_WIDTH");

	// build the castle
	mBlocks[0] = mCenterTower = new CastleTowerThing(mTeamNum, mPos);

	mBlocks[1] = mCornerTowerNE = new CastleTowerThing(mTeamNum, mPos + Vector3( 2*W, 0, 2*W));
	mBlocks[2] = mCornerTowerSE = new CastleTowerThing(mTeamNum, mPos + Vector3( 2*W, 0,-2*W));
	mBlocks[3] = mCornerTowerSW = new CastleTowerThing(mTeamNum, mPos + Vector3(-2*W, 0,-2*W));
	mBlocks[4] = mCornerTowerNW = new CastleTowerThing(mTeamNum, mPos + Vector3(-2*W, 0, 2*W));

	mBlocks[5]  = mInnerWallN1 = new CastleWallThing(mTeamNum, mPos + Vector3( 2*W, 0,  -W));
	mBlocks[6]  = mInnerWallN2 = new CastleWallThing(mTeamNum, mPos + Vector3( 2*W, 0,   0));
	mBlocks[7]  = mInnerWallN3 = new CastleWallThing(mTeamNum, mPos + Vector3( 2*W, 0,   W));
	mBlocks[8]  = mInnerWallE1 = new CastleWallThing(mTeamNum, mPos + Vector3(  -W, 0, 2*W));
	mBlocks[9]  = mInnerWallE2 = new CastleWallThing(mTeamNum, mPos + Vector3(   0, 0, 2*W));
	mBlocks[10] = mInnerWallE3 = new CastleWallThing(mTeamNum, mPos + Vector3(   W, 0, 2*W));
	mBlocks[11] = mInnerWallS1 = new CastleWallThing(mTeamNum, mPos + Vector3(-2*W, 0,  -W));
	mBlocks[12] = mInnerWallS2 = new CastleWallThing(mTeamNum, mPos + Vector3(-2*W, 0,   0));
	mBlocks[13] = mInnerWallS3 = new CastleWallThing(mTeamNum, mPos + Vector3(-2*W, 0,   W));
	mBlocks[14] = mInnerWallW1 = new CastleWallThing(mTeamNum, mPos + Vector3(  -W, 0,-2*W));
	mBlocks[15] = mInnerWallW2 = new CastleWallThing(mTeamNum, mPos + Vector3(   0, 0,-2*W));
	mBlocks[16] = mInnerWallW3 = new CastleWallThing(mTeamNum, mPos + Vector3(   W, 0,-2*W));

	mBlocks[17] = mFarCornerTowerN  = new CastleTowerThing(mTeamNum, mPos + Vector3(   0, 0, 4*W));
	mBlocks[18] = mFarCornerTowerS  = new CastleTowerThing(mTeamNum, mPos + Vector3(   0, 0,-4*W));
	mBlocks[19] = mFarCornerTowerE  = new CastleTowerThing(mTeamNum, mPos + Vector3( 4*W, 0,   0));
	mBlocks[20] = mFarCornerTowerW  = new CastleTowerThing(mTeamNum, mPos + Vector3(-4*W, 0,   0));
	mBlocks[21] = mFarCornerTowerNE = new CastleTowerThing(mTeamNum, mPos + Vector3( 4*W, 0, 4*W));
	mBlocks[22] = mFarCornerTowerSE = new CastleTowerThing(mTeamNum, mPos + Vector3( 4*W, 0,-4*W));
	mBlocks[23] = mFarCornerTowerSW = new CastleTowerThing(mTeamNum, mPos + Vector3(-4*W, 0,-4*W));
	mBlocks[24] = mFarCornerTowerNW = new CastleTowerThing(mTeamNum, mPos + Vector3(-4*W, 0, 4*W));

	mBlocks[25] = mOuterWallN1 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0,-3*W));
	mBlocks[26] = mOuterWallN2 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0,-2*W));
	mBlocks[27] = mOuterWallN3 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0,  -W));
	mBlocks[28] = mOuterWallN5 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0,   W));
	mBlocks[29] = mOuterWallN6 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0, 2*W));
	mBlocks[30] = mOuterWallN7 = new CastleWallThing(mTeamNum, mPos + Vector3( 4*W, 0, 3*W));
	mBlocks[31] = mOuterWallE1 = new CastleWallThing(mTeamNum, mPos + Vector3(-3*W, 0, 4*W));
	mBlocks[32] = mOuterWallE2 = new CastleWallThing(mTeamNum, mPos + Vector3(-2*W, 0, 4*W));
	mBlocks[33] = mOuterWallE3 = new CastleWallThing(mTeamNum, mPos + Vector3(  -W, 0, 4*W));
	mBlocks[34] = mOuterWallE5 = new CastleWallThing(mTeamNum, mPos + Vector3(   W, 0, 4*W));
	mBlocks[35] = mOuterWallE6 = new CastleWallThing(mTeamNum, mPos + Vector3( 2*W, 0, 4*W));
	mBlocks[36] = mOuterWallE7 = new CastleWallThing(mTeamNum, mPos + Vector3( 3*W, 0, 4*W));
	mBlocks[37] = mOuterWallS1 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0,-3*W));
	mBlocks[38] = mOuterWallS2 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0,-2*W));
	mBlocks[39] = mOuterWallS3 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0,  -W));
	mBlocks[40] = mOuterWallS5 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0,   W));
	mBlocks[41] = mOuterWallS6 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0, 2*W));
	mBlocks[42] = mOuterWallS7 = new CastleWallThing(mTeamNum, mPos + Vector3(-4*W, 0, 3*W));
	mBlocks[43] = mOuterWallW1 = new CastleWallThing(mTeamNum, mPos + Vector3(-3*W, 0,-4*W));
	mBlocks[44] = mOuterWallW2 = new CastleWallThing(mTeamNum, mPos + Vector3(-2*W, 0,-4*W));
	mBlocks[45] = mOuterWallW3 = new CastleWallThing(mTeamNum, mPos + Vector3(  -W, 0,-4*W));
	mBlocks[46] = mOuterWallW5 = new CastleWallThing(mTeamNum, mPos + Vector3(   W, 0,-4*W));
	mBlocks[47] = mOuterWallW6 = new CastleWallThing(mTeamNum, mPos + Vector3( 2*W, 0,-4*W));
	mBlocks[48] = mOuterWallW7 = new CastleWallThing(mTeamNum, mPos + Vector3( 3*W, 0,-4*W));

	// add them to physics
	for (int i=0; i<49; i++)
		Physics::getSingleton().addThing(mBlocks[i]);
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