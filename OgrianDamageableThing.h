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
OgrianDamageableThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: DamageableThing is a thing that can take damage, and will die
It also has an optional health bar

 *------------------------------------*/

#ifndef __OgrianDamageableThing_H__
#define __OgrianDamageableThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianClock.h"

using namespace Ogre;

namespace Ogrian
{

//////////////////////////////////////////////////////////////////////////////////////
class DamageableThing : public Thing
{
public:

	DamageableThing(String material, ThingVisRep visrep=SPRITE, String prefix="Thing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE, bool hasBar=true);

	// makes damage happen
	virtual void setHealth(int health);
	virtual void heal(int amount);
	virtual int getHealth();
	virtual int getMaxHealth();
	virtual void damage(int amount, int sourceTeamNum);
	virtual int getLastDamageSourceTeamNum();
	virtual void setTeamNum(int teamNum);
	virtual void destroy();
	virtual bool isDamageable()	{ return true; }

	// used for health bars
	virtual void setMaxHealth(int maxHealth);
	virtual void reset();	
	virtual void setColour(ColourValue colour);
	virtual void move(Real time);
	virtual bool hasHealthBar();

	// to update the health bar
	virtual void generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING);
	virtual void interpretBitStream(BitStream& bitstream);

private:
	int mHealth;
	int mMaxHealth;
	int mLastDamageSource;

	bool mHasBar;

	Billboard* mBar;
};

}

#endif
