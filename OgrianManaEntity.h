#ifndef __OgrianManaEntity_H__
#define __OgrianManaEntity_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianRollingEntity.h"

using namespace Ogre;

namespace Ogrian
{

class ManaEntity : public PhysicalEntity
{
public:
	ManaEntity(String material, unsigned int amount, Real x=0, Real y=0, Real z=0) 
		: PhysicalEntity(material, false, 1, x, y, z)
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
		PhysicalEntity::setPosition(x,
			HeightMap::getSingleton().getHeightAt(x,z) + getRadius(),
			z);
	}

	virtual void collided(PhysicalEntity* e)
	{

	}

private:
	unsigned int mAmount;
};

}

#endif