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
OgrianMenu.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The Menu
/*------------------------------------*/

#include "OgrianMenu.h"
#include "OgrianRenderer.h"

#include "OgreOverlayManager.h"

#include "OgreCursorGuiElement.h"
#include "OgreBorderButtonGuiElement.h"
#include "OgreListGuiElement.h"
#include "OgreListChanger.h"
#include "OgreEventProcessor.h"
#include "OgreStringResource.h"

template<> Ogrian::Menu * Singleton< Ogrian::Menu >::ms_Singleton = 0;

namespace Ogrian
{

Menu::Menu()
{
	mActive = false;

	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("SS/Setup/HostScreen/Overlay");
	
	ListChanger* list = static_cast<ListGuiElement*>(GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/AvailableGamesList"));

	list->addListItem(new StringResource("1: CRESCENT"));
	list->addListItem(new StringResource("2: ISLANDS"));
}

Menu::~Menu()
{
	if (mActive) hideMenu();
}

bool Menu::processKeyInput(InputReader* input)
{
	// Q Quits
    if ( input->isKeyDown( KC_Q) )
    {            
        return false;
    }

	// ESC goes back to the game
    if ( input->isKeyDown( KC_ESCAPE)  && mTimeUntilNextToggle <= 0)
    {            
        hideMenu();
    }

	// Y toggles the inversion of the mouse Y axis
    if ( input->isKeyDown( KC_Y)  && mTimeUntilNextToggle <= 0)
    {            
		OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();

		if (ofl->getInvertY())  // uninvert the mouse y axis
		{
			GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Yinvert")
				->setParameter("caption", "SS/Templates/BasicText y: INV (OFF)");

			ofl->setInvertY(false);
		}
		else // invert the mouse y axis
		{
			GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Yinvert")
				->setParameter("caption", "SS/Templates/BasicText y: INV (ON)");

			ofl->setInvertY(true);
		}

        mTimeUntilNextToggle = KEY_DELAY;
    }

	// 1 loads crescent
    if ( input->isKeyDown( KC_1)  && mTimeUntilNextToggle <= 0)
    {
		// Loading...
		GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Text")
			->setParameter("caption", "LOADING...");

		// load the map
		//Renderer::getSingleton().loadMap("crescent.cfg");
		
		// Menu
		//GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Text")
			//->setParameter("caption", "MENU");

		// hide the menu
		hideMenu();
	}

	// 2 loads islands
    if ( input->isKeyDown( KC_2)  && mTimeUntilNextToggle <= 0)
    {
		// Loading...
		GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Text")
			->setParameter("caption", "LOADING...");

		// load the map
		Renderer::getSingleton().loadMap("islands.cfg");
		
		// Menu
		GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Text")
			->setParameter("caption", "MENU");
		
		// hide the menu
		hideMenu();
	}

	return true;
}

void Menu::frame(Real time)
{
	// decrement mTimeUntilNextToggle
	if (mTimeUntilNextToggle >= 0) 
		mTimeUntilNextToggle -= time;
}

void Menu::showMenu()
{
	if (mActive == true) return;

	mOverlay->show();

	// to prevent immediately going back to the game
    mTimeUntilNextToggle = KEY_DELAY;

	mActive = true;
}

void Menu::hideMenu()
{
	if (mOverlay == false) return;

	mOverlay->hide();

	// to prevent immediately going back into the menu
	Input::getSingleton().delay(KEY_DELAY);

	mActive = false;
}

bool Menu::isActive()
{
	return mActive;
}

Menu& Menu::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Menu();
	}
	return Singleton<Menu>::getSingleton();
}

}