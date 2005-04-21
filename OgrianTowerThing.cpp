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
OgrianTower.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a tower to stand on. it also claims nearby mana and has
a flock of cranes

/*------------------------------------*/

#include <Ogre.h>
#include "OgrianTowerThing.h"
#include "OgrianHeightMap.h"
#include "OgrianClaimSpellThing.h"
#include "OgrianMultiplayer.h"
#include "OgrianSkinManager.h"
#include "OgrianConst.h"
#include "OgrianModel.h"
#include "OgrianCraneThing.h"
#include "OgrianPhysics.h"
#include "OgrianManaThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////


TowerBallThing::TowerBallThing(int teamNum)
	: Thing("Ogrian/TowerFlag", SPRITE, "TowerFlag", false, CONR("TOWER_BALL_SCALE"), Vector3(0,0,0), SPHERE)
{
	setTeamNum(teamNum);
	setThinkPeriod(CONR("TOWER_WIZ_REGEN_PERIOD"));
	mHeal = false;
}

void TowerBallThing::think()
{
	mHeal = true;
}

void TowerBallThing::collided(Thing* e)
{
	// periodically heal own team's wizard
	if (mHeal)
		if (e->getType() == WIZARDTHING || e->getType() == CAMERATHING)
			if (e->getTeamNum() == getTeamNum())
			{
				mHeal = false;
				DamageableThing* d = (DamageableThing*)e;
				d->setHealth(d->getHealth() + CONI("TOWER_WIZ_REGEN"));
			}

}


/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

TowerThing::TowerThing(int teamNum, Vector3 pos, int skin) 
	: DamageableThing("Ogrian/Tower", MODEL, "Tower", false, CONR("TOWER_WIDTH"), pos, CUBE, false)
{
	mLastCastTime = 0;
	mUnbuildMode = false;
	mBall = 0;
	mPortal = 0;
	mBeacon = 0;

	// set up the beacon
	mBeacon = new TowerBeaconThing();
	Physics::getSingleton().addEffect(mBeacon);

	Vector3 bpos = getPosition();
	bpos.y += CONR("TOWER_BEACON_ALTITUDE");
	mBeacon->setPosition(bpos);

	setThinkPeriod(CONR("TOWER_THINK_PERIOD"));

	setSkin(skin);

	// set the height
	setHeight(CONR("TOWER_HEIGHT"));

	// find the ground	
	Real w = getWidth()/2;
	Real ground00 = getGroundY(pos + Vector3(-w,0,-w));
	Real ground01 = getGroundY(pos + Vector3(-w,0, w));
	Real ground10 = getGroundY(pos + Vector3( w,0,-w));
	Real ground11 = getGroundY(pos + Vector3( w,0, w));

	Real ground = ground00;
	if (ground01 < ground) ground = ground01;
	if (ground10 < ground) ground = ground10;
	if (ground11 < ground) ground = ground11;
	mGroundY = ground;

	// start at zero and rise
	setPosY(mGroundY - CONR("TOWER_OFFSET"));
	mTargetY = getPosY();

	// set the health
	setHealth(CONI("TOWER_HEALTH"));

	// set the team
	setTeamNum(teamNum);

	if (!Multiplayer::getSingleton().isClient())
	{
		mColour = Physics::getSingleton().getTeam(teamNum)->getColour();
		setColour(mColour);

		// set up the ball
		mBall = new TowerBallThing(teamNum);
		mBall->setColour(mColour);
		Vector3 fpos = pos;
		fpos.y = HeightMap::getSingleton().getHeightAt(fpos.x, fpos.z) + CONR("TOWER_BALL_ALTITUDE");
		mBall->setPosition(fpos);
		Physics::getSingleton().addThing(mBall);

		// make the crane flock
		Vector3 pos = getPosition();
		pos.y += getHeight() - CONR("TOWER_OFFSET");
		for (int i=0; i<CONI("TOWER_NUM_CRANES"); i++)
		{
			CraneThing* crane = new CraneThing(teamNum,pos);
			mCranes.push_back(crane);
			Physics::getSingleton().addThing(crane);
		}

		// set up the portal
		Vector3 ppos = pos;
		ppos.x += CONR("PORTAL_TOWER_DIST");
		ppos.y = HeightMap::getSingleton().getHeightAt(ppos.x, ppos.z) + CONR("PORTAL_ALTITUDE");
		mPortal = new PortalThing(this, ppos);
		Physics::getSingleton().addThing(mPortal);

		// notify our wizard
		Team* team = Physics::getSingleton().getTeam(teamNum);
		WizardThing* wiz = (WizardThing*)Physics::getSingleton().getThing(team->getWizardUID());
		if (wiz) wiz->addTower();
	}

}

