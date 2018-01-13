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

 *------------------------------------*/


#ifndef __OgrianWizardThing_H__
#define __OgrianWizardThing_H__

#include <Ogre.h>
#include "OgrianConst.h"
#include "OgrianDamageableThing.h"
#include "OgrianTeam.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianMultiplayer.h"
#include "OgrianHud.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianSkinManager.h"

using namespace Ogre;

namespace Ogrian
{

enum MessageType
{
	SET_HEALTH,
	SET_MANA,
	SET_MANASTONE,

	SET_NUM_MANABALLS,
	SET_NUM_SHRINES,
	SET_NUM_TOWERS,
	SET_NUM_SENTINELS,
	SET_NUM_GNOMES,
	SET_NUM_TICKS,
	SET_NUM_ALBATROSSES,

	SET_SCORE,
};

class WizardThing : public DamageableThing
{
public:
	WizardThing(bool visible=true, int skin=0);

	// reset the wizard
	virtual void reset();

	virtual ThingType getType()	{ return WIZARDTHING; }

	virtual bool isBot() { return false; }

	// ignore external up/down velocity changes
	virtual void setVelocity(Vector3 vel);

	// smooth movement for bots and cameras
	virtual void moveForward();
	virtual void moveBack();
	virtual void moveLeft();
	virtual void moveRight();
	virtual Vector3 getDirection();


	// float
	virtual void move(Real time);

	virtual void collided(Thing* e);

	virtual void setPosition(Vector3 pos);
	virtual void setVelY(Real vel);

	virtual void setColour(ColourValue colour);
	virtual void setHealth(int health);
	virtual void damage(int amount, int sourceTeamNum);
	virtual void die();

	virtual void setManaStone(int manaStone);
	virtual void setMana(int mana);
	virtual void subtractMana(int amount);
	virtual int getMana();

	virtual int getLevel();
	
	virtual void destroy();

	virtual void makeGhost(); // put this wizard into ghost mode
	virtual bool isGhost();

	virtual Team* getTeam();
	
	// for keeping the skin updated
	virtual void generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING);
	virtual void interpretBitStream(BitStream& bitstream);

	virtual void _setUID(int uid);

	virtual void setSkin(int skin);

	virtual void speed(Real duration);
	virtual bool isSpeeding();

	// each shrine adds a point to regeneration
	virtual void addShrine();
	virtual void removeShrine();
	virtual int numShrines();

	// track these
	virtual void addManaBall(int amount);
	virtual void removeManaBall(int amount);
	virtual int numManaBalls();

	virtual void addTower();
	virtual void removeTower();
	virtual int numTowers();

	virtual void addSentinel();
	virtual void removeSentinel();
	virtual int numSentinels();

	virtual void addGnome();
	virtual void removeGnome();
	virtual int numGnomes();

	virtual void addTick();
	virtual void removeTick();
	virtual int numTicks();

	virtual void addAlbatross();
	virtual void removeAlbatross();
	virtual int numAlbatrosses();

	// update the hud
	virtual void handleMessage(int msg, Vector3 vec, int val);

	virtual Real getGroundY(Vector3 pos);

	virtual void placedInPhysics(int uid);

private:
	Team* mTeam;
	int mSkin;
	bool mVisible;
	bool mGhost;
	bool mSpeeding;
	Time mStopSpeedTime;
	Real mFrameTime;

	int mMana;
	int mManaStone;

	int mNumManaBalls;
	int mNumShrines;
	int mNumTowers;
	int mNumSentinels;
	int mNumGnomes;
	int mNumTicks;
	int mNumAlbatrosses;

	Time mNextRegenTime;
	Time mLastSetPosTime;

	// for smooth movement
	bool mForeward, mBack, mLeft, mRight;
	Vector3 mForce;
	Vector3 mDir;

	Real getGroundHeight(Vector3 pos);

	void setNumManaBalls(int num);
	void setNumShrines(int num);
	void setNumTowers(int num);
	void setNumSentinels(int num);
	void setNumGnomes(int num);
	void setNumTicks(int num);
	void setNumAlbatrosses(int num);
	
	// update the score for this wizard
	void setScore();
};

}

#endif
