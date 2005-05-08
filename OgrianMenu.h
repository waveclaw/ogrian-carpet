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
OgrianMenu.h
Original Author: Mike Prosser
Additional Authors: 

Description: The Menu that allows you to choose what to play, and also
to set your config options
/*------------------------------------*/

#ifndef __OgrianMenu_H__
#define __OgrianMenu_H__

#include <Ogre.h>
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgrianInput.h"
#include "OgreListGuiElement.h"
#include "OgreListChanger.h"

#include "OgrianClock.h"

using namespace Ogre;

namespace Ogrian
{

class Menu : public Singleton< Menu >
{
public:
	virtual ~Menu();

	// read ogrian.cfg into the configure menu
	void readConfig();

	// the config options
	String getChosenName();
	String getChosenServer();
	ColourValue getChosenColour();
	int getChosenWizardSkin();
	int getChosenCastleSkin();

	int getChosenSkirmishMode(); // 0 = normal, 1 = random, 2 = bot

	// write the configure menu settings into ogrian.cfg
	void writeConfig();

	// go into menu mode and display the menu
	void show();

	// go into game mode and hide the menu
	void hide();

	// this is called by the OgrianFrameListener when in Menu mode
	bool processKeyInput(InputReader* input);

	// returns true when in menu mode
	bool isActive();

	// this should be called every frame
	void frame(Real time);
	
	// called by OgrianFrameEventListener 
	void keyPressed(KeyEvent* e);

	// the button functions
	void button_invertMouseToggle();
	void button_musicOff();
	void button_musicSoft();
	void button_musicMed();
	void button_musicLoud();

	void button_skirmishload();
	void button_skirmishSelectNormal();
	void button_skirmishSelectRandom();
	void button_skirmishSelectBot();

	void button_hostload();
	void button_joinload();
	void button_joinback();

	void button_skirmish();
	void button_join();
	void button_host();
	void button_config();
	void button_credits();

	void button_configOk();

	void button_disconnect();
	void button_startGame();

	void button_quit();
	void button_back();

	void clearLoadingScreen();

	// set the message
	void setMessage(String message);

	static Menu& getSingleton(void);

private:
	Menu();

	// load a map
	void loadMap(String mapname);

	// refresh the map desription
	void selectMap(String mapname);

	// refresh the colour
	void selectColour(String colour);

	// refresh the wizard skin
	String mSelectedWizard;
	void selectWizardSkin(String colour);

	// load the lists
	void loadMapList();
	void loadColourList();
	void loadWizardSkinList();
	void loadCastleSkinList();

	bool mActive; // wether or not the menu mode is active
    Real mTimeUntilNextToggle; // prevents rapid toggling
	Overlay* mOverlay; // the menu overlay

	bool mLoadMap; // true when a map is ready to be loaded
	String mMapSelection; // the map selection
	String mMapName; // the name of the map
	int mLoadDelay; // counts down

	bool mTextCursorFlash; // for flashing the text cursor
	Time mNextTextCursorFlashTime;

	// the gui objects
	GuiContainer* mCursor; // the cursor

	ListChanger* mMapList; // the list of maps
	ListChanger* mColourList; // the list of colours
	ListChanger* mWizardSkinList; // the list of wizard skins
	ListChanger* mCastleSkinList; // the list of castle skins

	// panels for the lists
	GuiElement* mMapListPanel;
	GuiElement* mColourListPanel;
	GuiElement* mWizardSkinListPanel;
	GuiElement* mCastleSkinListPanel;

	GuiElement* mNameText; // the textbox for entering the player name
	GuiElement* mServerText; // the textbox for entering the server name

	// each submenu has a panel
	GuiElement* mMainMenuPanel;
	GuiElement* mSkirmishMenuPanel;
	GuiElement* mHostMenuPanel;
	GuiElement* mJoinMenuPanel;
	GuiElement* mConfigMenuPanel;
	GuiElement* mConnectionMenuPanel;
	GuiElement* mCreditsMenuPanel;

	// the map description panel - used by the host and skirmish menus
	GuiElement* mMapPanel;

	// the loading screen
	Overlay* mLoadingOverlay;

	// configuration options
	String mConfigName;
	String mConfigServer;
	String mConfigColour;
	int mConfigWizardSkin;
	int mConfigCastleSkin;
	int mConfigMusicVolume;
	int mConfigYInvert;
	int mConfigSkirmishRandom;

};

}

#endif