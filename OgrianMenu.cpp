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
#include "OgrianAIWizardThing.h"
#include "OgrianPhysics.h"
#include "OgrianSpellManager.h"

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
	mMusic = true;

	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/Menu/Overlay");

	// load the panels
	mMapListPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/MapListPanel");
	mColourListPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/ColourListPanel");
	mWizardSkinListPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/WizardSkinListPanel");
	mCastleSkinListPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/CastleSkinListPanel");

	mMainMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/MainMenuPanel");
	mSkirmishMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/SkirmishMenuPanel");
	mHostMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/HostMenuPanel");
	mJoinMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/JoinMenuPanel");
	mConfigMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/ConfigMenuPanel");
	mConnectionMenuPanel = GuiManager::getSingleton().getGuiElement("Ogrian/Menu/ConnectionMenuPanel");

	mMapListPanel->hide();
	mColourListPanel->hide();
	mWizardSkinListPanel->hide();
	mCastleSkinListPanel->hide();

	mMainMenuPanel->show();
	mSkirmishMenuPanel->hide();
	mHostMenuPanel->hide();
	mJoinMenuPanel->hide();
	mConfigMenuPanel->hide();
	mConnectionMenuPanel->hide();

	// set up the cursor
	mCursor = OverlayManager::getSingleton().getCursorGui();
    mCursor->setMaterialName("Cursor/default");
    mCursor->setDimensions(32.0/640.0, 32.0/480.0);

	// build the map list
	mMapList = static_cast<ListGuiElement*>(GuiManager::getSingleton().
		getGuiElement("Ogrian/Menu/MapList"));
	loadMapList();
	
	// build the colour list
	mColourList = static_cast<ListGuiElement*>(GuiManager::getSingleton().
		getGuiElement("Ogrian/Menu/ColourList"));
	loadColourList();
	
	// build the wizard skin list
	mWizardSkinList = static_cast<ListGuiElement*>(GuiManager::getSingleton().
		getGuiElement("Ogrian/Menu/WizardSkinList"));
	loadWizardSkinList();
	
	// build the wizard skin list
	mCastleSkinList = static_cast<ListGuiElement*>(GuiManager::getSingleton().
		getGuiElement("Ogrian/Menu/CastleSkinList"));
	loadCastleSkinList();

	// set the title
	setMessage(CONS("TITLE"));
}

//----------------------------------------------------------------------------

Menu::~Menu()
{
	if (mActive) hide();
}

//----------------------------------------------------------------------------

