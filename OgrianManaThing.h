#ifndef __OgrianManaThing_H__
#define __OgrianManaThing_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianRollingThing.h"

using namespace Ogre;

namespace Ogrian
{

class ManaThing : public RollingThing
{
public:
	ManaThing(unsigned int amount, Real x=0, Real y=0, Real z=0) 
		: RollingThing("Ogrian/Mana", "Mana", false, 1, x, y, z)
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

		RollingThing::move(time);
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