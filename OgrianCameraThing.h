/*****************************************************************************
	Copyright 2004 Mike Prosser

    This file is part of Ogrian Carpet.

    Ogrian Carpet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Ogrian Carpet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ogrian Carpet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

/*------------------------------------*
OgrianCameraThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a special Thing that is tied to the camera. 
It also handles the specifics of camera movement behavior. 
Later, this will derive from Wizard. 

/*------------------------------------*/

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
			mVel.y = 0;
			Thing::setVelocity(mVel);

			pos.y = ground;
			Thing::setPosition(pos);
		}
		else 
		{
			Thing::setPosition(pos);
		}
		

		camera->setPosition(mPos);
	}

	virtual void setVelocity(Vector3 vel)
	{
		vel.y = mVel.y;
		Thing::setVelocity(mVel);
	}
	virtual void move(Real time)
	{
		// fall
		if (mVel.y > -CAMERA_FALL_MAX)
		{
			mVel.y -= CAMERA_GRAV*time;
			Thing::setVelocity(mVel);
		}

		Thing::move(time);
	}
};
}
#endif