//----------------------------------------------------------------------------

void TowerThing::setSkin(int skin)
{
	mSkin = skin;

	// set the mesh
	String mesh = SkinManager::getSingleton().getTowerSkin(skin);
	static_cast<Model*>(getVisRep())->setMesh(mesh);
}

//----------------------------------------------------------------------------

void TowerThing::setColour(ColourValue& colour)
{
	DamageableThing::setColour(colour);
	mBeacon->setColour(colour);
}

//----------------------------------------------------------------------------

void TowerThing::setPosition(Vector3 pos)
{
	DamageableThing::setPosition(pos);

	pos.y += CONR("TOWER_BEACON_ALTITUDE");
	mBeacon->setPosition(pos);
}

//----------------------------------------------------------------------------

void TowerThing::unbuild()
{
	mUnbuildMode = true;
}

//----------------------------------------------------------------------------

void TowerThing::die()
{
	if (!isAlive()) return;

	if (mUnbuildMode)
	{
		// return the mana to the castle
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		if (team && team->getCastle())
		{
			int mana = team->getCastle()->getMana() + CONI("TOWER_COST");
			team->getCastle()->setMana(mana);
		}
	}
	else
	{	// return some mana to the castle
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		if (team && team->getCastle())
		{
			int mana = team->getCastle()->getMana() + CONI("TOWER_COST") - CONI("TOWER_DROP");
			team->getCastle()->setMana(mana);

			// drop the rest of our mana
			ManaThing* manathing = new ManaThing(CONI("TOWER_DROP"), getPosition());
			manathing->setTeamNum(getTeamNum());
			Physics::getSingleton().addThing(manathing);;
		}
		else
		{
			// drop all of our mana
			ManaThing* manathing = new ManaThing(CONI("TOWER_COST"), getPosition());
			manathing->setTeamNum(getTeamNum());
			Physics::getSingleton().addThing(manathing);;
		}
	}

	// notify our wizard
	Team* team = Physics::getSingleton().getTeam(getTeamNum());
	WizardThing* wiz = (WizardThing*)team->getWizard();
	if (wiz) wiz->removeTower();

	destroy();
}

//----------------------------------------------------------------------------

void TowerThing::destroy()
{
	// remove the cranes
	for (int i=0; i<(int)mCranes.size(); i++)
		mCranes[i]->destroy();

	mCranes.clear();

	if (mBeacon)
		mBeacon->destroy();
	if (mBall)
		mBall->destroy();
	if (mPortal)
		mPortal->destroy();

	DamageableThing::destroy();
}

//----------------------------------------------------------------------------

// set how far up this block should go to
void TowerThing::setPercentage(Real per)
{
	if (per >= 1) per = 1;
	if (per <= 0) per = 0;
	Real newTargetY = mGroundY - CONR("TOWER_OFFSET") - getHeight()/2 + getHeight()*per;

	if (newTargetY == mTargetY) return;

	mTargetY = newTargetY;

	if (mTargetY > getPosY())
	{
		setVelY(CONR("TOWER_RISE_SPEED"));
	}
	else
	{
		setVelY(0-CONR("TOWER_RISE_SPEED"));
	}

	setUpdateFlag();
}

//----------------------------------------------------------------------------

