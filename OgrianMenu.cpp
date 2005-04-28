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
#include "OgrianSkinManager.h"

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
	mTextCursorFlash = false;
	mNextTextCursorFlashTime = 0;

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

	mLoadingOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/Menu/LoadingOverlay");

	mMapListPanel->hide();
	mColourListPanel->hide();
	mWizardSkinListPanel->hide();
	mCastleSkinListPanel->hide();

	mNameText = GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Name");
	mServerText = GuiManager::getSingleton().getGuiElement("Ogrian/JoinMenu/Sever");

	mMainMenuPanel->show();
	mSkirmishMenuPanel->hide();
	mHostMenuPanel->hide();
	mJoinMenuPanel->hide();
	mConfigMenuPanel->hide();
	mConnectionMenuPanel->hide();

	mLoadingOverlay->hide();

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

void Menu::readConfig()
{
	ConfigFile config;
	config.load("ogrian.cfg");

	mConfigName = config.getSetting("name");
	mConfigServer = config.getSetting("server");
	mConfigColour = config.getSetting("colour");

	mConfigWizardSkin = atoi(config.getSetting("wizard_skin").c_str());
	mConfigCastleSkin = atoi(config.getSetting("castle_skin").c_str());

	mConfigMusicVolume = atoi(config.getSetting("music_volume").c_str());
	mConfigYInvert = atoi(config.getSetting("y_invert").c_str());

	// apply the settings // 

	// set the y invert
	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();
	if (ofl) ofl->setInvertY(mConfigYInvert > 0);

	if (mConfigYInvert == 0) 
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText   ");
	else 
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText  X");

	// set music volume
	if (mConfigMusicVolume == 0)
		button_musicOff();
	else if (mConfigMusicVolume <= 64)
		button_musicSoft();
	else if (mConfigMusicVolume <= 128)
		button_musicMed();
	else
		button_musicLoud();

	// select our skins
	String wizardSkinName = SkinManager::getSingleton().getRawWizardSkin(mConfigWizardSkin);
	String castleSkinName = SkinManager::getSingleton().getRawCastleSkin(mConfigCastleSkin);

	mWizardSkinList->setSelectedItem(&StringResource(wizardSkinName));
	mCastleSkinList->setSelectedItem(&StringResource(castleSkinName));

	// select our colour
	if (mConfigColour.length() > 0)
		mColourList->setSelectedItem(&StringResource(mConfigColour));

	// set the text for name and server
	mNameText->setCaption(mConfigName);
	mServerText->setCaption(mConfigServer);
}

//----------------------------------------------------------------------------

// based on Ogre::Root::saveConfig()
void Menu::writeConfig()
{
	String filename = "ogrian.cfg";
	FILE *fp = 0;
	char rec[100];

	fp = fopen(filename.c_str(), "w");
	if (!fp)
		Except(Exception::ERR_CANNOT_WRITE_TO_FILE, "Cannot create settings file.",
		"Menu::writeConfig");

	// write the values
	sprintf(rec, "%s\t%s\n", "name", mConfigName.c_str());
	fputs(rec, fp);

	sprintf(rec, "%s\t%s\n", "server", mConfigServer.c_str());
	fputs(rec, fp);

	sprintf(rec, "%s\t%s\n", "colour", mConfigColour.c_str());
	fputs(rec, fp);


	sprintf(rec, "%s\t%d\n", "wizard_skin", mConfigWizardSkin);
	fputs(rec, fp);

	sprintf(rec, "%s\t%d\n", "castle_skin", mConfigCastleSkin);
	fputs(rec, fp);

	sprintf(rec, "%s\t%d\n", "music_volume", mConfigMusicVolume);
	fputs(rec, fp);

	sprintf(rec, "%s\t%d\n", "y_invert", mConfigYInvert);
	fputs(rec, fp);

	fclose(fp);
}

//----------------------------------------------------------------------------

String Menu::getChosenName() { return mConfigName; }

String Menu::getChosenServer() { return mConfigServer; }

int Menu::getChosenWizardSkin() { return mConfigWizardSkin; }

int Menu::getChosenCastleSkin() { return mConfigCastleSkin; }

//----------------------------------------------------------------------------

