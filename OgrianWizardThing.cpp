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
OgrianWizardThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The wizard thing is the superclass of the CameraThing and the AIWizardThing
it handles all wizard movement code and army tracking and mana regen

 *------------------------------------*/


#include "OgrianWizardThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianMultiplayer.h"
#include "OgrianHud.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianSkinManager.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------
	
WizardThing::WizardThing(bool visible, int skin) 
	: DamageableThing("Ogrian/Clear", visible?ORIENTEDSPRITE:SPRITE, 
	visible?"WizardThing":"CameraThing", true, CONR("WIZARD_SCALE"), Vector3(0,0,0), CYLINDER)
{
	mNextRegenTime = 0;
	mLastSetPosTime = 0;
	mSpeeding = false;
	mStopSpeedTime = 0;
	mActiveMana = 0;
	mBaseMana = 0;
	mNumShrines = 0;
	mTeam = 0;
	mSkin = -1;
	mGhost = false;
	mVisible = visible;
	mFrameTime = 0;

	mForeward = mBack = mLeft = mRight = false;

	setUpdateType(CONTINUOUS);
	setMaxHealth(CONI("WIZARD_HEALTH"));

	setSkin(0);

	reset();
}

//----------------------------------------------------------------------------

void WizardThing::reset()
{
	DamageableThing::reset();

	setHealth(CONI("WIZARD_HEALTH"));
	setScale(CONR("WIZARD_SCALE"));

	if (mTeam)
		delete mTeam;

	mNextRegenTime = 0;
	mLastSetPosTime = 0;
	mSpeeding = false;
	mStopSpeedTime = 0;
	mActiveMana = 0;
	mBaseMana = 0;
	mTeam = 0;
	mGhost = false;
	mFrameTime = 0;

	mNumManaBalls = 0;
	mNumShrines = 0;
	mNumTowers = 0;
	mNumSentinels = 0;
	mNumGnomes = 0;
	mNumTicks = 0;
	mNumAlbatrosses = 0;

	// make a team for this wizard'
	if (!Multiplayer::getSingleton().isClient())
	{		
		int teamNum = Physics::getSingleton().newTeam(getColour());
		setTeamNum(teamNum);
		mTeam = Physics::getSingleton().getTeam(teamNum);
		mTeam->setScore(0);
	}

	// add this player to the server
	if (Multiplayer::getSingleton().isServer() && getType() == CAMERATHING)
		Multiplayer::getSingleton().serverAddCameraPlayer(this);

	// reset the HUD counters
	if (getType() == CAMERATHING)
	{
		setNumManaBalls(mNumManaBalls);
		setNumShrines(mNumShrines);
		setNumTowers(mNumTowers);
		setNumSentinels(mNumSentinels);
		setNumGnomes(mNumGnomes);
		setNumTicks(mNumTicks);
		setNumAlbatrosses(mNumAlbatrosses);
	}

}

//----------------------------------------------------------------------------

int WizardThing::getLevel()
{
	return getTeam()->getCastle()->getLevel();
}

//----------------------------------------------------------------------------

void WizardThing::speed(Real duration)
{
// again, duration is a Real, even if Time is a typedef to the same underlying
// type, a strict compiler would flag using it as a Time as an error.
// -- jdpowell 20060603
	mStopSpeedTime = Clock::getSingleton().getTime() + (Time) duration*1000;

	mSpeeding = true;
}

//----------------------------------------------------------------------------

bool WizardThing::isSpeeding()
{
	return mSpeeding;
}

//----------------------------------------------------------------------------

