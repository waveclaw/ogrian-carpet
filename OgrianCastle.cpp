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
#include "OgrianHud.h"
#include "OgrianSpellManager.h"
#include "OgrianManaThing.h"

namespace Ogrian
{

//----------------------------------------------------------------------------

CastleHeartThing::CastleHeartThing(DamageableThing* castle, Vector3 pos)
	: DamageableThing("Ogrian/Heart", SPRITE, "CastleHeart", true, CONR("CASTLE_HEART_SCALE"), pos, SPHERE)
{
	mCastle = castle;

	// set the colour
	if (castle)
		setColour(castle->getColour());

	// set the team
	if (castle)
		setTeamNum(castle->getTeamNum());

	// set the health
	setMaxHealth(CONI("CASTLE_HEART_HEALTH"));
	setHealth(CONI("CASTLE_HEART_HEALTH"));
}

//----------------------------------------------------------------------------

void CastleHeartThing::damage(int amount, int sourceTeamNum)
{
	DamageableThing::damage(amount, sourceTeamNum);

	// report the attack
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team->getWizardUID() == Renderer::getSingleton().getCameraThing()->getUID())
	{
		// send it to the HUD
		Hud::getSingleton().setMessage(CONS("CASTLE_HEART_UNDER_ATTACK"), true);
	}
	else
	{
		// send a message to the right player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(team->getWizardUID());
		Multiplayer::getSingleton().serverSendHudText(CONS("CASTLE_HEART_UNDER_ATTACK"), player);
	}
}

//----------------------------------------------------------------------------

void CastleHeartThing::die()
{
	mCastle->destroy();
}

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

CastleBlockThing::CastleBlockThing(DamageableThing* castle, Vector3 pos, Real width, Real height) 
	: DamageableThing("Ogrian/Tower", MODEL, "CastleBlock", false, width, pos, CUBE, false)
{
	setHeight(height);

	mCastle = castle;

	// find the ground	
	Real w = width/2;
	Real ground00 = getGroundY(pos + Vector3(-w,0,-w));
	Real ground01 = getGroundY(pos + Vector3(-w,0, w));
	Real ground10 = getGroundY(pos + Vector3( w,0,-w));
	Real ground11 = getGroundY(pos + Vector3( w,0, w));
	Real groundc  = getGroundY(pos + Vector3( 0,0, 0));

	Real ground = ground00;
	if (ground01 < ground) ground = ground01;
	if (ground10 < ground) ground = ground10;
	if (ground11 < ground) ground = ground11;
	if (groundc < ground)  ground = groundc;

	mGroundY = ground - CONR("CASTLE_OFFSET") - height/2;

	// start at zero
	setPosY(0.1 + mGroundY);
	mTargetY = getPosY();
	setPercentage(0.5);

	// set the team
	if (castle)
		setTeamNum(castle->getTeamNum());
	
	BuildingHeightMap::getSingleton().moldLandscape(this);
}

//----------------------------------------------------------------------------

// set how far up this block should go to
void CastleBlockThing::setPercentage(Real per)
{
	if (per >= 1) per = 1;
	if (per <= 0) per = -0.1;

	Real newTargetY = mGroundY + getHeight()*per;

	if (newTargetY == mTargetY) return;

	mTargetY = newTargetY;

	if (mTargetY > getPosY())
		setVelY(CONR("CASTLE_RISE_SPEED"));
	else
		setVelY(0-CONR("CASTLE_RISE_SPEED"));

	mPercentage = per;
	setUpdateFlag();
}

//----------------------------------------------------------------------------

// rise smoothly
void CastleBlockThing::move(Real time)
{
	// if it has reached its target pos.y, stop
	if (!Multiplayer::getSingleton().isClient() && (
		(getVelY() < -CONR("CASTLE_RISE_SPEED")/2 && mTargetY > getPosY()) ||
		(getVelY() > CONR("CASTLE_RISE_SPEED")/2 && mTargetY < getPosY())))
	{
		setPosY(mTargetY);
		setVelY(0);
		setUpdateFlag();
	}

	DamageableThing::move(time);
}

//----------------------------------------------------------------------------

// send damage to our host castle
void CastleBlockThing::damage(int amount, int sourceTeamNum)
{
	if (mCastle)
		mCastle->damage(amount, sourceTeamNum);
}

//----------------------------------------------------------------------------

Real CastleBlockThing::getCurrentLevel()
{
	return getPosY() - mGroundY - .1;
}

//----------------------------------------------------------------------------

CastleTurretThing::CastleTurretThing(DamageableThing* castle, Vector3 pos) 
	: CastleBlockThing(castle, pos, CONR("CASTLETURRET_WIDTH"), CONR("CASTLETURRET_HEIGHT"))
{
	mCrane = 0;

	static_cast<Model*>(getVisRep())->setMesh("tower1.mesh",
		CONR("CASTLETURRET_MESH_SCALE"), CONR("CASTLETURRET_MESH_RATIO"));
}

//----------------------------------------------------------------------------

void CastleTurretThing::destroy() 
{ 
	if (mCrane)
		mCrane->destroy();

	mCrane = 0;

	CastleBlockThing::destroy();
}

//----------------------------------------------------------------------------

CastleKeepThing::CastleKeepThing(DamageableThing* castle, Vector3 pos) 
	: CastleBlockThing(castle, pos, CONR("CASTLEKEEP_WIDTH"), CONR("CASTLEKEEP_HEIGHT"))
{
	mCrane = 0;

	static_cast<Model*>(getVisRep())->setMesh("keep.mesh",
		CONR("CASTLEKEEP_MESH_SCALE"), CONR("CASTLEKEEP_MESH_RATIO"));
}

