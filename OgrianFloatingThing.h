#ifndef __OgrianFloatingThing_H__
#define __OgrianFloatingThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianConstants.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class FloatingThing : public Thing
{
public:
	FloatingThing(String material, String prefix="FloatingThing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0) 
		: Thing(material, prefix, fixed_y, scale, x, y, z)
	{
		
	}
	virtual ThingType getType()
	{
		return FLOATINGTHING;
	}

	virtual void setPosition(const Vector3 pos)
	{
		setPosition(pos.x, pos.y, pos.z);
	}

	virtual void setVelocity(Real x, Real y, Real z)
	{
		Thing::setVelocity(x,mVel.y,z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		Real ground = HeightMap::getSingleton().getHeightAt(x,z) + getRadius();
		if (ground > mPos.y) 
		{
			Thing::setVelocity(mVel.x,0,mVel.y);
			Thing::setPosition(x,ground,z);
		}
		else 
		{
			Thing::setPosition(x,y,z);
		}
	}

	virtual void move(Real time)
	{
		// fall
		if (mVel.y > -FLOAT_FALL_MAX)
			Thing::setVelocity(mVel.x, mVel.y-FLOAT_GRAV*time, mVel.z);

		Thing::move(time);
	}
};

}

#endif