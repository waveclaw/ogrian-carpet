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
#include "OgrianRenderer.h"
#include "OgrianPhysics.h"
#include "OgrianSpellManager.h"
#include "OgrianManaThing.h"

namespace Ogrian
{

//----------------------------------------------------------------------------

CastleFlagThing::CastleFlagThing()
	: Thing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), Vector3(0,0,0), SPHERE)
{
	mBeacon = new CastleBeaconThing();
	Physics::getSingleton().addEffect(mBeacon);
	
	Vector3 bpos = getPosition();
	bpos.y += CONR("CASTLE_BEACON_ALTITUDE");
	mBeacon->setPosition(bpos);

	LogManager::getSingleton().logMessage("Making Beacon");
}

//----------------------------------------------------------------------------

Castle::Castle(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	setPosition(pos + Vector3(0, CONR("CASTLEKEEP_HEIGHT") + CONR("CASTLE_WIDTH") - CONR("CASTLE_OFFSET"), 0));
	Physics::getSingleton().addThing(this);

	// set up the beacon
	mBeacon = new CastleBeaconThing();
	Physics::getSingleton().addEffect(mBeacon);

	mBeacon->setColour(getColour());

	Vector3 bpos = getPosition();
	bpos.y += CONR("CASTLE_BEACON_ALTITUDE");
	mBeacon->setPosition(bpos);

	Real W = CONR("CASTLE_WIDTH");

	mBaloons[0] = 0; 
	mBaloons[1] = 0;
	mBaloons[2] = 0;
	mBaloons[3] = 0;
	mBaloons[4] = 0;

	mNumBaloons = 0;
	mRubble = false;

	// build the castle
	mBlocks[0] = mCenterTower = new CastleKeepThing(this, pos);

	// add them to physics
	Physics::getSingleton().addThing(mBlocks[0]);

	// start at level 0
	setMana(400);

	setHealth(CONI("CASTLE_HEALTH"));

	// add existing claimed mana to the list
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* thing = Physics::getSingleton().getThingByIndex(i);
		if (thing->getType() == MANATHING && thing->getTeamNum() == getTeamNum())
			claimedManaThing(thing);
	}
}

//----------------------------------------------------------------------------

void Castle::makeLevel1()
{
	Vector3 pos = getPosition();
	Real W = CONR("CASTLE_WIDTH");
	mBlocks[1] = mCornerTowerNE = new CastleTowerThing(this, pos + Vector3( 2*W, 0, 2*W));
	mBlocks[2] = mCornerTowerSE = new CastleTowerThing(this, pos + Vector3( 2*W, 0,-2*W));
	mBlocks[3] = mCornerTowerSW = new CastleTowerThing(this, pos + Vector3(-2*W, 0,-2*W));
	mBlocks[4] = mCornerTowerNW = new CastleTowerThing(this, pos + Vector3(-2*W, 0, 2*W));
	
	// add them to physics
	for (int i=1; i<5; i++)
		Physics::getSingleton().addThing(mBlocks[i]);
}

//----------------------------------------------------------------------------

void Castle::makeLevel2()
{
	Vector3 pos = getPosition();
	Real W = CONR("CASTLE_WIDTH");
	mBlocks[5]  = mInnerWallN1 = new CastleWallEWThing(this, pos + Vector3( 2*W, 0,  -W));
	mBlocks[6]  = mInnerWallN2 = new CastleWallEWThing(this, pos + Vector3( 2*W, 0,   0));
	mBlocks[7]  = mInnerWallN3 = new CastleWallEWThing(this, pos + Vector3( 2*W, 0,   W));
	mBlocks[8]  = mInnerWallE1 = new CastleWallNSThing(this, pos + Vector3(  -W, 0, 2*W));
	mBlocks[9]  = mInnerWallE2 = new CastleWallNSThing(this, pos + Vector3(   0, 0, 2*W));
	mBlocks[10] = mInnerWallE3 = new CastleWallNSThing(this, pos + Vector3(   W, 0, 2*W));
	mBlocks[11] = mInnerWallS1 = new CastleWallEWThing(this, pos + Vector3(-2*W, 0,  -W));
	mBlocks[12] = mInnerWallS2 = new CastleWallEWThing(this, pos + Vector3(-2*W, 0,   0));
	mBlocks[13] = mInnerWallS3 = new CastleWallEWThing(this, pos + Vector3(-2*W, 0,   W));
	mBlocks[14] = mInnerWallW1 = new CastleWallNSThing(this, pos + Vector3(  -W, 0,-2*W));
	mBlocks[15] = mInnerWallW2 = new CastleWallNSThing(this, pos + Vector3(   0, 0,-2*W));
	mBlocks[16] = mInnerWallW3 = new CastleWallNSThing(this, pos + Vector3(   W, 0,-2*W));
	
	// add them to physics
	for (int i=5; i<17; i++)
		Physics::getSingleton().addThing(mBlocks[i]);
}

