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
#include "OgrianManaThing.h"
#include "OgrianRenderer.h"

template<> Ogrian::Input * Singleton< Ogrian::Input >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Input::Input()
{

}

//----------------------------------------------------------------------------

Input::~Input()
{

}


//----------------------------------------------------------------------------

bool Input::processKeyInput(InputReader* input)
{
	// show the menu
    if( input->isKeyDown( KC_ESCAPE) && mTimeUntilNextToggle <= 0)
    {            
		Menu::getSingleton().showMenu();
        mTimeUntilNextToggle = KEY_DELAY;
    }

	// drop a manathing
    if (input->isKeyDown(KC_SPACE) && mTimeUntilNextToggle <= 0)
    {
		Physics::getSingleton().addThing(new ManaThing(1, 
			Renderer::getSingleton().getCameraThing()->getPosition()));
        mTimeUntilNextToggle = KEY_DELAY;
    }

	// play song 1
    if (input->isKeyDown(KC_N) && mTimeUntilNextToggle <= 0)
    {
		Audio::getSingleton().playSong("OgrianMedia/music/Bulerias.ogg");
        mTimeUntilNextToggle = KEY_DELAY;
    }
	
	// play song 2
    if (input->isKeyDown(KC_M) && mTimeUntilNextToggle <= 0)
    {
		Audio::getSingleton().playSong("OgrianMedia/music/Verdiales.ogg");
        mTimeUntilNextToggle = KEY_DELAY;
    }

	return true;
}

//----------------------------------------------------------------------------

void Input::frame(Real time)
{
	if (mTimeUntilNextToggle >= 0) 
		mTimeUntilNextToggle -= time;
}

//----------------------------------------------------------------------------

void Input::delay(Real time)
{
	mTimeUntilNextToggle = time;
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