void TowerThing::move(Real time)
{
	// if it has reached its target pos.y, stop
	if (!Multiplayer::getSingleton().isClient() && (
		(getVelY() < 0 && mTargetY > getPosY()) ||
		(getVelY() > 0 && mTargetY < getPosY())))
	{
		setPosY(mTargetY);
		setVelY(0);
		setUpdateFlag();
	}

	DamageableThing::move(time);
}

//----------------------------------------------------------------------------

void TowerThing::think()
{
	if (mUnbuildMode)
	{
		// un-regenerate
		int health = getHealth();
		setHealth(health - CONI("TOWER_DEGEN"));

		return;
	}

	// regenerate
	int health = getHealth();
	if (health < CONI("TOWER_HEALTH"))
		setHealth(health + CONI("TOWER_REGEN"));
	if (health > CONI("TOWER_HEALTH"))
		setHealth(CONI("TOWER_HEALTH"));

	// cast a mana at the nearest mana or shrine in range
	Thing* target = 0;
	Real bestDist = CONR("TOWER_RANGE");
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* candidate = Physics::getSingleton().getThingByIndex(i);
		int type = candidate->getType();
		if (candidate 
			&& (type == MANATHING || type == SHRINETHING || type == SENTINELTHING 
				|| type == TICKTHING || type == GNOMETHING)
			&& cylinderDistance(candidate) < bestDist
			&& candidate->isAlive() )
		{
			// only claim mana and shrines of a different colour
			if (type == MANATHING || type == SHRINETHING)
			{
				if (candidate->getColour() != getColour())
				{
					target = candidate;
					bestDist = cylinderDistance(candidate);
				}
			}

			// heal allies who need health
			else
			{
				int maxHealth = 0;

				if (type == SENTINELTHING) maxHealth = CONI("SENTINEL_HEALTH");
				if (type == GNOMETHING) maxHealth = CONI("GNOME_HEALTH");
				if (type == TICKTHING) maxHealth = CONI("TICK_HEALTH");

				if (candidate->getColour() == getColour()
					&& ((DamageableThing*)candidate)->getHealth() < maxHealth)
				{
					target = candidate;
					bestDist = cylinderDistance(candidate);
				}
			}
		}
	}

	if (target) 
	{
		Vector3 startPos = getPosition();
		startPos.y += getHeight()/2 - CONR("TOWER_OFFSET");

		// account for target movement
		Real claimTravelTime = sphereDistance(target) / CONR("CLAIMSPELL_SPEED");
		Vector3 targetOffset = target->getVelocity()*claimTravelTime;

		Vector3 vel = (target->getPosition() + targetOffset) - startPos;
		vel.normalise();
		vel *= CONR("CLAIMSPELL_SPEED");

		Physics::getSingleton().addThing(new ClaimSpellThing(getTeamNum(), mColour , startPos, 
			vel, CONR("TOWER_CLAIM_LIFETIME")));
	}
	}
	
//----------------------------------------------------------------------------

// set the percentage to match the health
void TowerThing::setHealth(int health)
{
	DamageableThing::setHealth(health);

	if (health <= 0) die();

	setPercentage((health + CONR("TOWER_RUBBLE"))
		/ (CONR("TOWER_HEALTH") + CONR("TOWER_RUBBLE")));
}

//----------------------------------------------------------------------------

// exit unbuild mode if damaged
void TowerThing::damage(int amount, int source)
{
	mUnbuildMode = false;

	DamageableThing::damage(amount, source);
}

//----------------------------------------------------------------------------

void TowerThing::generateBitStream(BitStream& bitstream, int pid)
{
	DamageableThing::generateBitStream(bitstream,pid);

	bitstream.Write(mSkin);
}

//----------------------------------------------------------------------------

void TowerThing::interpretBitStream(BitStream& bitstream)
{
	DamageableThing::interpretBitStream(bitstream);

	int skin;

	bitstream.Read(skin);

	setSkin(skin);
}

//----------------------------------------------------------------------------

}