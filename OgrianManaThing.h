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
		setScale(sqrt(amount));
		mAmount=amount;
	}

	virtual unsigned int getAmount()
	{
		return mAmount;
	}

	virtual void collided(Thing* e)
	{
		setAmount(100);
	}

	virtual void move(Real time)
	{

	}

private:
	unsigned int mAmount;
};

}

#endif