//----------------------------------------------------------------------------

void Castle::makeLevel3()
{
	Vector3 pos = getPosition();
	Real W = CONR("CASTLE_WIDTH");
	mBlocks[17] = mFarCornerTowerN  = new CastleTowerThing(this, pos + Vector3(   0, 0, 4*W));
	mBlocks[18] = mFarCornerTowerS  = new CastleTowerThing(this, pos + Vector3(   0, 0,-4*W));
	mBlocks[19] = mFarCornerTowerE  = new CastleTowerThing(this, pos + Vector3( 4*W, 0,   0));
	mBlocks[20] = mFarCornerTowerW  = new CastleTowerThing(this, pos + Vector3(-4*W, 0,   0));
	mBlocks[21] = mFarCornerTowerNE = new CastleTowerThing(this, pos + Vector3( 4*W, 0, 4*W));
	mBlocks[22] = mFarCornerTowerSE = new CastleTowerThing(this, pos + Vector3( 4*W, 0,-4*W));
	mBlocks[23] = mFarCornerTowerSW = new CastleTowerThing(this, pos + Vector3(-4*W, 0,-4*W));
	mBlocks[24] = mFarCornerTowerNW = new CastleTowerThing(this, pos + Vector3(-4*W, 0, 4*W));
	
	// add them to physics
	for (int i=17; i<25; i++)
		Physics::getSingleton().addThing(mBlocks[i]);
}

//----------------------------------------------------------------------------

void Castle::makeLevel4()
{
	Vector3 pos = getPosition();
	Real W = CONR("CASTLE_WIDTH");
	mBlocks[25] = mOuterWallN1 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0,-3*W));
	mBlocks[26] = mOuterWallN2 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0,-2*W));
	mBlocks[27] = mOuterWallN3 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0,  -W));
	mBlocks[28] = mOuterWallN5 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0,   W));
	mBlocks[29] = mOuterWallN6 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0, 2*W));
	mBlocks[30] = mOuterWallN7 = new CastleWallEWThing(this, pos + Vector3( 4*W, 0, 3*W));
	mBlocks[31] = mOuterWallE1 = new CastleWallNSThing(this, pos + Vector3(-3*W, 0, 4*W));
	mBlocks[32] = mOuterWallE2 = new CastleWallNSThing(this, pos + Vector3(-2*W, 0, 4*W));
	mBlocks[33] = mOuterWallE3 = new CastleWallNSThing(this, pos + Vector3(  -W, 0, 4*W));
	mBlocks[34] = mOuterWallE5 = new CastleWallNSThing(this, pos + Vector3(   W, 0, 4*W));
	mBlocks[35] = mOuterWallE6 = new CastleWallNSThing(this, pos + Vector3( 2*W, 0, 4*W));
	mBlocks[36] = mOuterWallE7 = new CastleWallNSThing(this, pos + Vector3( 3*W, 0, 4*W));
	mBlocks[37] = mOuterWallS1 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0,-3*W));
	mBlocks[38] = mOuterWallS2 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0,-2*W));
	mBlocks[39] = mOuterWallS3 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0,  -W));
	mBlocks[40] = mOuterWallS5 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0,   W));
	mBlocks[41] = mOuterWallS6 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0, 2*W));
	mBlocks[42] = mOuterWallS7 = new CastleWallEWThing(this, pos + Vector3(-4*W, 0, 3*W));
	mBlocks[43] = mOuterWallW1 = new CastleWallNSThing(this, pos + Vector3(-3*W, 0,-4*W));
	mBlocks[44] = mOuterWallW2 = new CastleWallNSThing(this, pos + Vector3(-2*W, 0,-4*W));
	mBlocks[45] = mOuterWallW3 = new CastleWallNSThing(this, pos + Vector3(  -W, 0,-4*W));
	mBlocks[46] = mOuterWallW5 = new CastleWallNSThing(this, pos + Vector3(   W, 0,-4*W));
	mBlocks[47] = mOuterWallW6 = new CastleWallNSThing(this, pos + Vector3( 2*W, 0,-4*W));
	mBlocks[48] = mOuterWallW7 = new CastleWallNSThing(this, pos + Vector3( 3*W, 0,-4*W));
	
	// add them to physics
	for (int i=25; i<49; i++)
		Physics::getSingleton().addThing(mBlocks[i]);
}