ColourValue Menu::getChosenColour() 
{ 
		 if (mConfigColour == "Black")		return ColourValue(.1, .1, .1);	// Black
	else if (mConfigColour == "Blue")		return ColourValue( 0,  0,  1);	// Blue
	else if (mConfigColour == "Brown")		return ColourValue(.5,.25,  0);	// Brown
	else if (mConfigColour == "Cyan")		return ColourValue( 0,  1,  1);	// Cyan
	else if (mConfigColour == "Dark Blue")	return ColourValue( 0,  0, .5);	// Dark Blue
	else if (mConfigColour == "Dark Green")	return ColourValue( 0, .5,  0);	// Dark Green
	else if (mConfigColour == "Dark Red")	return ColourValue(.5,  0,  0);	// Dark Red
	else if (mConfigColour == "Green")		return ColourValue( 0,  1,  0);	// Green
	else if (mConfigColour == "Orange")		return ColourValue( 1, .5,.25);	// Orange
	else if (mConfigColour == "Purple")		return ColourValue( 1,  0,  1);	// Purple
	else if (mConfigColour == "Red")		return ColourValue( 1,  0,  0);	// Red
	else if (mConfigColour == "Turquoise")	return ColourValue( 0,  1, .5);	// Turquoise
	else if (mConfigColour == "Yellow")		return ColourValue( 1,  1,  0);	// Yellow

	else return ColourValue::Blue;
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
	mColourList->addListItem(new StringResource("Blue"));
	mColourList->addListItem(new StringResource("Brown"));
	mColourList->addListItem(new StringResource("Cyan"));
	mColourList->addListItem(new StringResource("Dark Blue"));
	mColourList->addListItem(new StringResource("Dark Green"));
	mColourList->addListItem(new StringResource("Dark Red"));
	mColourList->addListItem(new StringResource("Green"));
	mColourList->addListItem(new StringResource("Orange"));
	mColourList->addListItem(new StringResource("Purple"));
	mColourList->addListItem(new StringResource("Red"));
	mColourList->addListItem(new StringResource("Turquoise"));
	mColourList->addListItem(new StringResource("Yellow"));
}

//----------------------------------------------------------------------------

void Menu::button_invertMouseToggle()
{
	OgrianFrameListener* ofl = Renderer::getSingleton().getFrameListener();

	if (ofl->getInvertY())  // uninvert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText   ");

		mConfigYInvert = 0;
	}
	else // invert the mouse y axis
	{
		GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert")
			->setParameter("caption", "SS/Templates/BasicText  X");
		
		mConfigYInvert = 1;
	}
		
	ofl->setInvertY(mConfigYInvert > 0);
}

//----------------------------------------------------------------------------

void Menu::button_musicOff()
{
	mConfigMusicVolume = 0;  // deactivate the music

	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicOff")
		->setParameter("caption", "SS/Templates/BasicText X");  ////////
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicSoft")
		->setParameter("caption", "SS/Templates/BasicText  ");
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicMed")
		->setParameter("caption", "SS/Templates/BasicText  ");
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicLoud")
		->setParameter("caption", "SS/Templates/BasicText  ");

	Audio::getSingleton().stopSong();
}

//----------------------------------------------------------------------------

void Menu::button_musicSoft()
{
	mConfigMusicVolume = 64;  // set to a low volume

	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicOff")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicSoft")
		->setParameter("caption", "SS/Templates/BasicText X");	/////////
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicMed")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicLoud")
		->setParameter("caption", "SS/Templates/BasicText  ");

	Audio::getSingleton().playSong(CONS("THEME_MUSIC"), mConfigMusicVolume);
}

//----------------------------------------------------------------------------

void Menu::button_musicMed()
{
	mConfigMusicVolume = 128;  // set to the middle volume

	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicOff")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicSoft")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicMed")
		->setParameter("caption", "SS/Templates/BasicText X");	/////////
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicLoud")
		->setParameter("caption", "SS/Templates/BasicText  ");

	Audio::getSingleton().playSong(CONS("THEME_MUSIC"), mConfigMusicVolume);
}

//----------------------------------------------------------------------------

void Menu::button_musicLoud()
{
	mConfigMusicVolume = 255;  // set to the max volume

	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicOff")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicSoft")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicMed")
		->setParameter("caption", "SS/Templates/BasicText  ");	
	GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicLoud")
		->setParameter("caption", "SS/Templates/BasicText X");  ///////

	Audio::getSingleton().playSong(CONS("THEME_MUSIC"), mConfigMusicVolume);
}

//----------------------------------------------------------------------------