void Menu::loadMapList()
{
	String filename = "maps.txt";

	FILE *fp;
	char rec[100], *ret;
	String optName, optVal;

	// Open and parse entire file
	fp = fopen(filename.c_str(), "r");
	if( !fp )
		Except(
			Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!", "Menu::loadMapList" );

	ret = fgets(rec, 100, fp);
	while (ret != NULL)
	{
		String tst = rec;
		StringUtil::trim(tst);
		// Ignore comments & blanks
		if (tst.length() > 0 && tst.at(0) != '#' && tst.at(0) != '@' && tst.at(0) != '\n')
		{
			// Tokenise on newline
			char* pName = strtok(rec, "\n");
			if (pName)
			{
				String optName = pName;
				StringUtil::trim(optName);

				mMapList->addListItem(new StringResource(optName));
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

void Menu::loadWizardSkinList()
{
	String filename = "wizardskins.txt";

	FILE *fp;
	char rec[100], *ret;
	String optName, optVal;

	// Open and parse entire file
	fp = fopen(filename.c_str(), "r");
	if( !fp )
		Except(
			Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!", "Menu::loadWizardSkinList" );

	ret = fgets(rec, 100, fp);
	while (ret != NULL)
	{
		String tst = rec;
		StringUtil::trim(tst);
		// Ignore comments & blanks
		if (tst.length() > 0 && tst.at(0) != '#' && tst.at(0) != '@' && tst.at(0) != '\n')
		{
			// Tokenise on newline
			char* pName = strtok(rec, "\n");
			if (pName)
			{
				String optName = pName;
				StringUtil::trim(optName);

				mWizardSkinList->addListItem(new StringResource(optName));
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

void Menu::loadCastleSkinList()
{
	String filename = "castleskins.txt";

	FILE *fp;
	char rec[100], *ret;
	String optName, optVal;

	// Open and parse entire file
	fp = fopen(filename.c_str(), "r");
	if( !fp )
		Except(
			Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!", "Menu::loadCastleSkinList" );

	ret = fgets(rec, 100, fp);
	while (ret != NULL)
	{
		String tst = rec;
		StringUtil::trim(tst);
		// Ignore comments & blanks
		if (tst.length() > 0 && tst.at(0) != '#' && tst.at(0) != '@' && tst.at(0) != '\n')
		{
			// Tokenise on newline
			char* pName = strtok(rec, "\n");
			if (pName)
			{
				String optName = pName;
				StringUtil::trim(optName);

				mCastleSkinList->addListItem(new StringResource(optName));
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

void Menu::loadColourList()
{
	mColourList->addListItem(new StringResource("Black"));
	mColourList->addListItem(new StringResource("Red"));
	mColourList->addListItem(new StringResource("Green"));
	mColourList->addListItem(new StringResource("Blue"));
	mColourList->addListItem(new StringResource("Cyan"));
	mColourList->addListItem(new StringResource("Purple"));
}

//----------------------------------------------------------------------------

void Menu::button_invertMouseToggle()
{
	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();

	if (ofl->getInvertY())  // uninvert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText INV MOUSE (OFF)");

		ofl->setInvertY(false);
	}
	else // invert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText INV MOUSE (ON)");

		ofl->setInvertY(true);
	}
}
//----------------------------------------------------------------------------
void Menu::button_musicToggle()
{
	if (mMusic)  // deactivate the music
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Music")
			->setParameter("caption", "SS/Templates/BasicText MUSIC (OFF)");

		Audio::getSingleton().stopSong();
		mMusic = false;
	}
	else // activate the music
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Music")
			->setParameter("caption", "SS/Templates/BasicText MUSIC (ON)");

		Audio::getSingleton().playSong(CONS("THEME_MUSIC"));
		mMusic = true;
	}
}
//----------------------------------------------------------------------------

void Menu::button_quit()
{
	// hide the menu
	hide();
}

//----------------------------------------------------------------------------

void Menu::button_back()
{
	mMapListPanel->hide();
	mColourListPanel->hide();
	mWizardSkinListPanel->hide();
	mCastleSkinListPanel->hide();

	mMainMenuPanel->show();
	mSkirmishMenuPanel->hide();
	mHostMenuPanel->hide();
	mJoinMenuPanel->hide();
	mConfigMenuPanel->hide();
	mConnectionMenuPanel->hide();
}

//----------------------------------------------------------------------------

void Menu::button_config() 
{
	mMainMenuPanel->hide();
	mConfigMenuPanel->show();
	mColourListPanel->show();
	mWizardSkinListPanel->show();
	mCastleSkinListPanel->show();
}

//----------------------------------------------------------------------------

void Menu::button_skirmish() 
{
	mMainMenuPanel->hide();
	mSkirmishMenuPanel->show();
	mMapListPanel->show();
}

//----------------------------------------------------------------------------

void Menu::button_skirmishload() 
{
	if (Multiplayer::getSingleton().isServer()) Multiplayer::getSingleton().serverDisconnect();
	if (Multiplayer::getSingleton().isClient()) Multiplayer::getSingleton().clientDisconnect();

	loadMap(static_cast<StringResource*>(mMapList->getSelectedItem())->getName());
}

//----------------------------------------------------------------------------

void Menu::button_hostload() 
{
	if (Multiplayer::getSingleton().isServer()) Multiplayer::getSingleton().serverDisconnect();
	if (Multiplayer::getSingleton().isClient()) Multiplayer::getSingleton().clientDisconnect();

	loadMap(static_cast<StringResource*>(mMapList->getSelectedItem())->getName());

	setMessage(CONS("MSG_SERVER_LOADING"));

	Multiplayer::getSingleton().serverStart();

	button_back();
	mMainMenuPanel->hide();
	mConnectionMenuPanel->show();
}

//----------------------------------------------------------------------------

void Menu::button_join()
{
	mMainMenuPanel->hide();
	mJoinMenuPanel->show();

	GuiManager::getSingleton().getGuiElement("Ogrian/ConnectionMenu/StartGame")->hide();
}

//----------------------------------------------------------------------------

void Menu::button_joinload()
{
	setMessage(CONS("MSG_JOIN_CONNECTING"));

	Multiplayer::getSingleton().clientStart();
	
	button_back();
	mMainMenuPanel->hide();
	mConnectionMenuPanel->show();
}

//----------------------------------------------------------------------------

void Menu::button_host()
{
	mMainMenuPanel->hide();
	mHostMenuPanel->show();
	mMapListPanel->show();
	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConnectionMenu/StartGame")->show();
}

//----------------------------------------------------------------------------

void Menu::button_disconnect()
{
	setMessage(CONS("MSG_DISCONNECT"));
	if (Multiplayer::getSingleton().isClient())	Multiplayer::getSingleton().clientRequestKick();
	else if (Multiplayer::getSingleton().isServer()) Multiplayer::getSingleton().serverDisconnect();

	button_back();
}

//----------------------------------------------------------------------------

void Menu::button_startGame()
{
	LogManager::getSingleton().logMessage(CONS("MSG_START_GAME"));
	GuiManager::getSingleton().getGuiElement("Ogrian/ConnectionMenu/StartGame")->hide();
	
	// hide the menu
	hide();

	// end the pregame - start the real game
	if (Multiplayer::getSingleton().isServer())
		Game::getSingleton().serverEndPreGame();
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
	// Tab shows players //////////////////////////////
    if( input->isKeyDown( KC_TAB) && mTimeUntilNextToggle <= 0)
    {            
		PlayerList::getSingleton().show();
    }
	else PlayerList::getSingleton().hide();

	// ESC goes back to the game //////////////////////////////
    if ( input->isKeyDown( KC_ESCAPE) && mTimeUntilNextToggle <= 0)
    {            
        hide();
    }

	return true;
}

//----------------------------------------------------------------------------

void Menu::loadMap(String mapname)
{
	// Loading...
	setMessage(CONS("MSG_LOADMAP"));

	// move the camera so the terrain will reload properly
	//Renderer::getSingleton().getCamera()->setPosition(-100000,0,-100000);

	// stop the game
	Audio::getSingleton().stop();
	Renderer::getSingleton().getFrameListener()->setGameRunning(false);

	// load the map
	mMapName = String("Media/maps/") + mapname + ".txt";
	mLoadMap = true;
	mLoadDelay = CONI("LOAD_DELAY");
}

//----------------------------------------------------------------------------

void Menu::frame(Real time)
{
	// decrement mTimeUntilNextToggle
	if (mTimeUntilNextToggle >= 0) 
		mTimeUntilNextToggle -= time;

	// decrement mLoadDelay
	if (mLoadMap)
		mLoadDelay--;

	// load a map if theres one ready
	if (mLoadMap && mLoadDelay == 0)
	{
		// load the map
		Renderer::getSingleton().loadMap(mMapName);

		// Menu
		setMessage(CONS("TITLE"));
		
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
    mTimeUntilNextToggle = CONR("KEY_DELAY");

	// show cursor
    mCursor->show();

	// freeze the camera
	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();
	if (ofl != 0) ofl->setCameraFrozen(true);

	// play menu music
	if (mMusic)
		Audio::getSingleton().playSong(CONS("THEME_MUSIC"));

	mActive = true;
}

//----------------------------------------------------------------------------

void Menu::hide()
{
	if (mOverlay == false) return;

	mOverlay->hide();

	// to prevent immediately going back into the menu
	Input::getSingleton().delay(CONR("KEY_DELAY"));

	// hide cursor
    mCursor->hide();

	// unfreeze the camera
	Renderer::getSingleton().getFrameListener()->setCameraFrozen(false);
	
	// play game music
	if (mMusic)
		Audio::getSingleton().playSong(Game::getSingleton().getMapMusic());

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