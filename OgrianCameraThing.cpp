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

 *------------------------------------*/

#include "OgrianRenderer.h"
#include "OgrianCameraThing.h"
#include "OgrianHud.h"
#include "OgrianPhysics.h"
#include "OgrianCastle.h"

#include "OgreConfigFile.h"

namespace Ogrian
{

//----------------------------------------------------------------------------

CameraThing::CameraThing(Camera* camera) : WizardThing(false)
{
	mCamera = camera;
	mTempCam = new Camera("TempCam", 0);

	// set the sensitivity
	mSensitivity = CONR("MOUSE_SENSITIVITY");
	if (mSensitivity == 0) mSensitivity = 1;

	// set the color and the skin, BTW this changes as
	// there was a type mismatch for this function.
	// &a != a no matter what M$ says, kids -- jdpowell 20060612
	Ogrian::WizardThing::setColour(Menu::getSingleton().getChosenColour());
	Ogrian::WizardThing::setSkin(Menu::getSingleton().getChosenWizardSkin());
}

//----------------------------------------------------------------------------

CameraThing::~CameraThing()
{
	delete mTempCam;
}

//----------------------------------------------------------------------------

void CameraThing::reset()
{
	WizardThing::reset();
	// set the color and the skin, BTW this changes as
	// there was a type mismatch for this function.
	// &a != a no matter what M$ says, kids -- jdpowell 20060612	
	WizardThing::setColour(Menu::getSingleton().getChosenColour());
	WizardThing::setSkin(Menu::getSingleton().getChosenWizardSkin());
}

//----------------------------------------------------------------------------

void CameraThing::makeGhost()
{
	WizardThing::makeGhost();

	Hud::getSingleton().makeGhost();
}

//----------------------------------------------------------------------------

Vector3 CameraThing::getDirection()
{
	return mCamera->getDerivedDirection();	
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
	WizardThing::move(time);

	mCamera->setPosition(getPosition());
}

//----------------------------------------------------------------------------

void CameraThing::setHealth(int health)
{
	if (isGhost()) health = 0;

	WizardThing::setHealth(health);
}

//----------------------------------------------------------------------------

void CameraThing::die()
{
	WizardThing::die();
	
	if (!Multiplayer::getSingleton().isClient())
	{
		Castle* castle = 0;
		if (getTeam()) castle = getTeam()->getCastle();

		if (castle)
		{
			setPosition(castle->getPosition());
		}
		else
		{
			// respawn at a random location
			Vector3 pos = Vector3(0,0,0);
			Real size = HeightMap::getSingleton().getWorldSize();
			while (pos.y <= CONR("BUILDING_MIN_GROUNDY"))
			{
				pos.x = Math::RangeRandom(0, size);
				pos.z = Math::RangeRandom(0, size);
				pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z) + getHeight();
			}
			pos.y = 0;
			setPosition(pos);
		}
	}
}

//----------------------------------------------------------------------------

}
