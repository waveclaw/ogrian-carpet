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
OgrianInput.h
Original Author: Mike Prosser
Additional Authors: 

Description: Handle game input
 *------------------------------------*/

#ifndef __OgrianInput_H__
#define __OgrianInput_H__

#include <Ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>

#include "OgrianPhysics.h"
#include "OgrianHud.h"
#include "OgrianAudio.h"
#include "OgrianMenu.h"
#include "OgrianPlayerList.h"
#include "OgrianManaThing.h"
#include "OgrianFireballThing.h"
#include "OgrianClaimSpellThing.h"
#include "OgrianBuildSpellThing.h"
#include "OgrianAIWizardThing.h"
#include "OgrianRenderer.h"
#include "OgrianSpellManager.h"

using namespace Ogre;

namespace Ogrian
{

#define NUM_KEYS 0xED

class Input : public Singleton< Input >
{
public:
	virtual ~Input();

	// this is called by OgrianFrameListener when not in menu mode
	virtual bool processKeyInput(InputReader* input);

	// ignore keypresses for this amount of time
	virtual void delay(Real time);

	// this should be called every frame
	virtual void frame(Real time);

	static Input& getSingleton(void);

private:
	Input();
    Real mTimeUntilNextCast; // limits casting speed

	bool mKeys[NUM_KEYS]; // tracks which keys are down

	void keyPressed(int key);
};

}

#endif
