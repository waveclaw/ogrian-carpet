#ifndef __OgrianRollingThing_H__
#define __OgrianRollingThing_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianThing.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class RollingThing : public Thing
{
public:
	RollingThing(String material, bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0) 
		: Thing(material, fixed_y, scale, x, y, z)
	{
		
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
};

}

#endif