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

#include "OgrianRenderer.h"
#include "OgrianCameraThing.h"
#include "OgrianHud.h"

#include "OgreConfigFile.h"

namespace Ogrian
{

//----------------------------------------------------------------------------

CameraThing::CameraThing(Camera* camera) : WizardThing(false)
{
	mCamera = camera;
	mTempCam = new Camera("TempCam", 0);
	mForeward = mBack = mLeft = mRight = false;

	// set the sensitivity
	ConfigFile config;
	config.load( "config.cfg" );
	mSensitivity = atof(config.getSetting( "sensitivity" ).c_str());
	if (mSensitivity == 0) mSensitivity = 1;

	// set the color
	ColourValue colour;
	colour.r = atof(config.getSetting( "red" ).c_str());
	colour.b = atof(config.getSetting( "blue" ).c_str());
	colour.g = atof(config.getSetting( "green" ).c_str());
	setColour(colour);

	int skin = atoi(config.getSetting( "skin" ).c_str());
	setSkin(skin);
}

//----------------------------------------------------------------------------

CameraThing::~CameraThing()
{
	delete mTempCam;
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
	// multiply by the sensitivity
	rotX *= mSensitivity;
	rotY *= mSensitivity;

	// set the yaw
	mCamera->yaw(Degree(rotX));

	// constrain the pitch
	Vector3 dir = mCamera->getDirection();

	mTempCam->setDirection(Vector3(0,0,1));
	mTempCam->setDirection(dir);
	mTempCam->pitch(Degree(rotY));
	dir = mTempCam->getDirection();

	if (dir.y < CONR("CAMERA_PITCH_MAX") && dir.y > -CONR("CAMERA_PITCH_MAX"))
		mCamera->setDirection(dir);

	// set the orientation of the thing to match the camera
	setOrientation(atan2(dir.x, dir.z));
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
	mForce *= CONR("CAMERA_MOVE_SPEED");

	Vector3 vel = getVelocity();

	if (mForce.length() == 0) vel -= vel*time*CONR("CAMERA_DECEL"); // slowing down
	else vel = mForce*time*CONR("CAMERA_ACCEL") + vel*(1-time*CONR("CAMERA_ACCEL")); // speeding up

	setVelocity(vel);
	mForeward = mBack = mLeft = mRight = false;

	WizardThing::move(time);

	mCamera->setPosition(getPosition());
}

//----------------------------------------------------------------------------

void CameraThing::setHealth(int health)
{
	WizardThing::setHealth(health);

	Hud::getSingleton().setHealth(health);
}

//----------------------------------------------------------------------------

void CameraThing::die()
{
	WizardThing::die();

	Vector3 offset;
	Real wdo = CONR("WIZARD_DEATH_OFFSET");
	offset.x = Math::RangeRandom(-wdo, wdo);
	offset.z = Math::RangeRandom(-wdo, wdo);
	setPosition(getPosition() + offset);
}

//----------------------------------------------------------------------------

}