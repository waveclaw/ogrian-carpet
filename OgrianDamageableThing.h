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

/*------------------------------------*/

#ifndef __OgrianDamageableThing_H__
#define __OgrianDamageableThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianTime.h"

using namespace Ogre;

namespace Ogrian
{

class DamageableThing : public Thing
{
public:

	DamageableThing(String material, ThingVisRep visrep=SPRITE, String prefix="Thing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE)
		: Thing(material, visrep, prefix, fixed_y, scale, pos, shape)
	{
		mHealth = 0;
	}

	virtual void setHealth(int health)
	{
		mHealth = health;
	}

	virtual int getHealth()
	{
		return mHealth;
	}

	virtual void damage(int amount, int sourceTeamNum)
	{
		mLastDamageSource = sourceTeamNum;

		setHealth(mHealth - amount);
		if (mHealth <= 0) die();
	}

	virtual int getLastDamageSourceTeamNum()
	{
		return mLastDamageSource;
	}

	virtual bool isDamageable()
	{
		return true;
	}

	virtual void generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING)
	{
		Thing::generateBitStream(bitstream,pid);
		bitstream.Write(mHealth);
	}

	virtual void interpretBitStream(BitStream& bitstream)
	{
		Thing::interpretBitStream(bitstream);
		bitstream.Read(mHealth);

		setHealth(mHealth);
	}

private:
	int mHealth;
	int mLastDamageSource;
};

}

#endif