//----------------------------------------------------------------------------

void Castle::destroyLevel1()
{
	for (int i=1; i<=4; i++)
	{
		if (mBlocks[i])
		{
			mBlocks[i]->destroy();
			mBlocks[i] = 0;
		}
	}
}

//----------------------------------------------------------------------------

void Castle::destroyLevel2()
{
	for (int i=5; i<=16; i++)
	{
		if (mBlocks[i])
		{
			mBlocks[i]->destroy();
			mBlocks[i] = 0;
		}
	}
}

//----------------------------------------------------------------------------

void Castle::destroyLevel3()
{
	for (int i=17; i<=24; i++)
	{
		if (mBlocks[i])
		{
			mBlocks[i]->destroy();
			mBlocks[i] = 0;
		}
	}
}

//----------------------------------------------------------------------------

void Castle::destroyLevel4()
{
	for (int i=25; i<=48; i++)
	{
		if (mBlocks[i])
		{
			mBlocks[i]->destroy();
			mBlocks[i] = 0;
		}
	}
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
			Thing* target = generateTarget(baloon);

			// set the target unless its got no mana to get and is already at the castle
			if (target->getType() == MANATHING || sphereDistance(baloon) > getWidth() + baloon->getWidth())
				baloon->setTarget(target);
		}
	}
}

//----------------------------------------------------------------------------

void Castle::setHealth(int health)
{
	if (health <= 0)
	{
		health = 0;
		mBlocks[0]->setPercentage(0);
		mRubble = true;
	}
	else
	{
		mBlocks[0]->setPercentage(
			(health + CONR("CASTLE_RUBBLE"))
			/ (CONR("CASTLE_HEALTH") + CONR("CASTLE_RUBBLE")));

		mRubble = false;
	}

	DamageableThing::setHealth(health);
}

//----------------------------------------------------------------------------

void Castle::setMana(int amount)
{
	if (amount < 0) amount = 0;

	mMana = amount;

	setLevel(mMana / CONR("CASTLE_MANA_PER_LEVEL"));

	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	team->setScore(amount);

	if (amount > 0) setHealth(CONI("CASTLE_HEALTH"));
}

//----------------------------------------------------------------------------

int Castle::getMana()
{
	return mMana;
}

//----------------------------------------------------------------------------

void Castle::addMana(int amount)
{
		//std::ostringstream num("");
		//num << amount;
		//LogManager::getSingleton().logMessage("Castle gaining mana: " + num.str());

	setMana(mMana + amount);
}

//----------------------------------------------------------------------------

void Castle::damage(int amount, int sourceTeamNum)
{
	if (mMana > 0)
		dropMana(amount / CONR("CASTLE_DAMAGE_PER_MANA"));
	else 
		DamageableThing::damage(amount, sourceTeamNum);
}

//----------------------------------------------------------------------------

