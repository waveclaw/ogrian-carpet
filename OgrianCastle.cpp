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
#include "OgrianManaThing.h"

namespace Ogrian
{

//----------------------------------------------------------------------------
Castle::Castle(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	setPosition(pos + Vector3(0, CONR("CASTLETOWER_HEIGHT") + CONR("CASTLE_WIDTH") - CONR("CASTLE_OFFSET"), 0));
	Physics::getSingleton().addThing(this);

	Real W = CONR("CASTLE_WIDTH");

	// make a baloon
	mBaloons[0] = new BaloonThing(teamNum, getPosition());
	mBaloons[1] = 0;
	mBaloons[2] = 0;
	mBaloons[3] = 0;
	mBaloons[4] = 0;
	Physics::getSingleton().addThing(mBaloons[0]);

	// build the castle
	mBlocks[0] = mCenterTower = new CastleTowerThing(this, pos);

	mBlocks[1] = mCornerTowerNE = new CastleTowerThing(this, pos + Vector3( 2*W, 0, 2*W));
	mBlocks[2] = mCornerTowerSE = new CastleTowerThing(this, pos + Vector3( 2*W, 0,-2*W));
	mBlocks[3] = mCornerTowerSW = new CastleTowerThing(this, pos + Vector3(-2*W, 0,-2*W));
	mBlocks[4] = mCornerTowerNW = new CastleTowerThing(this, pos + Vector3(-2*W, 0, 2*W));

	mBlocks[5]  = mInnerWallN1 = new CastleWallThing(this, pos + Vector3( 2*W, 0,  -W));
	mBlocks[6]  = mInnerWallN2 = new CastleWallThing(this, pos + Vector3( 2*W, 0,   0));
	mBlocks[7]  = mInnerWallN3 = new CastleWallThing(this, pos + Vector3( 2*W, 0,   W));
	mBlocks[8]  = mInnerWallE1 = new CastleWallThing(this, pos + Vector3(  -W, 0, 2*W));
	mBlocks[9]  = mInnerWallE2 = new CastleWallThing(this, pos + Vector3(   0, 0, 2*W));
	mBlocks[10] = mInnerWallE3 = new CastleWallThing(this, pos + Vector3(   W, 0, 2*W));
	mBlocks[11] = mInnerWallS1 = new CastleWallThing(this, pos + Vector3(-2*W, 0,  -W));
	mBlocks[12] = mInnerWallS2 = new CastleWallThing(this, pos + Vector3(-2*W, 0,   0));
	mBlocks[13] = mInnerWallS3 = new CastleWallThing(this, pos + Vector3(-2*W, 0,   W));
	mBlocks[14] = mInnerWallW1 = new CastleWallThing(this, pos + Vector3(  -W, 0,-2*W));
	mBlocks[15] = mInnerWallW2 = new CastleWallThing(this, pos + Vector3(   0, 0,-2*W));
	mBlocks[16] = mInnerWallW3 = new CastleWallThing(this, pos + Vector3(   W, 0,-2*W));

	mBlocks[17] = mFarCornerTowerN  = new CastleTowerThing(this, pos + Vector3(   0, 0, 4*W));
	mBlocks[18] = mFarCornerTowerS  = new CastleTowerThing(this, pos + Vector3(   0, 0,-4*W));
	mBlocks[19] = mFarCornerTowerE  = new CastleTowerThing(this, pos + Vector3( 4*W, 0,   0));
	mBlocks[20] = mFarCornerTowerW  = new CastleTowerThing(this, pos + Vector3(-4*W, 0,   0));
	mBlocks[21] = mFarCornerTowerNE = new CastleTowerThing(this, pos + Vector3( 4*W, 0, 4*W));
	mBlocks[22] = mFarCornerTowerSE = new CastleTowerThing(this, pos + Vector3( 4*W, 0,-4*W));
	mBlocks[23] = mFarCornerTowerSW = new CastleTowerThing(this, pos + Vector3(-4*W, 0,-4*W));
	mBlocks[24] = mFarCornerTowerNW = new CastleTowerThing(this, pos + Vector3(-4*W, 0, 4*W));

	mBlocks[25] = mOuterWallN1 = new CastleWallThing(this, pos + Vector3( 4*W, 0,-3*W));
	mBlocks[26] = mOuterWallN2 = new CastleWallThing(this, pos + Vector3( 4*W, 0,-2*W));
	mBlocks[27] = mOuterWallN3 = new CastleWallThing(this, pos + Vector3( 4*W, 0,  -W));
	mBlocks[28] = mOuterWallN5 = new CastleWallThing(this, pos + Vector3( 4*W, 0,   W));
	mBlocks[29] = mOuterWallN6 = new CastleWallThing(this, pos + Vector3( 4*W, 0, 2*W));
	mBlocks[30] = mOuterWallN7 = new CastleWallThing(this, pos + Vector3( 4*W, 0, 3*W));
	mBlocks[31] = mOuterWallE1 = new CastleWallThing(this, pos + Vector3(-3*W, 0, 4*W));
	mBlocks[32] = mOuterWallE2 = new CastleWallThing(this, pos + Vector3(-2*W, 0, 4*W));
	mBlocks[33] = mOuterWallE3 = new CastleWallThing(this, pos + Vector3(  -W, 0, 4*W));
	mBlocks[34] = mOuterWallE5 = new CastleWallThing(this, pos + Vector3(   W, 0, 4*W));
	mBlocks[35] = mOuterWallE6 = new CastleWallThing(this, pos + Vector3( 2*W, 0, 4*W));
	mBlocks[36] = mOuterWallE7 = new CastleWallThing(this, pos + Vector3( 3*W, 0, 4*W));
	mBlocks[37] = mOuterWallS1 = new CastleWallThing(this, pos + Vector3(-4*W, 0,-3*W));
	mBlocks[38] = mOuterWallS2 = new CastleWallThing(this, pos + Vector3(-4*W, 0,-2*W));
	mBlocks[39] = mOuterWallS3 = new CastleWallThing(this, pos + Vector3(-4*W, 0,  -W));
	mBlocks[40] = mOuterWallS5 = new CastleWallThing(this, pos + Vector3(-4*W, 0,   W));
	mBlocks[41] = mOuterWallS6 = new CastleWallThing(this, pos + Vector3(-4*W, 0, 2*W));
	mBlocks[42] = mOuterWallS7 = new CastleWallThing(this, pos + Vector3(-4*W, 0, 3*W));
	mBlocks[43] = mOuterWallW1 = new CastleWallThing(this, pos + Vector3(-3*W, 0,-4*W));
	mBlocks[44] = mOuterWallW2 = new CastleWallThing(this, pos + Vector3(-2*W, 0,-4*W));
	mBlocks[45] = mOuterWallW3 = new CastleWallThing(this, pos + Vector3(  -W, 0,-4*W));
	mBlocks[46] = mOuterWallW5 = new CastleWallThing(this, pos + Vector3(   W, 0,-4*W));
	mBlocks[47] = mOuterWallW6 = new CastleWallThing(this, pos + Vector3( 2*W, 0,-4*W));
	mBlocks[48] = mOuterWallW7 = new CastleWallThing(this, pos + Vector3( 3*W, 0,-4*W));

	// add them to physics
	for (int i=0; i<49; i++)
		Physics::getSingleton().addThing(mBlocks[i]);

	// start at level 0
	setMana(400);
}

//----------------------------------------------------------------------------

void Castle::move(Real time)
{
	DamageableThing::move(time);

	// every frame, we check the baloons
	for (int i=0; i<NUM_BALOONS; i++)
	{
		if (mBaloons[i] && mBaloons[i]->needsOrders())
		{
			BaloonThing* baloon = mBaloons[i];
			Vector3 pos = baloon->getPosition();
			int amount = baloon->getAmount();
			Thing* target = generateTarget(pos, amount);

			// set the target unless its got no mana to get and is already at the castle
			if (target->getType() == MANATHING || sphereDistance(baloon) > getWidth()*2 + baloon->getWidth()*2)
				baloon->setTarget(target);
		}
	}
}

//----------------------------------------------------------------------------

void Castle::setMana(int amount)
{
	mMana = amount;

	setLevel(mMana / CONR("CASTLE_MANA_PER_LEVEL"));
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

void Castle::damage(int amount, int sourceTeamNum)
{
	dropMana(amount);
}

//----------------------------------------------------------------------------

Thing* Castle::generateTarget(Vector3 pos, int amount)
{
	if ((int)mManaThings.size() > 0 && mManaThings[0]->isAlive()) return mManaThings[0];

	else return this;
}

//----------------------------------------------------------------------------

void Castle::claimedManaThing(Thing* mana)
{
	// add the mana to the list
	mManaThings.push_back(mana);
	
	LogManager::getSingleton().logMessage("Mana added to castle");
}

//----------------------------------------------------------------------------

void Castle::unclaimedManaThing(Thing* mana)
{
	// remove the mana from the list
	for (int i=0; i<(int)mManaThings.size(); i++)
	{
		if (mManaThings[i] == mana)
		{
	        mManaThings.erase(mManaThings.begin()+i);
		}
	}

	// retarget the baloon that was getting it
	for (int i=0; i<NUM_BALOONS; i++)
	{
		if (mBaloons[i] && mBaloons[i]->getTarget() == mana)
		{
			mBaloons[i]->setTarget(0);
		}
	}
}

//----------------------------------------------------------------------------

void Castle::dropMana(int amount)
{
	// cant drop more then we have
	if (amount > mMana) amount = mMana;

	// can't drop less then one mana
	if (amount < 1) return;

	setMana(mMana-amount);

	// drop some mana
	Real dir = Math::RangeRandom(0, 2*Math::PI);
	Vector3 offset;
	offset.x = sin(dir);
	offset.z = cos(dir);
	offset.normalise();
	offset *= CONR("CASTLE_MANA_RADIUS");
	ManaThing* mana = new ManaThing(amount, getPosition()+offset);
	Physics::getSingleton().addThing(mana);
	mana->setTeamNum(getTeamNum());
	mana->setPosY(mana->getGroundY());

	//LogManager::getSingleton().logMessage(String("dropped mana: ") += mana->getPosition(). += " offset: " += offset);
}

//----------------------------------------------------------------------------

void Castle::setLevel(Real level)
{
	mLevel = level;

	mBlocks[0]->setPercentage(1);

	// place the first set
	for (int i=1; i<5; i++)
		mBlocks[i]->setPercentage(level);

	// place the second set
	for (i=5; i<17; i++)
		mBlocks[i]->setPercentage(level-1);

	// place the second set
	for (i=17; i<25; i++)
		mBlocks[i]->setPercentage(level-2);
	
	// place the second set
	for (i=25; i<49; i++)
		mBlocks[i]->setPercentage(level-3);
}

//----------------------------------------------------------------------------

}