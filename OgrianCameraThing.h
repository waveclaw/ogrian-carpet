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
This is never rendered.

/*------------------------------------*/

#ifndef __OgrianCameraThing_H__
#define __OgrianCameraThing_H__

#include <Ogre.h>
#include "OgrianWizardThing.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

class CameraThing : public WizardThing
{
public:
	CameraThing(Camera* camera)
		: WizardThing("Ogrian/Clear")
	{
		mCamera = camera;
	}

	virtual ThingType getType()
	{
		return CAMERATHING;
	}

	virtual Vector3 getDirection()
	{
		return mCamera->getDerivedDirection();	
	}

	// handle camera movement given directional input 
	virtual void moveCamera(Camera* camera, Real rotX, Real rotY, Vector3 trans)
	{
		mCamera = camera;

		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
		camera->yaw(rotX);
		camera->pitch(rotY);
		camera->moveRelative(trans);

		// follow the landscape
		Vector3 pos = camera->getPosition();
		float ground = getGroundY(pos) + CAMERA_HEIGHT;
				
		if (ground > getPosY()) 
		{
			setVelY(0);
			pos.y = ground;
		}
		else 
		{
			pos.y = getPosY();
		}
		setPosition(pos);

		camera->setPosition(getPosition());
	}

	// ignore external up/down velocity changes
	virtual void setVelocity(Vector3 vel)
	{
		vel.y = getVelY();
		Thing::setVelocity(vel);
	}

	// fall
	virtual void move(Real time)
	{
		if (getVelY() > -CAMERA_FALL_MAX)
		{
			setVelY(getVelY() - CAMERA_GRAV*time);
		}

		Thing::move(time);
	}

	// this must not be deleted!
	virtual void destroy()
	{
		assert(false);
	}

private:
	Camera* mCamera;

};
}
#endif