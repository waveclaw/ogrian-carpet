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
#include "OgrianMultiplayer.h"
#include "OgrianHud.h"
#include "OgrianSkinManager.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------
	
WizardThing::WizardThing(bool visible, int skin) 
	: DamageableThing("Ogrian/Clear", visible?ORIENTEDSPRITE:SPRITE, 
	visible?"WizardThing":"CameraThing", true, CONR("WIZARD_SCALE"))
{
	mNextRegenTime = 0;
	mLastSetPosTime = 0;
	mSpeeding = false;
	mStopSpeedTime = 0;
	mActiveMana = 0;
	mBaseMana = 0;
	mNumHuts = 0;
	mBar = 0;
	mTeam = 0;
	mSkin = -1;
	mGhost = false;
	mRamp = new RampThing(this);
	Physics::getSingleton().addThing(mRamp);

	setSkin(skin);

	if (visible)
	{
		mBar = new HealthBarEffect(getPosition(), getHeight());
		Physics::getSingleton().addEffect(mBar);
	}

	// make a team for this wizard'
	if (Multiplayer::getSingleton().isServer())
	{		
		int teamNum;

		if (!visible) 
			teamNum = 0;
		else 
			teamNum = Physics::getSingleton().newTeam(getUID(), getColour());

		setTeamNum(teamNum);
		mTeam = Physics::getSingleton().getTeam(teamNum);
		mTeam->setScore(0);
		
		std::ostringstream num("");
		num << teamNum;
		LogManager::getSingleton().logMessage("Making server Team: " + num.str());
	}
	else if (!Multiplayer::getSingleton().isClient()) // skirmish mode
	{
		int teamNum = Physics::getSingleton().newTeam(0, getColour());
		setTeamNum(teamNum);
		mTeam = Physics::getSingleton().getTeam(teamNum);
		mTeam->setScore(0);

		std::ostringstream num("");
		num << teamNum;
		LogManager::getSingleton().logMessage("Making Team: " + num.str());
	}

	setUpdateType(CONTINUOUS);
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

Thing* WizardThing::getRamp()
{
	return mRamp;
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
	if (mBar) mBar->destroy();
	mBar = 0;
	
	// remove it from the teams enemy lists
	if (!Multiplayer::getSingleton().isClient())
		for (int i=0; i<Physics::getSingleton().numTeams(); i++)
			Physics::getSingleton().getTeam(i)->removeEnemy(this);
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
	getVisRep()->addPose(SkinManager::getSingleton().getSkin(skin));
	getVisRep()->setPose(0);
}

//----------------------------------------------------------------------------

void WizardThing::setupSkins()
{
	getVisRep()->addPose("Ogrian/Wizard/");
	getVisRep()->addPose("Ogrian/Jeff/");
	getVisRep()->addPose("Ogrian/Mike/");
	getVisRep()->addPose("Ogrian/Rick/");
	getVisRep()->addPose("Ogrian/Jess/");
	getVisRep()->addPose("Ogrian/Bunny/");

}

//----------------------------------------------------------------------------

void WizardThing::setColour(ColourValue& colour)
{
	DamageableThing::setColour(colour);
	if (mBar) mBar->setColour(colour);
	if (mTeam) mTeam->setColour(colour);
}

//----------------------------------------------------------------------------

void WizardThing::setHealth(int health)
{
	DamageableThing::setHealth(health);

	if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
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
			// kill it
			Castle* castle = getTeam()->getCastle();

			if (castle && castle->isRubble())
			{
				Multiplayer::getSingleton().ghostWizard(this);
				return;
			}
			else
				Multiplayer::getSingleton().killWizard(this);
		}			
	}
}

//----------------------------------------------------------------------------

// ignore external up/down velocity changes
void WizardThing::setVelocity(Vector3 vel)
{
	if (!mSpeeding)
		vel.y = getVelY();
	
	Thing::setVelocity(vel);
}

//----------------------------------------------------------------------------
	
void WizardThing::collided(Thing* e)
{
	// if its your castle
	if(!mGhost && e->getType() == CASTLEFLAGTHING && e->getTeamNum() == getTeamNum())
		setHealth(CONI("WIZARD_HEALTH"));
}

