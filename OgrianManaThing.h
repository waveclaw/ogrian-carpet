#ifndef __OgrianManaThing_H__
#define __OgrianManaThing_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianRollingThing.h"

using namespace Ogre;

namespace Ogrian
{

class ManaThing : public Thing
{
public:
	ManaThing(String material, unsigned int amount, Real x=0, Real y=0, Real z=0) 
		: Thing(material, false, 1, x, y, z)
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

	virtual void setPosition(Vector3 pos)
	{
		setPosition(pos.x, pos.y, pos.z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		Thing::setPosition(x,
			HeightMap::getSingleton().getHeightAt(x,z) + getRadius(),
			z);
	}

	virtual void collided(Thing* e)
	{

	}

private:
	unsigned int mAmount;
};

}

#endif