Thing* Castle::generateTarget(BaloonThing* baloon)
{
	// if the baloon is full, return to the castle
	if (baloon->getAmount() >= CONI("BALOON_CAPACITY"))
		return this;

	// pick a target mana
	ManaThing* target = 0;
	if ((int)mManaThings.size() > 0)
	{
		// build a list of the other targets
		std::vector<ManaThing*> targets;
		for (int i=0; i<mNumBaloons; i++)
			if (mBaloons[i]->getTarget() && mBaloons[i]->getTarget()->getType() == MANATHING
				&& mBaloons[i]->getTarget()->isAlive())
				targets.push_back((ManaThing*)mBaloons[i]->getTarget());

		// if there are no other targets, chose the mana closest to this baloon
		if (targets.size() == 0) 
		{
			Real bestScore = 0;

			// check each mana in the list
			for (int i=0; i<(int)mManaThings.size(); i++)
			{
				ManaThing* candidate = (ManaThing*)mManaThings[i];

				// skip dead manas
				if (candidate->isAlive())
				{
					Real score = 1.0f / baloon->sphereDistance(candidate);
										
					// if its the best so far, store it
					if (score > bestScore)
					{
						bestScore = score;
						target = candidate;
					}
				}
			}
		}
		else // chose the mana farthest away from the other targets
		{
			Real bestScore = 0;
			// check each mana in the list
			for (int i=0; i<(int)mManaThings.size(); i++)
			{
				ManaThing* candidate = (ManaThing*)mManaThings[i];

				// skip dead manas
				if (candidate && candidate->isAlive())
				{
					// generate a score equal to the sum of the distances between the candidate and the other targets
					int score=0;
					for (int j=0; j<(int)targets.size(); j++)
						score += candidate->cylinderDistance(targets[j]);

					// if its the best so far, store it
					if (score > bestScore)
					{
						bestScore = score;
						target = candidate;
					}
				}
			}

			if (bestScore == 0) // if all the mana is already targetted
				return this; // come back to the castle
		}

		if (target) // if a valid target was found,
		{
			// check to see if we should drop off what we have before proceeding to it
			if (baloon->getAmount() > 0)
			{
				Real bcdist = baloon->sphereDistance(this); // baloon -> castle distance
				Real bmdist = baloon->sphereDistance(target); // baloon -> mana distance
				Real mcdist = target->sphereDistance(this); // castle -> mana distance

				if (bmdist + mcdist > bcdist * CONR("BALOON_RETURN_THRESHOLD_RATIO")) 
					return this;
			}

			return target; // return it
		}
		else 
			return this; // otherwise, wait at the castle
	}
	
	// there is no mana, so wait at the castle
	else return this;
}

//----------------------------------------------------------------------------

bool Castle::isRubble()
{
	return mRubble;
}

//----------------------------------------------------------------------------

void Castle::claimedManaThing(Thing* mana)
{
	// add the mana to the list
	mManaThings.push_back(mana);

	// check to see if an empty baloon is heading back to the castle
	// find the closest such baloon and set it to pick up this mana
	BaloonThing* bal = 0;
	Real bestDist = HeightMap::getSingleton().getWorldSize()*2;
	for (int i=0; i<mNumBaloons; i++)
	{
		BaloonThing* candidate = mBaloons[i];
		if (candidate->getTarget() == this
			&& candidate->getAmount() == 0
			&& bestDist > mana->cylinderDistance(candidate))
		{
			bal = candidate;
			bestDist = mana->cylinderDistance(candidate);
		}
	}

	if (bal) bal->setTarget(mana);
}

//----------------------------------------------------------------------------

