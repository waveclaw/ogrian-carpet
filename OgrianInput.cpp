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

Description: Handle game input, as opposed to menu input.
/*------------------------------------*/

#include "OgrianInput.h"
#include "OgrianPhysics.h"
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

template<> Ogrian::Input * Singleton< Ogrian::Input >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Input::Input()
{
	mTimeUntilNextCast = 0;
	
	SpellManager::getSingleton().readyDefaultSpell();
}

//----------------------------------------------------------------------------

Input::~Input()
{

}


//----------------------------------------------------------------------------

bool Input::processKeyInput(InputReader* input)
{
	// check each keycode
	for (int i=0; i<NUM_KEYS; i++)
	{
		// when a key is pressed
		if (!mKeys[i] && input->isKeyDown((Ogre::KeyCode)i))
			keyPressed(i);

		// update keys
		mKeys[i] = input->isKeyDown((Ogre::KeyCode)i);
	}

	// show the PlayerList
    if( input->isKeyDown( KC_TAB ))
    {            
		PlayerList::getSingleton().show();
    }
	else PlayerList::getSingleton().hide();

	// cast the selected spell
	if (input->getMouseButton(0) && mTimeUntilNextCast <= 0 && !Renderer::getSingleton().getCameraThing()->isGhost())
	{
        mTimeUntilNextCast = SpellManager::getSingleton().castSpell();
	}

	return true;
}

//----------------------------------------------------------------------------

void Input::keyPressed(int key)
{
	// show the menu
    if(key == KC_ESCAPE)
    {            
		Menu::getSingleton().show();
    }

	// select the next spell
	if (key == KC_E)
	{
		SpellManager::getSingleton().readyNextSpell();
	}

	// select the prev spell
	if (key == KC_Q)
	{
		SpellManager::getSingleton().readyPrevSpell();
	}
}

//----------------------------------------------------------------------------

void Input::frame(Real time)
{
	mTimeUntilNextCast -= time;
}

//----------------------------------------------------------------------------

void Input::delay(Real time)
{

}

//----------------------------------------------------------------------------

Input& Input::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Input();
	}
	return Singleton<Input>::getSingleton();
}

//----------------------------------------------------------------------------

}