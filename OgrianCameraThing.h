#ifndef __OgrianCameraThing_H__
#define __OgrianCameraThing_H__

#include <Ogre.h>
#include "OgrianThing.h"
#include "OgrianConstants.h"

using namespace Ogre;

namespace Ogrian
{

class CameraThing : public Thing
{
public:
	CameraThing(Camera* camera)
		: Thing("", "CameraThing", true, CAMERA_HEIGHT)
	{
	}

	virtual ThingType getType()
	{
		return CAMERATHING;
	}

	virtual void moveCamera(Camera* camera, Real rotX, Real rotY, Vector3 trans)
	{
		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
		camera->yaw(rotX);
		camera->pitch(rotY);
		camera->moveRelative(trans);

		// follow the landscape
		Vector3 pos = camera->getPosition();
		float ground = HeightMap::getSingleton().getHeightAt(pos.x, pos.z) + CAMERA_HEIGHT;
				
		if (ground > mPos.y) 
		{
			Thing::setVelocity(mVel.x,0,mVel.y);
			Thing::setPosition(pos.x,ground,pos.z);
		}
		else 
		{
			Thing::setPosition(pos.x,mPos.y,pos.z);
		}
		

		camera->setPosition(mPos);
	}

	virtual void setVelocity(Real x, Real y, Real z)
	{
		Thing::setVelocity(x, mVel.y, z);
	}
	virtual void move(Real time)
	{
		// fall
		if (mVel.y > -CAMERA_FALL_MAX)
			Thing::setVelocity(mVel.x, mVel.y-CAMERA_GRAV*time, mVel.z);

		Thing::move(time);
	}
};
}
#endif