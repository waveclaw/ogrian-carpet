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
#include "OgrianSkinManager.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------
	
WizardThing::WizardThing(bool visible, int skin) 
	: DamageableThing("Ogrian/Clear", visible?ORIENTEDSPRITE:SPRITE, 
	visible?"WizardThing":"CameraThing", true, CONR("CAMERA_HEIGHT"))
{
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

Thing* WizardThing::getRamp()
{
	return mRamp;
}

//----------------------------------------------------------------------------

void WizardThing::makeGhost()
{
	mGhost = true;
	if (mBar) mBar->destroy();
	mBar = 0;
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
		std::ostringstream num("");
		num << health;
		Multiplayer::getSingleton().serverSendText(String("Health: ") + num.str() , ID_SETHEALTH, player);
	}
}
//----------------------------------------------------------------------------

void WizardThing::die()
{
	DamageableThing::die();

	if (!Multiplayer::getSingleton().isClient())
	{
		if (Multiplayer::getSingleton().isServer() && getType() != CAMERATHING)
		{
			// kill it
			Castle* castle = getTeam()->getCastle();

			if (castle && castle->isRubble())
				Multiplayer::getSingleton().ghostWizard(this);
			else
				Multiplayer::getSingleton().killWizard(this);
		}			
	}
	
	setHealth(CONI("WIZARD_HEALTH"));
}

//----------------------------------------------------------------------------

// ignore external up/down velocity changes
void WizardThing::setVelocity(Vector3 vel)
{
	vel.y = getVelY();
	Thing::setVelocity(vel);
}

//----------------------------------------------------------------------------
	
void WizardThing::collided(Thing* e)
{

}

//----------------------------------------------------------------------------
	
void WizardThing::move(Real time)
{
	// float
	if (!mOnBuilding && getVelY() > -CONR("CAMERA_FALL_MAX")
		&& !(Multiplayer::getSingleton().isServer() && getType() == WIZARDTHING))
	{
		if (getVelY() > CONR("CAMERA_RISE_MAX")) 
			setVelY(CONR("CAMERA_RISE_MAX"));

		setVelY(getVelY() - CONR("CAMERA_GRAV")*time);
	}
	
	if (mOnBuilding) setVelY(0);

	if (Multiplayer::getSingleton().isClient() && getType() != CAMERATHING) setVelY(0);

	mOnBuilding = false;

	// follow the landscape
	Vector3 pos = getPosition();
	float ground = getGroundY(pos) + CONR("CAMERA_HEIGHT");
			
	if (ground > getPosY()) 
	{
		if (getVelY() < 0)
			setVelY(0);

		pos.y = ground;
		setPosition(pos);
	}

	DamageableThing::move(time);

	// update health bar
	if (mBar)
		mBar->update(getPosition(), getHealth()/100.0*getWidth());

	// update the ramp
	if (mRamp)
		mRamp->setPosition(pos);
}

//----------------------------------------------------------------------------

void WizardThing::setPosition(Vector3 pos)
{
	DamageableThing::setPosition(pos);
}

//----------------------------------------------------------------------------

void WizardThing::setVelY(Real vel)
{
	DamageableThing::setVelY(vel);

	if (vel == 0) mOnBuilding = true;
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