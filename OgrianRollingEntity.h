#ifndef __OgrianRollingEntity_H__
#define __OgrianRollingEntity_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianPhysicalEntity.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class RollingEntity : public PhysicalEntity
{
public:
	RollingEntity(String material, Real x=0, Real y=0, Real z=0) : PhysicalEntity(material, x, y, z)
	{
		
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
};

}

#endif