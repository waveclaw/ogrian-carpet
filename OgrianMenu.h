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

Description: The Menu
/*------------------------------------*/

#ifndef __OgrianMenu_H__
#define __OgrianMenu_H__

#include <Ogre.h>
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgrianInput.h"
//#include "OgreListGuiElement.h"
//#include "OgreListChanger.h"

using namespace Ogre;

namespace Ogrian
{

class Menu : public Singleton< Menu >
{
public:
	virtual ~Menu();

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

	// the button functions
	void button_invertMouseToggle();
	void button_musicToggle();
	void button_load();
	void button_join();
	void button_host();
	void button_disconnect();
	void button_startGame();
	void button_quit();

	// set the message
	void setMessage(String message);

	static Menu& getSingleton(void);

private:
	Menu();

	// load a map
	void loadMap(String mapname);

	// load the map list
	void loadMapList();

	bool mMusic; // wether or not to play music

	bool mActive; // wether or not the menu mode is active
    Real mTimeUntilNextToggle; // prevents rapid toggling
	Overlay* mOverlay; // the menu overlay

	bool mLoadMap; // true when a map is ready to be loaded
	String mMapName; // the name of the map
	int mLoadDelay; // counts down

//	GuiContainer* mCursor; // the cursor
//	ListChanger* mList; // the list of maps
};

}

#endif