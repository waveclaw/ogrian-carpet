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
OgrianManaThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the ManaThing is a ball of mana that derives from FloatingThing. 
They drift downhill and combine when they touch. 
They have some amount of mana.
When they combine, one transfers all of its amount to the other.

/*------------------------------------*/


#ifndef __OgrianManaThing_H__
#define __OgrianManaThing_H__

#include <Ogre.h>
#include "OgrianFloatingThing.h"

using namespace Ogre;

namespace Ogrian
{

// extend floatingThing for floating behavior
class ManaThing : public FloatingThing
{
public:
	ManaThing(unsigned int amount=1, Vector3 pos = Vector3(0,0,0)) 
		: FloatingThing("Ogrian/Mana", "ManaThing", false, 1, pos, SPHERE)
	{
		setAmount(amount);
		playSound("OgrianMedia/sounds/hum1.wav", true);
	}

	// setting the amount automatically sets the scale
	virtual void setAmount(unsigned int amount)
	{
		if (mAmount == amount) return;

		mAmount=amount;
		if (amount > 1)
			setScale(sqrt(amount));
		else 
			setScale(1);
	}

	// get the amount of mana this represents
	virtual inline unsigned int getAmount()
	{
		return mAmount;
	}

	// When two manas collide, they combine - one takes on all of the amount of the other
	// and the other disapears
	virtual void collided(Thing* e)
	{
		if (e->getType() == MANATHING)
		{
			if(getAmount() > 0)
			{
				ManaThing* m = static_cast<ManaThing*>(e);

				// set the position as a weighted average of the two
				setPosition(
					(getPosition()*getAmount() + m->getPosition()*m->getAmount())
						/ (getAmount() + m->getAmount()));
						
				// absorb the other
				setAmount(getAmount() + m->getAmount());
				m->setAmount(0);
			}
			else
			{
				// be absorbed
				destroy();
			}
		}
	}

	// move downhill at a constant slow velocity
	virtual void move(Real time)
	{
		Vector3 vel;
		Vector3 pos = getPosition();
		vel.x = HeightMap::getSingleton().getXSlopeAt(pos.x, pos.z);
		vel.z = HeightMap::getSingleton().getZSlopeAt(pos.x, pos.z);
		vel.normalise();
		vel *= MANA_DRIFT_SPEED;

		setVelocity(vel);

		FloatingThing::move(time);
	}

	virtual ThingType getType()
	{
		return MANATHING;
	}

	virtual void generateBitStream(BitStream& bitstream)
	{
		Thing::generateBitStream(bitstream);
		bitstream.Write(getAmount());
	}
	virtual void interpretBitStream(BitStream& bitstream)
	{
		int amount;

		Thing::interpretBitStream(bitstream);
		bitstream.Read(amount);

		setAmount(amount);
	}

private:
	unsigned int mAmount;
};

}

#endif