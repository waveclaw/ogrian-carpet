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
}

//----------------------------------------------------------------------------

Castle::Castle(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), pos, SPHERE)
{
	setTeamNum(teamNum);
	setColour(Physics::getSingleton().getTeam(teamNum)->getColour());

	Physics::getSingleton().addThing(this);

	// set up the beacon
	mBeacon = new CastleBeaconThing();
	Physics::getSingleton().addEffect(mBeacon);

	mBeacon->setColour(getColour());

	Vector3 bpos = getPosition();
	bpos.y += CONR("CASTLE_BEACON_ALTITUDE");
	mBeacon->setPosition(bpos);


	mBaloons[0] = 0; 
	mBaloons[1] = 0;
	mBaloons[2] = 0;
	mBaloons[3] = 0; 
	mBaloons[4] = 0;

	mNumBaloons = 0;
	mRubble = false;
	mLevel = -1;

	// build the castle
	mBlocks[0] = new CastleKeepThing(this, pos);

	for (int i=1; i<NUM_BLOCKS; i++)
		mBlocks[i]=0;

	// add them to physics
	Physics::getSingleton().addThing(mBlocks[0]);

	setMana(CONI("CASTLE_START_MANA"));
	setHealth(CONI("CASTLE_HEALTH"));
	setPosition(mBlocks[0]->getPosition() + Vector3(0, CONR("CASTLEKEEP_HEIGHT")*1.5 + CONR("CASTLE_WIDTH")/4, 0));

	// add existing claimed mana to the list
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* thing = Physics::getSingleton().getThingByIndex(i);
		if (thing->getType() == MANATHING && thing->getTeamNum() == getTeamNum())
			claimedManaThing(thing);
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

	// check the blocks
	for (int i=1; i<NUM_BLOCKS; i++)
	{
		if (mBlocks[i] 
			&& mBlocks[i]->getCurrentLevel() <= 0
			&& mBlocks[i]->getVelY() <= 0 )
		{
			mBlocks[i]->destroy();
			mBlocks[i] = 0;
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

	// inform the wizard
	int wuid = Physics::getSingleton().getTeam(getTeamNum())->getWizardUID();
	WizardThing* wizard = (WizardThing*)Physics::getSingleton().getThing(wuid);

	if (wizard)
		wizard->setBaseMana(mMana);
}

//----------------------------------------------------------------------------

int Castle::getMana()
{
	return mMana;
}

//----------------------------------------------------------------------------

void Castle::addMana(int amount)
{
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

		// chose the mana closest to this baloon
		Real bestScore = 0;

		// check each mana in the list
		for (int i=0; i<(int)mManaThings.size(); i++)
		{
			ManaThing* candidate = (ManaThing*)mManaThings[i];

			// skip dead manas
			if (candidate->isAlive())
			{
				Real score = 1.0f / baloon->sphereDistance(candidate);

				// see if its already a target
				bool isATarget = false;
				for (int j=0; j<(int)targets.size(); j++)
				{
					if (targets[j] == candidate)
					{
						isATarget = true;
						break;
					}
				}
									
				// if its the best so far, store it
				if (score > bestScore
					&& !isATarget)
				{
					bestScore = score;
					target = candidate;
				}
			}
		}
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
		if (candidate
			&& candidate->getTarget() == this
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

		//std::ostringstream num("");
		//num << amount;
		//LogManager::getSingleton().logMessage("Castle dropping mana: " + num.str());

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

	// set the level of each turret
	for (int i=1; i<NUM_BLOCKS; i++)
	{
		if (level+1 > i)
		{
			// make the turret if needed
			if (mBlocks[i] == 0)
			{
				mBlocks[i] = newCastleTurret(i);
				Physics::getSingleton().addThing(mBlocks[i]);
			}

			mBlocks[i]->setPercentage(level+1-i);
		}
		else if (mBlocks[i])
			mBlocks[i]->setPercentage(0);
	}

	mLevel = level;

	// set the number of baloons equal to the level+1
	setNumBaloons(level+1);

	// set the spells
	setSpells(level);
}

//----------------------------------------------------------------------------

CastleTurretThing* Castle::newCastleTurret(int level)
{
	Vector3 pos = getPosition();
	Real W = CONR("CASTLE_WIDTH");

	switch (level)
	{
		case 1: return new CastleTurretThing(this, pos + Vector3( 2*W, 0, 2*W));
	 	case 2: return new CastleTurretThing(this, pos + Vector3( 2*W, 0,-2*W));
	 	case 3: return new CastleTurretThing(this, pos + Vector3(-2*W, 0,-2*W));
	 	case 4: return new CastleTurretThing(this, pos + Vector3(-2*W, 0, 2*W));

	  	case 5: return new CastleTurretThing(this, pos + Vector3(   0, 0, 4*W));
	  	case 6: return new CastleTurretThing(this, pos + Vector3(   0, 0,-4*W));
	  	case 7: return new CastleTurretThing(this, pos + Vector3( 4*W, 0,   0));
	  	case 8: return new CastleTurretThing(this, pos + Vector3(-4*W, 0,   0));
		//case 9: return new CastleTurretThing(this, pos + Vector3( 4*W, 0, 4*W));
		//case 10: return new CastleTurretThing(this, pos + Vector3( 4*W, 0,-4*W));
		//case 11: return new CastleTurretThing(this, pos + Vector3(-4*W, 0,-4*W));
		//case 12: return new CastleTurretThing(this, pos + Vector3(-4*W, 0, 4*W));
	}
	return 0;
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

		if (level >= 0) SpellManager::getSingleton().enableSpell(SPELL_FIREBALL);
		else			SpellManager::getSingleton().disableSpell(SPELL_FIREBALL);

		if (level >= 1) SpellManager::getSingleton().enableSpell(SPELL_SENTINEL);
		else			SpellManager::getSingleton().disableSpell(SPELL_SENTINEL);

		if (level >= 2) SpellManager::getSingleton().enableSpell(SPELL_SPEED);
		else			SpellManager::getSingleton().disableSpell(SPELL_SPEED);

		if (level >= 3) SpellManager::getSingleton().enableSpell(SPELL_AKIMBO_FIREBALL);
		else			SpellManager::getSingleton().disableSpell(SPELL_AKIMBO_FIREBALL);

		if (level >= 4) SpellManager::getSingleton().enableSpell(SPELL_FIRESTORM);
		else			SpellManager::getSingleton().disableSpell(SPELL_FIRESTORM);
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

				if (level >= 0) Multiplayer::getSingleton().serverSendInt(SPELL_FIREBALL,ID_ENABLESPELL,player.id);
				else			Multiplayer::getSingleton().serverSendInt(SPELL_FIREBALL,ID_DISABLESPELL,player.id);

				if (level >= 1) Multiplayer::getSingleton().serverSendInt(SPELL_SENTINEL,ID_ENABLESPELL,player.id);
				else			Multiplayer::getSingleton().serverSendInt(SPELL_SENTINEL,ID_DISABLESPELL,player.id);

				if (level >= 1) Multiplayer::getSingleton().serverSendInt(SPELL_SPEED,ID_ENABLESPELL,player.id);
				else			Multiplayer::getSingleton().serverSendInt(SPELL_SPEED,ID_DISABLESPELL,player.id);

				if (level >= 2) Multiplayer::getSingleton().serverSendInt(SPELL_AKIMBO_FIREBALL,ID_ENABLESPELL,player.id);
				else			Multiplayer::getSingleton().serverSendInt(SPELL_AKIMBO_FIREBALL,ID_DISABLESPELL,player.id);

				if (level >= 3) Multiplayer::getSingleton().serverSendInt(SPELL_FIRESTORM,ID_ENABLESPELL,player.id);
				else			Multiplayer::getSingleton().serverSendInt(SPELL_FIRESTORM,ID_DISABLESPELL,player.id);
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

void CastleTurretThing::setPercentage(Real per)
{
	CastleBlockThing::setPercentage(per);
	
	if (per >= 0 && !mCrane) 
	{
		Vector3 pos = getPosition();
		pos.y = getGroundY() + getHeight();
		mCrane = new CraneThing(getTeamNum(), pos);
		Physics::getSingleton().addThing(mCrane);
	}
	else if (per <= 0 && mCrane)
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