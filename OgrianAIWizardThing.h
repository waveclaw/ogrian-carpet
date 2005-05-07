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
OgrianAIWizardThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: A computer controlled Wizard

/*------------------------------------*/


#ifndef __OgrianAIWizardThing_H__
#define __OgrianAIWizardThing_H__

#include <Ogre.h>
#include "OgrianWizardThing.h"

using namespace Ogre;

namespace Ogrian
{

class AIWizardThing : public WizardThing
{
public:
	AIWizardThing(Vector3 pos, int skin, int teamNum);

	// think
	virtual void think();

	virtual void die();

private:
	void think_moveTo(Thing* target);
	void think_attack(Thing* target);
	void think_circleStrafe(Thing* target);
	void think_faceTarget(Thing* target);
};

}

#endif