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
OgrianWizardThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: The wizard thing is the superclass of the CameraThing

/*------------------------------------*/


#ifndef __OgrianWizardThing_H__
#define __OgrianWizardThing_H__

#include <Ogre.h>
#include "OgrianConstants.h"
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

// extend floatingThing for floating behavior
class WizardThing : public Thing
{
public:
	WizardThing(bool visible=true) 
		: Thing("Ogrian/Clear", visible?ORIENTEDSPRITE:SPRITE, "WizardThing", true, CAMERA_HEIGHT)
	{
		if (visible)
		{
			getVisRep()->addPose("Ogrian/Wizard/");
			getVisRep()->setPose(0);
		}
	}

	virtual ThingType getType()
	{
		return WIZARDTHING;
	}

	virtual void move(Real time)
	{
		Vector3 lastPos = getPosition();

		Thing::move(time);

		Vector3 pos = getPosition();

		// generate a velocity to use for multiplayer interpolation
		mInterpVel = (pos - lastPos) / time;
	}

	virtual void generateBitStream(BitStream& bitstream)
	{
		Vector3 originalVel = getVelocity();

		// give the bitstream the inerpolation velocity
		setVelocity(mInterpVel);
		Thing::generateBitStream(bitstream);

		// clean up
		setVelocity(originalVel);
	}

	virtual void destroy()
	{
		// DONT
	}

private:
	Vector3 mInterpVel;
};

}

#endif