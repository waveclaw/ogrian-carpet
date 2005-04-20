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

Description: The wizard thing is the superclass of the CameraThing

/*------------------------------------*/


#include "OgrianWizardThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianMultiplayer.h"
#include "OgrianHud.h"
#include "OgrianBuildingHeightmap.h"
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
	mNumShrines = 0;
	mTeam = 0;
	mGhost = false;
	mFrameTime = 0;

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
	{
		Multiplayer::getSingleton().serverAddCameraPlayer(this);
	}

}

//----------------------------------------------------------------------------

void WizardThing::speed(Real duration)
{
	mStopSpeedTime = Clock::getSingleton().getTime() + duration*1000;

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
	{
		Hud::getSingleton().setActiveMana(activeMana);
	}

	// send out the update if this is a server
	else if (Multiplayer::getSingleton().isServer())
	{
		// find the wizard's player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

		// update it
		Multiplayer::getSingleton().serverSendInt(activeMana, ID_SET_ACTIVE_MANA, player);
	}
}

//----------------------------------------------------------------------------

void WizardThing::setBaseMana(int baseMana)
{
	mBaseMana = baseMana;
	
	// update the hud if this is a camerathing
	if (getType() == CAMERATHING)
	{
		Hud::getSingleton().setBaseMana(baseMana);
	}

	// send out the update if this is a server
	else if (Multiplayer::getSingleton().isServer())
	{
		// find the wizard's player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

		// update it
		Multiplayer::getSingleton().serverSendInt(baseMana, ID_SET_BASE_MANA, player);
	}
}

//----------------------------------------------------------------------------

void WizardThing::subtractActiveMana(int amount)
{
	if (amount == 0) return;

	// send an update if this is a client
	if (Multiplayer::getSingleton().isClient())
		Multiplayer::getSingleton().clientSendInt(mActiveMana - amount, ID_SET_ACTIVE_MANA);
	else
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

void WizardThing::setColour(ColourValue& colour)
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

	if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING && getUID() >= 0)
	{
		// find the wizard's player
		PlayerID player = Multiplayer::getSingleton().getPlayerID(getUID());

		// update it
		Multiplayer::getSingleton().serverSendInt(health, ID_SET_HEALTH, player);
	}
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
	// if its your castle
	if(!mGhost && e->getType() == CASTLEFLAGTHING && e->getTeamNum() == getTeamNum())
		setHealth(CONI("WIZARD_HEALTH"));

	// claim mana and Shrines
	if ((e->getType() == MANATHING || e->getType() == SHRINETHING || e->getType() == SHRINEBALLTHING)
			&& e->getTeamNum() != getTeamNum())
		e->claim(getTeamNum());
}

//----------------------------------------------------------------------------

void WizardThing::addShrine()
{
	mNumShrines++;
}

//----------------------------------------------------------------------------

void WizardThing::removeShrine()
{
	mNumShrines--;
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

void WizardThing::move(Real time)
{
	// check to see if its time to stop speeding yet
	if (mStopSpeedTime < Clock::getSingleton().getTime())
		mSpeeding = false;

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
		float fall_max = CONR("CAMERA_FALL_MAX");
		float grav = CONR("CAMERA_GRAV");

		// fall faster when speeding on a lava map
		if (mSpeeding && Game::getSingleton().getLava())
		{
			fall_max *= CONR("SPEEDSPELL_MULTIPLIER");
			grav *= CONR("SPEEDSPELL_MULTIPLIER");
		}

		// accelerate downward until we reach terminal velocity
		if (getVelY() > -fall_max)
		{
			setVelY(getVelY() - CONR("CAMERA_GRAV")*time);
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
	if (mFrameTime > 0)
	{
		Vector3 path = pos - getPosition();
		if (path.length() > CONR("CAMERA_MOVE_SPEED") * mFrameTime)
		{
			path.normalise();
			path *= CONR("CAMERA_MOVE_SPEED") * mFrameTime;

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