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
#include "OgrianAIWizardThing.h"
#include "OgrianRenderer.h"

template<> Ogrian::Input * Singleton< Ogrian::Input >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Input::Input()
{
	mTimeUntilNextToggle = -1;
	mTimeUntilNextCast = 0;
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
		Menu::getSingleton().show();
        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }

	// show the PlayerList
    if( input->isKeyDown( KC_TAB) && mTimeUntilNextToggle <= 0)
    {            
		PlayerList::getSingleton().toggle();
        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }

	// drop a manathing
    if (input->isKeyDown(KC_SPACE) && mTimeUntilNextToggle <= 0)
    {
		Physics::getSingleton().addThing(new ManaThing(1, 
			Renderer::getSingleton().getCameraThing()->getPosition()));
        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }

	// cast a fireball
	if (input->getMouseButton(0) && mTimeUntilNextCast <= 0)
	{
		Vector3 pos = Renderer::getSingleton().getCamera()->getPosition();
		Vector3 vel = Renderer::getSingleton().getCamera()->getDirection();
		vel.normalise();

		pos += vel*(CONR("WIZARD_SCALE") + CONR("FIREBALL_SCALE"))*1.1;
		vel *= CONR("FIREBALL_SPEED");
	
		Physics::getSingleton().addThing(new FireballThing(0, Physics::getSingleton().getTeam(0)->getColour(), pos,vel));
        mTimeUntilNextCast = CONR("FIREBALL_CAST_PERIOD");
	}

	// cast claim
	if (input->getMouseButton(1) && mTimeUntilNextCast <= 0)
	{
		Vector3 pos = Renderer::getSingleton().getCamera()->getPosition();
		Vector3 vel = Renderer::getSingleton().getCamera()->getDirection();
		vel.normalise();

		pos += vel*(CONR("WIZARD_SCALE") + CONR("CLAIMSPELL_SCALE"))*1.1;
		vel *= CONR("CLAIMSPELL_SPEED");
	
		Physics::getSingleton().addThing(new ClaimSpellThing(0, pos, vel));
        mTimeUntilNextCast = CONR("CLAIMSPELL_CAST_PERIOD");
	}

	return true;
}

//----------------------------------------------------------------------------

void Input::frame(Real time)
{
	mTimeUntilNextToggle -= time;
	mTimeUntilNextCast -= time;
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