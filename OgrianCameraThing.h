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
It derives from wizard. 
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
	CameraThing(Camera* camera);
	virtual ~CameraThing();

	virtual ThingType getType();

	virtual Vector3 getDirection();

	virtual void moveForward();
	virtual void moveBack();
	virtual void moveLeft();
	virtual void moveRight();

	// handle camera movement given directional input 
	virtual void moveCamera(Real rotX, Real rotY);

	// ignore external up/down velocity changes
	virtual void setVelocity(Vector3 vel);

	virtual void move(Real time);

	virtual void die();

	// this must not be destroyed
	virtual void destroy() {}

private:
	Camera* mCamera;
	Camera* mTempCam;
	bool mForeward, mBack, mLeft, mRight;
	Vector3 mForce;

};
}
#endif