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
OgrianCameraThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a special Thing that is tied to the camera. 
It also handles the specifics of camera movement behavior. 
It derives from wizard. 
This is never rendered.

/*------------------------------------*/

#include "OgrianCameraThing.h"


namespace Ogrian
{

//----------------------------------------------------------------------------

CameraThing::CameraThing(Camera* camera) : WizardThing(false)
{
	mCamera = camera;
	mForeward = mBack = mLeft = mRight = false;
}

//----------------------------------------------------------------------------

ThingType CameraThing::getType()
{
	return CAMERATHING;
}

//----------------------------------------------------------------------------

Vector3 CameraThing::getDirection()
{
	return mCamera->getDerivedDirection();	
}

//----------------------------------------------------------------------------

void CameraThing::moveForward()
{
	mForeward = true;
}

//----------------------------------------------------------------------------

void CameraThing::moveBack()
{
	mBack = true;
}

//----------------------------------------------------------------------------

void CameraThing::moveLeft()
{
	mLeft = true;
}

//----------------------------------------------------------------------------

void CameraThing::moveRight()
{
	mRight = true;
}

//----------------------------------------------------------------------------

// handle camera movement given directional input 
void CameraThing::moveCamera(Real rotX, Real rotY)
{
	// Make all the changes to the camera
	mCamera->yaw(rotX);
	mCamera->pitch(rotY);

	// set the orientation of the thing to match the camera
	Vector3 dir = mCamera->getDirection();
	setOrientation(atan2(dir.x, dir.z));
}

//----------------------------------------------------------------------------

// ignore external up/down velocity changes
void CameraThing::setVelocity(Vector3 vel)
{
	vel.y = getVelY();
	Thing::setVelocity(vel);
}

//----------------------------------------------------------------------------

void CameraThing::move(Real time)
{
	// set the velocity according to the orientation
	mForce = Vector3(0,0,0);
	Real or = getOrientation();
	if (mForeward && !mBack)
	{
		mForce.x += sin(or);
		mForce.z += cos(or);
	}
	if (mBack && !mForeward)
	{
		mForce.x -= sin(or);
		mForce.z -= cos(or);
	}
	if (mLeft && !mRight)
	{
		mForce.x += cos(or);
		mForce.z -= sin(or);
	}
	if (mRight && !mLeft)
	{
		mForce.x -= cos(or);
		mForce.z += sin(or);
	}

	mForce.normalise();
	mForce *= CAMERA_MOVE_SPEED;

	Vector3 vel = getVelocity();

	if (mForce.length() == 0) vel -= vel*time*CAMERA_DECEL; // slowing down
	else vel = mForce*time*CAMERA_ACCEL + vel*(1-time*CAMERA_ACCEL); // speeding up

	setVelocity(vel);
	mForeward = mBack = mLeft = mRight = false;

	// fall
	if (getVelY() > -CAMERA_FALL_MAX)
	{
		setVelY(getVelY() - CAMERA_GRAV*time);
	}

	// follow the landscape
	Vector3 pos = getPosition();
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

	Thing::move(time);

	mCamera->setPosition(getPosition());
}

//----------------------------------------------------------------------------

// this must not be destroyed!
void CameraThing::destroy()
{

}

//----------------------------------------------------------------------------

}