//----------------------------------------------------------------------------

void CastleKeepThing::destroy() 
{ 
	if (mCrane)
		mCrane->destroy();

	mCrane = 0;

	CastleBlockThing::destroy();
}

//----------------------------------------------------------------------------

Castle::Castle(int teamNum, Vector3 pos) 
	: DamageableThing("Ogrian/Flag", SPRITE, "Castle", true, CONR("CASTLE_WIDTH"), pos, SPHERE, false)
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

	mRubble = false;
	mLevel = -1;

	// build the castle
	mBlocks[0] = new CastleKeepThing(this, pos);
	mBlocks[0]->setColour(getColour());

	for (int i=1; i<NUM_BLOCKS; i++)
		mBlocks[i]=0;

	// add them to physics
	Physics::getSingleton().addThing(mBlocks[0]);

	setMana(CONI("CASTLE_START_MANA"));
	setHealth(CONI("CASTLE_HEALTH"));
	setPosition(mBlocks[0]->getPosition() + Vector3(0, CONR("CASTLEKEEP_HEIGHT")*1.5 + CONR("CASTLE_WIDTH")/4, 0));
	
	// set the flag position
	Vector3 fpos = pos;
	fpos.y = HeightMap::getSingleton().getHeightAt(fpos.x, fpos.z) + CONR("CASTLE_FLAG_ALTITUDE");
	setPosition(fpos);

	// set up the portal
	Vector3 ppos = pos;
	ppos.x += CONR("PORTAL_CASTLE_DIST");
	ppos.y = HeightMap::getSingleton().getHeightAt(ppos.x, ppos.z) + CONR("PORTAL_ALTITUDE");
	mPortal = new PortalThing(this, ppos);
	Physics::getSingleton().addThing(mPortal);

	// set up the heart
	Vector3 hpos = pos;
	hpos.y = HeightMap::getSingleton().getHeightAt(hpos.x, hpos.z) + CONR("CASTLE_HEART_ALTITUDE");
	mHeart = new CastleHeartThing(this, hpos);
	Physics::getSingleton().addThing(mHeart);
}

//----------------------------------------------------------------------------

Castle::~Castle()
{
	mPortal = 0;
	mBeacon = 0;
	mHeart = 0;

	for (int i=1; i<NUM_BLOCKS; i++)
		mBlocks[i] = 0;
}

//----------------------------------------------------------------------------

void Castle::destroy()
{
	// drop any remaining mana
	ManaThing* mana = new ManaThing(getMana(), getPosition());
	Physics::getSingleton().addThing(mana);

	// destroy the portal
	if (mPortal) mPortal->destroy();

	// destroy the beacon
	if (mBeacon) mBeacon->destroy();

	// destroy the heart
	if (mHeart) mHeart->destroy();

	// destroy the blocks
	for (int i=0; i<NUM_BLOCKS; i++)
		if(mBlocks[i]) mBlocks[i]->destroy();

	// remove the castle from the team
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	team->setCastle(0);

	// banish the wizard
	Thing* wiz = Physics::getSingleton().getThing(team->getWizardUID());
	((WizardThing*)wiz)->makeGhost();

	if (Multiplayer::getSingleton().isServer() && wiz->getType() != CAMERATHING)
	{
		Multiplayer::getSingleton().ghostWizard(wiz);
	}

	DamageableThing::destroy();
}

//----------------------------------------------------------------------------

void Castle::move(Real time)
{
	DamageableThing::move(time);

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

int Castle::removeMana(int amount)
{
	if (amount > mMana) amount = mMana;
	setMana(mMana - amount);
	return amount;
}

//----------------------------------------------------------------------------

void Castle::damage(int amount, int sourceTeamNum)
{
	if (mMana > 0)
		dropMana(amount / CONR("CASTLE_DAMAGE_PER_MANA"));
	else 
		DamageableThing::damage(amount, sourceTeamNum);

	// report the attack
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	if (team->getWizardUID() == Renderer::getSingleton().getCameraThing()->getUID())
	{
		// send it to the HUD
		Hud::getSingleton().setMessage(CONS("CASTLE_UNDER_ATTACK"), true);
	}
	else
	{
		// send a message to the right player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(team->getWizardUID());
		Multiplayer::getSingleton().serverSendHudText(CONS("CASTLE_UNDER_ATTACK"), player);
	}
}

//----------------------------------------------------------------------------

bool Castle::isRubble()
{
	return mRubble;
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
				mBlocks[i]->setColour(getColour());
				Physics::getSingleton().addThing(mBlocks[i]);
			}

			mBlocks[i]->setPercentage(level+1-i);
		}
		else if (mBlocks[i])
			mBlocks[i]->setPercentage(0);
	}

	mLevel = level;

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

		// if you ever feel like adding more levels, here are some convenient tower locations
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

	// set the spells
	int cuid = Renderer::getSingleton().getCameraThing()->getUID();
	if (wuid == cuid)
	{
		SpellManager::getSingleton().setLevel(level);
	}
	else if (Multiplayer::getSingleton().isServer())
	{
		// find the right player
		for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
		{
			PlayerInfo player = Multiplayer::getSingleton().getClient(i);
			if (wuid == player.wizardUID)
			{
				Multiplayer::getSingleton().serverSendInt(level,ID_SETSPELLLEVEL,player.id);
			}
		}
	}
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