void WizardThing::setActiveMana(int activeMana)
{
	mActiveMana = activeMana;

	// update the hud if this is a camerathing
	if (getType() == CAMERATHING)
		Hud::getSingleton().setActiveMana(activeMana);

	// send out the update if this is a server
	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_ACTIVE_MANA, getPosition(), activeMana, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setBaseMana(int baseMana)
{
	mBaseMana = baseMana;
	
	setScore();

	// update the hud if this is a camerathing
	if (getType() == CAMERATHING)
		Hud::getSingleton().setBaseMana(baseMana);

	// send out the update if this is a server
	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_BASE_MANA, getPosition(), baseMana, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::subtractActiveMana(int amount)
{
	if (amount == 0) return;

	setActiveMana(mActiveMana - amount);
}

//----------------------------------------------------------------------------

int WizardThing::getActiveMana()
{
	return mActiveMana;
}

//----------------------------------------------------------------------------

void WizardThing::damage(int amount, int sourceTeamNum)
{
	if (!mGhost) 
		DamageableThing::damage(amount, sourceTeamNum);

	if (mGhost)
		setHealth(0);
}

//----------------------------------------------------------------------------

void WizardThing::makeGhost()
{
	mGhost = true;
	setScale(0);
	
	// remove it from the teams enemy lists
	if (!Multiplayer::getSingleton().isClient())
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
		{
			Team* team = Physics::getSingleton().getTeam(i);
			if (team) team->removeEnemy(this);
		}
}

//----------------------------------------------------------------------------

bool WizardThing::isGhost()
{
	return mGhost;
}

//----------------------------------------------------------------------------

Team* WizardThing::getTeam()
{
	return mTeam;
}

//----------------------------------------------------------------------------

void WizardThing::_setUID(int uid)
{
	DamageableThing::_setUID(uid);
	if (mTeam) mTeam->setWizardUID(uid);
}

//----------------------------------------------------------------------------

void WizardThing::setSkin(int skin)
{
	if (mSkin == skin) return;

	mSkin = skin;
	
	getVisRep()->clearPoses();
	getVisRep()->addPose(SkinManager::getSingleton().getWizardSkin(skin));
	getVisRep()->setPose(0);
}

//----------------------------------------------------------------------------

void WizardThing::setColour(ColourValue colour)
{
	DamageableThing::setColour(colour);
	if (mTeam) mTeam->setColour(colour);
}

//----------------------------------------------------------------------------

void WizardThing::setHealth(int health)
{
	// dont exceed the max health
	if (health >= CONI("WIZARD_HEALTH"))
		health = CONI("WIZARD_HEALTH");

	DamageableThing::setHealth(health);

	if (getType() == CAMERATHING)
		Hud::getSingleton().setHealth(health);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_HEALTH, getPosition(), health, getUID());
}
//----------------------------------------------------------------------------

void WizardThing::die()
{
	DamageableThing::die();
		
	setActiveMana(0);

	if (!Multiplayer::getSingleton().isClient())
	{
		setHealth(CONI("WIZARD_HEALTH"));

		if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
		{
			Multiplayer::getSingleton().killWizard(this);
		}		
	}
}

//----------------------------------------------------------------------------

// ignore external up/down velocity changes
void WizardThing::setVelocity(Vector3 vel)
{
	if ((!mSpeeding || Game::getSingleton().getLava()) && !mGhost)
		vel.y = getVelY();
	
	Thing::setVelocity(vel);
}

//----------------------------------------------------------------------------
	
void WizardThing::collided(Thing* e)
{
	if (isGhost()) return;

	// if its your castle
	if(!mGhost && e->getType() == CASTLEFLAGTHING && e->getTeamNum() == getTeamNum()
		&& getHealth() < getMaxHealth())
	{
		playSound(Game::getSingleton().SOUND_HEAL, true);
		setHealth(CONI("WIZARD_HEALTH"));
	}

	// claim mana and Shrines
	if ((e->getType() == MANATHING || e->getType() == SHRINETHING || e->getType() == SHRINEBALLTHING)
			&& e->getTeamNum() != getTeamNum())
	{
		e->claim(getTeamNum());
	}
}

//----------------------------------------------------------------------------

void WizardThing::handleMessage(int msg, Vector3 vec, int val)
{
	if (getType() == CAMERATHING)
	{
		switch (msg)
		{
		case SET_HEALTH:			setHealth(val);			break;
		case SET_ACTIVE_MANA:		setActiveMana(val);		break;
		case SET_BASE_MANA:			setBaseMana(val);		break;

		case SET_NUM_MANABALLS:		setNumManaBalls(val);	break;
		case SET_NUM_SHRINES:		setNumShrines(val);		break;
		case SET_NUM_TOWERS:		setNumTowers(val);		break;
		case SET_NUM_SENTINELS:		setNumSentinels(val);	break;
		case SET_NUM_GNOMES:		setNumGnomes(val);		break;
		case SET_NUM_TICKS:			setNumTicks(val);		break;
		case SET_NUM_ALBATROSSES:	setNumAlbatrosses(val);	break;

		case SET_SCORE: Hud::getSingleton().setScore(val); break;
		}
	}
}

//----------------------------------------------------------------------------

void WizardThing::setNumManaBalls(int num)
{
	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumManaBalls(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_MANABALLS, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumShrines(int num)
{
	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumShrines(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_SHRINES, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumTowers(int num)
{
	setScore();

	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumTowers(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_TOWERS, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumSentinels(int num)
{
	setScore();

	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumSentinels(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_SENTINELS, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumGnomes(int num)
{
	setScore();

	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumGnomes(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_GNOMES, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumTicks(int num)
{
	setScore();

	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumTicks(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_TICKS, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setNumAlbatrosses(int num)
{
	setScore();

	if (getType() == CAMERATHING)
		Hud::getSingleton().setNumAlbatrosses(num);

	else if (Multiplayer::getSingleton().isServer())
		sendMessage(SET_NUM_ALBATROSSES, getPosition(), num, getUID());
}

//----------------------------------------------------------------------------

void WizardThing::setScore()
{
	if (!Multiplayer::getSingleton().isClient())
	{
		Team* team = Physics::getSingleton().getTeam(getTeamNum());

		// calcualte score
		int score = mBaseMana;
		score += mNumTowers * CONI("TOWER_COST");
		score += mNumSentinels * CONI("SENTINEL_COST");
		score += mNumGnomes * CONI("GNOME_COST");
		score += mNumTicks * CONI("TICK_COST");
		score += mNumAlbatrosses * CONI("ALBATROSS_COST");

		team->setScore(score);

		if (getType() == CAMERATHING)
			Hud::getSingleton().setScore(score);

		else if (Multiplayer::getSingleton().isServer())
			sendMessage(SET_SCORE, getPosition(), score, getUID());
	}
}

//----------------------------------------------------------------------------

void WizardThing::addManaBall(int amount)
{
	mNumManaBalls += amount;
	setNumManaBalls(mNumManaBalls);
}

//----------------------------------------------------------------------------

void WizardThing::removeManaBall(int amount)
{
	mNumManaBalls -= amount;
	setNumManaBalls(mNumManaBalls);
}
	
//----------------------------------------------------------------------------

int WizardThing::numManaBalls()
{
	return mNumManaBalls;
}

//----------------------------------------------------------------------------

void WizardThing::addShrine()
{
	mNumShrines++;
	setNumShrines(mNumShrines);
}

//----------------------------------------------------------------------------

void WizardThing::removeShrine()
{
	mNumShrines--;
	setNumShrines(mNumShrines);
}
	
//----------------------------------------------------------------------------

int WizardThing::numShrines()
{
	return mNumShrines;
}

//----------------------------------------------------------------------------

void WizardThing::addTower()
{
	mNumTowers++;
	setNumTowers(mNumTowers);
}

//----------------------------------------------------------------------------

void WizardThing::removeTower()
{
	mNumTowers--;
	setNumTowers(mNumTowers);
}
	
//----------------------------------------------------------------------------

int WizardThing::numTowers()
{
	return mNumTowers;
}

//----------------------------------------------------------------------------

void WizardThing::addSentinel()
{
	mNumSentinels++;
	setNumSentinels(mNumSentinels);
}

//----------------------------------------------------------------------------

void WizardThing::removeSentinel()
{
	mNumSentinels--;
	setNumSentinels(mNumSentinels);
}
	
//----------------------------------------------------------------------------

int WizardThing::numSentinels()
{
	return mNumSentinels;
}

//----------------------------------------------------------------------------

void WizardThing::addGnome()
{
	mNumGnomes++;
	setNumGnomes(mNumGnomes);
}

//----------------------------------------------------------------------------

void WizardThing::removeGnome()
{
	mNumGnomes--;
	setNumGnomes(mNumGnomes);
}
	
//----------------------------------------------------------------------------

int WizardThing::numGnomes()
{
	return mNumGnomes;
}

//----------------------------------------------------------------------------

void WizardThing::addTick()
{
	mNumTicks++;
	setNumTicks(mNumTicks);
}

//----------------------------------------------------------------------------

void WizardThing::removeTick()
{
	mNumTicks--;
	setNumTicks(mNumTicks);
}
	
//----------------------------------------------------------------------------

int WizardThing::numTicks()
{
	return mNumTicks;
}

//----------------------------------------------------------------------------

void WizardThing::addAlbatross()
{
	mNumAlbatrosses++;
	setNumAlbatrosses(mNumAlbatrosses);
}

//----------------------------------------------------------------------------

void WizardThing::removeAlbatross()
{
	mNumAlbatrosses--;
	setNumAlbatrosses(mNumAlbatrosses);
}
	
//----------------------------------------------------------------------------

int WizardThing::numAlbatrosses()
{
	return mNumAlbatrosses;
}

//----------------------------------------------------------------------------

Real WizardThing::getGroundY(Vector3 pos)
{
	return BuildingHeightMap::getSingleton().getHeightAt(pos.x, pos.z);
}

//----------------------------------------------------------------------------
	
Real WizardThing::getGroundHeight(Vector3 pos)
{
	// check the terrain height
	Real w = getWidth();
	Real ground00 = getGroundY(pos + Vector3(-w,0,-w));
	Real ground01 = getGroundY(pos + Vector3(-w,0, w));
	Real ground10 = getGroundY(pos + Vector3( w,0,-w));
	Real ground11 = getGroundY(pos + Vector3( w,0, w));
	Real groundc  = getGroundY(pos + Vector3( 0,0, 0));

	Real ground = ground00;
	if (ground01 > ground) ground = ground01;
	if (ground10 > ground) ground = ground10;
	if (ground11 > ground) ground = ground11;
	if (groundc > ground)  ground = groundc;

	return ground;
}

//----------------------------------------------------------------------------

void WizardThing::moveForward()
{
	mForeward = true;
}

//----------------------------------------------------------------------------

void WizardThing::moveBack()
{
	mBack = true;
}

//----------------------------------------------------------------------------

void WizardThing::moveLeft()
{
	mLeft = true;
}

//----------------------------------------------------------------------------

void WizardThing::moveRight()
{
	mRight = true;
}

//----------------------------------------------------------------------------

Vector3 WizardThing::getDirection()
{
	return Vector3(1,0,0);
}

//----------------------------------------------------------------------------

void WizardThing::move(Real time)
{
	// check to see if its time to stop speeding yet
	if (mStopSpeedTime < Clock::getSingleton().getTime())
		mSpeeding = false;

	//handle movement for the camera and bots
	if ((getType() == CAMERATHING) || isBot())
	{
		// set the velocity according to the orientation
		mForce = Vector3(0,0,0);
// note to Mr. Prosser: or is a reserved C++ word for |, ONLY M$ doesn't implement it.
// -- jdpowell 20050603
		Real orient = getOrientation();
		if (mForeward && !mBack)
		{
			mForce.x += sin(orient);
			mForce.z += cos(orient);
		}
		if (mBack && !mForeward)
		{
			mForce.x -= sin(orient);
			mForce.z -= cos(orient);
		}
		if (mLeft && !mRight)
		{
			mForce.x += cos(orient);
			mForce.z -= sin(orient);
		}
		if (mRight && !mLeft)
		{
			mForce.x -= cos(orient);
			mForce.z += sin(orient);
		}

		mForce.normalise();
		mForce *= CONR("WIZARD_MOVE_SPEED");

		if (isSpeeding() || isGhost())
			mForce *= CONR("SPEEDSPELL_MULTIPLIER");

		Vector3 vel = getVelocity();

		if (mForce.length() == 0) vel -= vel*time*CONR("WIZARD_DECEL"); // slowing down
		else vel = mForce*time*CONR("WIZARD_ACCEL") + vel*(1-time*CONR("WIZARD_ACCEL")); // speeding up

		if (isSpeeding())
		{
			vel = getDirection();
			vel.normalise();
			vel *= CONR("WIZARD_MOVE_SPEED") * CONR("SPEEDSPELL_MULTIPLIER");
		}

		setVelocity(vel);

		mForeward = mBack = mLeft = mRight = false;
	}

	// regenerate mana
	if (!Multiplayer::getSingleton().isClient())
	{
		if (Clock::getSingleton().getTime() > mNextRegenTime)
		{
			mNextRegenTime = Clock::getSingleton().getTime() + CONT("WIZARD_MANA_REGEN_PERIOD");
			mActiveMana += CONI("WIZARD_MANA_REGEN") + mNumShrines*CONI("WIZARD_MANA_REGEN_BONUS");
			if (mActiveMana > mBaseMana)
				mActiveMana = mBaseMana;

			setActiveMana(mActiveMana);
		}
	}

	// float
	if (!(Multiplayer::getSingleton().isServer() && getType() == WIZARDTHING) && !mGhost) // dont make remote wizards float
	{
		float fall_max = CONR("WIZARD_MOVE_SPEED");
		float grav = CONR("WIZARD_GRAV");

		// stick to the ground when speeding on a lava map
		if (mSpeeding && Game::getSingleton().getLava())
		{
			setPosY(getGroundHeight(getPosition()));
		}

		// accelerate downward until we reach terminal velocity
		if (getVelY() > -fall_max)
		{
			setVelY(getVelY() - CONR("WIZARD_GRAV")*time);
		}
		else if (getVelY() < -fall_max) 
		{
			setVelY(-fall_max);
		}
	}

	mFrameTime = time;

	DamageableThing::move(time);
}

//----------------------------------------------------------------------------

void WizardThing::setPosition(Vector3 pos)
{
	// follow the landscape 
	Real ground = getGroundHeight(pos);
	ground += CONR("WIZARD_MIN_ALTITUDE");
			
	// dont ever go below ground
	if (ground > pos.y) 
	{
		if (getVelY() < 0)
			setVelY(0);

		pos.y = ground;
	}

	// speed limit
	if (mFrameTime > 0 && !mSpeeding)
	{
		Vector3 path = pos - getPosition();
		if (path.length() > CONR("WIZARD_MOVE_SPEED") * mFrameTime)
		{
			path.normalise();
			path *= CONR("WIZARD_MOVE_SPEED") * mFrameTime;

			pos = getPosition() + path;
		}
	}

	DamageableThing::setPosition(pos);

	mFrameTime = 0;

	// check for lava
	if (!Multiplayer::getSingleton().isClient())
		if (Game::getSingleton().getLava() && getPosY() <= CONR("HEIGTHMAP_MIN_HEIGHT") + CONR("WIZARD_MIN_ALTITUDE") + .01)
			die();
}

//----------------------------------------------------------------------------

void WizardThing::setVelY(Real vel)
{
	DamageableThing::setVelY(vel);
}

//----------------------------------------------------------------------------

void WizardThing::destroy()
{
	if (mTeam) Physics::getSingleton().removeTeam(getTeamNum());
	mTeam = 0;

	DamageableThing::destroy();
}

//----------------------------------------------------------------------------

void WizardThing::generateBitStream(BitStream& bitstream, int pid)
{
	DamageableThing::generateBitStream(bitstream,pid);

	bitstream.Write(mSkin);
}

//----------------------------------------------------------------------------

void WizardThing::interpretBitStream(BitStream& bitstream)
{
	DamageableThing::interpretBitStream(bitstream);

	int skin;

	bitstream.Read(skin);

	setSkin(skin);
}

//----------------------------------------------------------------------------

void WizardThing::placedInPhysics(int uid)
{
	DamageableThing::placedInPhysics(uid);

	if (!Multiplayer::getSingleton().isClient())
		mTeam->setWizardUID(uid);
}

//----------------------------------------------------------------------------

}
