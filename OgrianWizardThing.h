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
	WizardThing(String material="Ogrian/Mana") 
		: Thing(material, "WizardThing", true, CAMERA_HEIGHT)
	{
	}


private:
};

}

#endif