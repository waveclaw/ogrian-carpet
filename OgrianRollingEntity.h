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
	RollingEntity(String material, bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0) 
		: PhysicalEntity(material, fixed_y, scale, x, y, z)
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
};

}

#endif