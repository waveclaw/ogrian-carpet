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

/*------------------------------------*/


#ifndef __OgrianManaThing_H__
#define __OgrianManaThing_H__

#include <Ogre.h>
#include "OgrianFloatingThing.h"

using namespace Ogre;

namespace Ogrian
{

class ManaThing : public FloatingThing
{
public:
	ManaThing(unsigned int amount, Real x=0, Real y=0, Real z=0) 
		: FloatingThing("Ogrian/Mana", "Mana", false, 1, x, y, z)
	{
		setAmount(amount);
	}

	virtual void setAmount(unsigned int amount)
	{
		mAmount=amount;
		if (amount > 1)
			setScale(sqrt(amount));
		else 
			setScale(1);
	}

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
				// absorb the other
				ManaThing* m = static_cast<ManaThing*>(e);
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

	virtual void move(Real time)
	{
		Vector3 vel;
		vel.x = HeightMap::getSingleton().getXSlopeAt(mPos.x, mPos.z);
		vel.z = HeightMap::getSingleton().getZSlopeAt(mPos.x, mPos.z);
		vel.normalise();
		vel *= MANA_DRIFT_SPEED;

		setVelocity(vel.x,0,vel.z);

		FloatingThing::move(time);
	}

	virtual ThingType getType()
	{
		return MANATHING;
	}

private:
	unsigned int mAmount;
};

}

#endif