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
OgrianWizardThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The wizard thing is the superclass of the CameraThing

/*------------------------------------*/


#ifndef __OgrianWizardThing_H__
#define __OgrianWizardThing_H__

#include <Ogre.h>
#include "OgrianConst.h"
#include "OgrianDamageableThing.h"
#include "OgrianTeam.h"

using namespace Ogre;

namespace Ogrian
{

class HealthBarEffect : public Thing
{
public:
	HealthBarEffect(Vector3 pos, Real height)
		: Thing("Ogrian/Wizard/HealthBar", SPRITE, "HealthBar", true, 1, pos)
	{
		setHeight(height);
		mOffset = Vector3(0,CONR("WIZARD_HEALTHBAR_OFFSET"),0);
	}

	virtual void update(Vector3 pos, Real width)
	{	
		setPosition(pos+mOffset);
		setWidth(width);
	}

	virtual ThingType getType()	{ return EFFECT; }

private:
	Vector3 mOffset;
};

class RampThing : public Thing
{
public:
	RampThing(Thing* wizard) 
		: Thing("Ogrian/Clear", SPRITE, "Ramp", true, CONR("WIZARD_RAMP_SCALE")*2)
	{
		mWizard = wizard;
		setUpdateType(NEVER);
	}

	virtual void collided(Thing* e)
	{
		if (e->isBuilding())
		{
			Real topPosY = e->getPosY() + e->getHeight()/2.0;// + mWizard->getHeight()*.45;

			Real dist = axisDistance(e) - e->getWidth();
			if (dist < 0) dist = 0;

			Real ratio = dist / CONR("WIZARD_RAMP_SCALE");

			Real minY = topPosY - ratio * e->getHeight() + mWizard->getHeight();

			if (mWizard->getPosY() < minY)
			{
				mWizard->setPosY(minY);
				mWizard->setVelY(0);
			}
		}
	}

private:
	Thing* mWizard;
};

class WizardThing : public DamageableThing
{
public:
	WizardThing(bool visible=true, int skin=0);

	virtual ThingType getType()	{ return WIZARDTHING; }

	// ignore external up/down velocity changes
	virtual void setVelocity(Vector3 vel);

	// float
	virtual void move(Real time);

	// go over castle walls
	virtual void collided(Thing* e);

	virtual void setPosition(Vector3 pos);
	virtual void setVelY(Real vel);

	virtual void setColour(ColourValue& colour);
	virtual void setHealth(int health);
	virtual void damage(int amount, int sourceTeamNum);
	virtual void die();

	virtual void setBaseMana(int baseMana);
	virtual void setActiveMana(int activeMana);
	virtual void subtractActiveMana(int amount);
	virtual int getActiveMana();
	
	virtual void destroy();

	virtual void makeGhost(); // put this wizard into ghost mode
	virtual bool isGhost();

	virtual Team* getTeam();
	
	virtual void WizardThing::generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING);
	virtual void WizardThing::interpretBitStream(BitStream& bitstream);

	virtual void _setUID(int uid);

	virtual void setSkin(int skin);

	virtual Thing* getRamp();

	virtual void speed(Real duration);
	virtual bool isSpeeding();

private:
	RampThing* mRamp;
	HealthBarEffect* mBar;
	bool mOnBuilding;
	Team* mTeam;
	int mSkin;
	bool mGhost;
	bool mSpeeding;
	Time mStopSpeedTime;

	int mActiveMana;
	int mBaseMana;

	Time mNextRegenTime;
	Time mLastSetPosTime;

	virtual void setupSkins();
};

}

#endif