//----------------------------------------------------------------------------

void WizardThing::addHut()
{
	mNumHuts++;
}

//----------------------------------------------------------------------------

void WizardThing::removeHut()
{
	mNumHuts--;
}
	
//----------------------------------------------------------------------------
	
Real WizardThing::getGroundHeight()
{
	// check the terrain height
	Vector3 pos = getPosition();
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

	// check the building ramp height
	Real ramp = mRamp->getGroundHeight();

	// return the greater of the two
	if (ramp > ground) return ramp;
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
			mActiveMana += CONI("WIZARD_MANA_REGEN") + mNumHuts*CONI("WIZARD_MANA_REGEN_BONUS");
			if (mActiveMana > mBaseMana)
				mActiveMana = mBaseMana;

			setActiveMana(mActiveMana);
		}
	}

	// float
	if (!mSpeeding 
		&& !mRamp->isOnBuilding()
		&& getVelY() > -CONR("CAMERA_FALL_MAX")
		&& !(Multiplayer::getSingleton().isServer() && getType() == WIZARDTHING))
	{
		if (getVelY() > CONR("CAMERA_RISE_MAX")) 
			setVelY(CONR("CAMERA_RISE_MAX"));

		setVelY(getVelY() - CONR("CAMERA_GRAV")*time);
	}
	
	if (mRamp->isOnBuilding()) setVelY(0);

	if (Multiplayer::getSingleton().isClient() && getType() != CAMERATHING) setVelY(0);

	DamageableThing::move(time);

	// update health bar
	if (mBar)
		mBar->update(getPosition(), getHealth()/100.0*getWidth());

	// update the ramp
	if (mRamp)
		mRamp->setPosition(getPosition());
}

//----------------------------------------------------------------------------

void WizardThing::setPosition(Vector3 pos)
{
	// follow the landscape (and building ramps)
	Real ground = getGroundHeight();
	ground += CONR("CAMERA_HEIGHT");
			
	if (ground > getPosY()) 
	{
		if (getVelY() < 0)
			setVelY(0);

		pos.y = ground;
	}

	Vector3 lastPos = getPosition();

	if (!mSpeeding) // if we are not under the infuence of the speed spell
	{
		// if this is not a teleport
		Vector3 diff = pos - lastPos;
		Vector3 diffb = diff;
		diffb.y = 0;
		Time time = Clock::getSingleton().getTime();
		Real dt = (time - mLastSetPosTime)/1000.0;
		mLastSetPosTime = Clock::getSingleton().getTime();
		Real maxdist = CONR("CAMERA_MOVE_SPEED")*dt;
		if (diffb.length() <= maxdist*1.1 && diff.length() > maxdist)
		{
			// constrain the speed
			diff.normalise();
			diff *= CONR("CAMERA_MOVE_SPEED")*dt;
			pos = lastPos + diff;
		}
	}

	DamageableThing::setPosition(pos);

}

//----------------------------------------------------------------------------

void WizardThing::setVelY(Real vel)
{
	DamageableThing::setVelY(vel);
}

//----------------------------------------------------------------------------

void WizardThing::destroy()
{
	if (mBar) mBar->destroy();
	mBar = 0;

	if (mTeam) Physics::getSingleton().removeTeam(getTeamNum());
	mTeam = 0;

	if (mRamp) mRamp->destroy();
	mRamp = 0;

	DamageableThing::destroy();
}

//----------------------------------------------------------------------------

void WizardThing::generateBitStream(BitStream& bitstream, int pid)
{
	DamageableThing::generateBitStream(bitstream,pid);

	bitstream.Write(getHealth());
	bitstream.Write(mSkin);
}

//----------------------------------------------------------------------------

void WizardThing::interpretBitStream(BitStream& bitstream)
{
	DamageableThing::interpretBitStream(bitstream);

	int health, skin;

	bitstream.Read(health);
	bitstream.Read(skin);

	// ignore incoming health if this is a server
	if (Multiplayer::getSingleton().isClient()) setHealth(health);
	setSkin(skin);
}
//----------------------------------------------------------------------------

}