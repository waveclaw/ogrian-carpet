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
NOTE: to add buttons and stuff, be sure to edit OgrianMouseFrameListener.h
/*------------------------------------*/

#include "OgrianMenu.h"
#include "OgrianPlayerList.h"
#include "OgrianRenderer.h"
#include "OgrianMultiplayer.h"

#include "OgreOverlayManager.h"

#include "OgreCursorGuiElement.h"
#include "OgreBorderButtonGuiElement.h"
#include "OgreEventProcessor.h"
#include "OgreStringResource.h"

template<> Ogrian::Menu * Singleton< Ogrian::Menu >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Menu::Menu()
{
	mActive = false;
	mLoadMap = false;

	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/Menu/Overlay");
	
	// set up the cursor
	mCursor = OverlayManager::getSingleton().getCursorGui();
    mCursor->setMaterialName("Cursor/default");
    mCursor->setDimensions(32.0/640.0, 32.0/480.0);

	// hide the disconnect button at the start
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Disconnect")->hide();

	// build the list
	mList = static_cast<ListGuiElement*>(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/AvailableMapsList"));

	mList->addListItem(new StringResource("crescent"));
	mList->addListItem(new StringResource("islands"));
}

//----------------------------------------------------------------------------

Menu::~Menu()
{
	if (mActive) hide();
}

//----------------------------------------------------------------------------
void Menu::button_invertMouseToggle()
{
	setMessage("toggled");

	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();

	if (ofl->getInvertY())  // uninvert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText INV MOUSE (OFF)");

		ofl->setInvertY(false);
	}
	else // invert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText INV MOUSE (ON)");

		ofl->setInvertY(true);
	}
}
//----------------------------------------------------------------------------
void Menu::button_load()
{
	loadMap( static_cast<StringResource*>(mList->getSelectedItem())->getName() + ".cfg" );
}
//----------------------------------------------------------------------------
void Menu::button_join()
{
	setMessage("Joining Server");
	Multiplayer::getSingleton().clientStart();
	
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Host")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Join")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Load")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Disconnect")->show();
}

//----------------------------------------------------------------------------
void Menu::button_host()
{
	loadMap( static_cast<StringResource*>(mList->getSelectedItem())->getName() + ".cfg" );

	setMessage("Starting Server...");
	Multiplayer::getSingleton().serverStart();

	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Host")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Join")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Load")->hide();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Disconnect")->show();
}

//----------------------------------------------------------------------------
void Menu::button_disconnect()
{
	setMessage("Disconnecting");
	if (Multiplayer::getSingleton().isClient())	Multiplayer::getSingleton().clientDisconnect();
	else if (Multiplayer::getSingleton().isServer()) Multiplayer::getSingleton().serverDisconnect();

	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Host")->show();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Join")->show();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Load")->show();
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Disconnect")->hide();
}

//----------------------------------------------------------------------------

void Menu::setMessage(String message)
{
	GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Text")
		->setParameter("caption", message);
}

//----------------------------------------------------------------------------

bool Menu::processKeyInput(InputReader* input)
{
	// Q Quits //////////////////////////////
    if ( input->isKeyDown( KC_Q) )
    {            
        return false;
    }

	// Tab shows players //////////////////////////////
    if ( input->isKeyDown( KC_TAB) && mTimeUntilNextToggle <= 0)
    {           
		PlayerList::getSingleton().toggle();
        mTimeUntilNextToggle = KEY_DELAY; 
    }

	// ESC goes back to the game //////////////////////////////
    if ( input->isKeyDown( KC_ESCAPE) && mTimeUntilNextToggle <= 0)
    {            
        hide();
    }

	// Y toggles the inversion of the mouse Y axis //////////////////////////////
    if ( input->isKeyDown( KC_Y) && mTimeUntilNextToggle <= 0)
    {            
		button_invertMouseToggle();
        mTimeUntilNextToggle = KEY_DELAY;
    }

	return true;
}

//----------------------------------------------------------------------------

void Menu::loadMap(String mapname)
{
	// Loading...
	setMessage("Loading...");

	// move the camera so the terrain will reload properly
	Renderer::getSingleton().getCamera()->setPosition(-100000,0,-100000);

	// stop the game
	Audio::getSingleton().stop();
	Renderer::getSingleton().getFrameListener()->setGameRunning(false);

	// load the map
	mMapName = mapname;
	mLoadMap = true;
}

//----------------------------------------------------------------------------

void Menu::frame(Real time)
{
	// decrement mTimeUntilNextToggle
	if (mTimeUntilNextToggle >= 0) 
		mTimeUntilNextToggle -= time;

	// load a map if theres one ready
	if (mLoadMap)
	{
		// load the map
		Renderer::getSingleton().loadMap(mMapName);

		// Menu
		setMessage("Menu");
		
		// hide the menu
		hide();

		mLoadMap = false;
	}
}

//----------------------------------------------------------------------------

void Menu::show()
{
	if (mActive == true) return;

	mOverlay->show();

	// to prevent immediately going back to the game
    mTimeUntilNextToggle = KEY_DELAY;

	// show cursor
    mCursor->show();

	// freeze the camera
	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();
	if (ofl != 0) ofl->setCameraFrozen(true);

	mActive = true;
}

//----------------------------------------------------------------------------

void Menu::hide()
{
	if (mOverlay == false) return;

	mOverlay->hide();

	// to prevent immediately going back into the menu
	Input::getSingleton().delay(KEY_DELAY);

	// hide cursor
    mCursor->hide();

	// unfreeze the camera
	Renderer::getSingleton().getFrameListener()->setCameraFrozen(false);
	mActive = false;
}

//----------------------------------------------------------------------------

bool Menu::isActive()
{
	return mActive;
}

//----------------------------------------------------------------------------

Menu& Menu::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Menu();
	}
	return Singleton<Menu>::getSingleton();
}

}