#ifndef __OgrianFoliageThing_H__
#define __OgrianFoliageThing_H__

#include <Ogre.h>
#include "OgrianRollingThing.h"

using namespace Ogre;

namespace Ogrian
{

class FoliageThing : public RollingThing
{
public:
	FoliageThing(String material, Real scale, Real x=0, Real y=0, Real z=0) 
		: RollingThing(material, "Foliage", true, scale, x, y, z)
	{
	}

	virtual ThingType getType()
	{
		return FOLIAGETHING;
	}

	virtual void collided(Thing* e)
	{
		// dies if it touches mana
		if (e->getType() == MANATHING)
			setMaterial("Ogrian/PalmTreeDead");
	}	
};

}

#endif