void Castle::unclaimedManaThing(Thing* mana)
{
	// remove the mana from the list
	for (int i=0; i<(int)mManaThings.size(); i++)
	{
		if (mManaThings[i] == mana)
		{
			mManaThings[i]=0;
	        mManaThings.erase(mManaThings.begin()+i);
		}
	}

	// retarget the baloon that was getting it
	for (int i=0; i<mNumBaloons; i++)
	{
		if (mBaloons[i]->getTarget() == mana)
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

		std::ostringstream num("");
		num << amount;
		LogManager::getSingleton().logMessage("Castle dropping mana: " + num.str());

	// can't drop less then one mana
	if (amount < 1) return;

	setMana(mMana-amount);

	// drop some mana
	Real dir = Math::RangeRandom(0, 2*Math::PI);
	Vector3 offset;
	offset.x = sin(dir);
	offset.y = 0;
	offset.z = cos(dir);
	offset.normalise();
	offset *= CONR("CASTLE_MANA_RADIUS");
	ManaThing* mana = new ManaThing(amount, getPosition()+offset);
	Physics::getSingleton().addThing(mana);
	mana->setTeamNum(getTeamNum());
	mana->setPosY(mana->getGroundY());
}

//----------------------------------------------------------------------------

void Castle::setLevel(Real level)
{
	mBlocks[0]->setPercentage(1);

	if (level >= 0)
	{
		if (mLevel < 0) makeLevel1();

		// place the first set
		for (int i=1; i<5; i++)
			mBlocks[i]->setPercentage(level);
	}
	else destroyLevel1();

	if (level >= 1)
	{
		if (mLevel < 1) makeLevel2();

		// place the second set
		for (int i=5; i<17; i++)
			mBlocks[i]->setPercentage(level-1);
	}
	else destroyLevel2();

	if (level >= 2)
	{
		if (mLevel < 2) makeLevel3();

		// place the second set
		for (int i=17; i<25; i++)
			mBlocks[i]->setPercentage(level-2);
	}
	else destroyLevel3();

	if (level >= 3)
	{
		if (mLevel < 3) makeLevel4();

		// place the second set
		for (int i=25; i<49; i++)
			mBlocks[i]->setPercentage(level-3);
	}
	else destroyLevel4();

	mLevel = level;

	// set the number of baloons equal to the level+1
	setNumBaloons(level+1);

	// set the spells
	setSpells(level);
}

//----------------------------------------------------------------------------

void Castle::setSpells(int level)
{
	if (Multiplayer::getSingleton().isClient()) return;

	// get our wizard
	int wuid = Physics::getSingleton().getTeam(getTeamNum())->getWizardUID();
	//WizardThing* wizard = Physics::getSingleton().getThing(wuid);

	// set the spells
	int cuid = Renderer::getSingleton().getCameraThing()->getUID();
	if (wuid == cuid)
	{
		SpellManager::getSingleton().enableSpell(SPELL_BUILD);
		SpellManager::getSingleton().enableSpell(SPELL_CLAIM);
		SpellManager::getSingleton().enableSpell(SPELL_FIREBALL);
	}
	else if (Multiplayer::getSingleton().isServer())
	{
		// find the right player
		for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
		{
			PlayerInfo player = Multiplayer::getSingleton().getClient(i);
			if (wuid == player.wizardUID)
			{
				Multiplayer::getSingleton().serverSendInt(SPELL_BUILD,ID_ENABLESPELL,player.id);
				Multiplayer::getSingleton().serverSendInt(SPELL_CLAIM,ID_ENABLESPELL,player.id);
				Multiplayer::getSingleton().serverSendInt(SPELL_FIREBALL,ID_ENABLESPELL,player.id);
			}
		}
	}
}

//----------------------------------------------------------------------------

void Castle::setNumBaloons(int num)
{
	// 0 <= num <= NUM_BALOONS
	if (num < 0) num = 0;
	if (num > NUM_BALOONS) num = NUM_BALOONS;

	// loop through each baloon slot
	for (int i=1; i<=NUM_BALOONS; i++)
	{
		// if the baloon there is being removed
		if (mNumBaloons >= i && num < i)
		{
			// drop any mana that it is carrying
			ManaThing* mana = new ManaThing(mBaloons[i-1]->unload(), mBaloons[i-1]->getPosition());
			mana->setTeamNum(getTeamNum());
			Physics::getSingleton().addThing(mana);
			
			// remove the baloon
			mBaloons[i-1]->destroy();
			mBaloons[i-1] = 0;
		}
		// else if a baloon needs to be added to this slot
		else if (mNumBaloons < i && num >= i)
		{
			// add the baloon
			mBaloons[i-1] = new BaloonThing(getTeamNum(), getPosition());
			Physics::getSingleton().addThing(mBaloons[i-1]);
		}
	}

	// update mNumBaloons
	mNumBaloons = num;
}

//----------------------------------------------------------------------------

void CastleTowerThing::setPercentage(Real per)
{
	CastleBlockThing::setPercentage(per);
	
	if (per >= 1 && !mCrane) 
	{
		Vector3 pos = getPosition();
		pos.y = getGroundY() + getHeight();
		mCrane = new CraneThing(getTeamNum(), pos);
		Physics::getSingleton().addThing(mCrane);
	}
	else if (per < 1 && mCrane)
	{
		mCrane->destroy();
		mCrane = 0;
	}
}

//----------------------------------------------------------------------------

void CastleKeepThing::setPercentage(Real per)
{
	CastleBlockThing::setPercentage(per);
	
	if (per >= 1 && !mCrane) 
	{
		Vector3 pos = getPosition();
		pos.y = getGroundY() + getHeight();
		mCrane = new CraneThing(getTeamNum(), pos);
		Physics::getSingleton().addThing(mCrane);
	}
	else if (per < 1 && mCrane)
	{
		mCrane->destroy();
		mCrane = 0;
	}
}

//----------------------------------------------------------------------------

}