void Menu::button_configOk()
{	
	mConfigColour = static_cast<StringResource*>(mColourList->getSelectedItem())->getName();

	String wizardName = static_cast<StringResource*>(mWizardSkinList->getSelectedItem())->getName();
	String castleName = static_cast<StringResource*>(mCastleSkinList->getSelectedItem())->getName();

	// do a reverse lookup on the wizard skin
	mConfigWizardSkin = 0;
	int num = SkinManager::getSingleton().numWizardSkins();
	for (int i=0; i<num; i++)
	{
		String name = SkinManager::getSingleton().getRawWizardSkin(i);
		if (name == wizardName)
		{
			mConfigWizardSkin = i;
			break;
		}
	}

	// do a reverse lookup on the castle skin
	mConfigCastleSkin = 0;
	num = SkinManager::getSingleton().numCastleSkins();
	for (int i=0; i<num; i++)
	{
		String name = SkinManager::getSingleton().getRawCastleSkin(i);
		if (name == castleName)
		{
			mConfigCastleSkin = i;
			break;
		}
	}

	button_back();
	writeConfig();
}
//----------------------------------------------------------------------------

void Menu::button_quit()
{
	// do nothing exra
}

//----------------------------------------------------------------------------

void Menu::button_back()
{
	mLoadingOverlay->hide();

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

	button_back();

	loadMap(static_cast<StringResource*>(mMapList->getSelectedItem())->getName());
}

//----------------------------------------------------------------------------

void Menu::button_hostload() 
{
	if (Multiplayer::getSingleton().isServer()) Multiplayer::getSingleton().serverDisconnect();
	if (Multiplayer::getSingleton().isClient()) Multiplayer::getSingleton().clientDisconnect();

	Multiplayer::getSingleton().serverStart();

	button_back();
	mMainMenuPanel->hide();
	mConnectionMenuPanel->show();

	loadMap(static_cast<StringResource*>(mMapList->getSelectedItem())->getName());
	setMessage(CONS("MSG_SERVER_LOADING"));
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
	
	writeConfig(); // save the server name
	button_back();

	mMainMenuPanel->hide();
	mConnectionMenuPanel->show();
	mLoadingOverlay->show();
}

//----------------------------------------------------------------------------

void Menu::button_joinback()
{
	writeConfig(); // save the server name
	button_back();
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

void Menu::clearLoadingScreen()
{
	mLoadingOverlay->hide();
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
    if ( input->isKeyDown( KC_ESCAPE) && mTimeUntilNextToggle <= 0
		&& !mLoadingOverlay->isVisible()
		&& Renderer::getSingleton().getMapName() != "")
    {            
        hide();
    }

	return true;
}

//----------------------------------------------------------------------------

void Menu::keyPressed(KeyEvent* e)
{
	char c = e->getKeyChar();

	if (' ' <= c && c <= '~')
	{
		if (mConfigMenuPanel->isVisible())
		{
			// add this letter to the player name
			mConfigName.push_back(c);
			mNameText->setCaption(mConfigName);
		}
		else if (mJoinMenuPanel->isVisible())
		{
			// add this letter to the server name
			mConfigServer.push_back(c);
			mServerText->setCaption(mConfigServer);
		}
	}

	else if (e->getKey() == KC_BACK)
	{
		if (mConfigMenuPanel->isVisible() && mConfigName.length() > 0)
		{
			// remove the last letter from the player name
			mConfigName.resize(mConfigName.length()-1);
			mNameText->setCaption(mConfigName);
		}
		else if (mJoinMenuPanel->isVisible() && mConfigServer.length() > 0)
		{
			// add this letter to the server name
			mConfigServer.resize(mConfigServer.length()-1);
			mServerText->setCaption(mConfigServer);
		}
	}
}

//----------------------------------------------------------------------------

void Menu::loadMap(String mapname)
{
	// Loading...
	mLoadingOverlay->show();
	setMessage(CONS("MSG_LOADMAP"));

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

		// hide the loading screen
		mLoadingOverlay->hide();

		mLoadMap = false;
	}

	// flash a cursor on the text entry boxes
	if (Clock::getSingleton().getTime() >= mNextTextCursorFlashTime)
	{
		mNextTextCursorFlashTime = 
			Clock::getSingleton().getTime() + CONT("TEXT_CURSOR_FLASH_PERIOD");

		mTextCursorFlash = !mTextCursorFlash;
		
		if (mConfigMenuPanel->isVisible())
		{
			if (mTextCursorFlash)
				mNameText->setCaption(mConfigName);
			else
				mNameText->setCaption(mConfigName + "_");
		}
		else if (mJoinMenuPanel->isVisible())
		{
			if (mTextCursorFlash)
				mServerText->setCaption(mConfigServer);
			else
				mServerText->setCaption(mConfigServer + "_");
		}
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
	if (mConfigMusicVolume > 0)
		Audio::getSingleton().playSong(CONS("THEME_MUSIC"), mConfigMusicVolume);

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
	if (mConfigMusicVolume > 0)
		Audio::getSingleton().playSong(Game::getSingleton().getMapMusic(), mConfigMusicVolume);

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