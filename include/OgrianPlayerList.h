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
OgrianPlayerList.h
Original Author: Mike Prosser
Additional Authors: 

Description: The Player List
 *------------------------------------*/

#ifndef __OgrianPlayerList_H__
#define __OgrianPlayerList_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgreOverlayManager.h"
#include "OgreBorderButtonGuiElement.h"
#include "OgreStringResource.h"

#define MAX_PLAYERS 8

using namespace Ogre;

namespace Ogrian
{

class PlayerList : public Singleton< PlayerList >
{
public:
	virtual ~PlayerList();

	// show the list
	void show();

	// hide the list
	void hide();

	// returns true when shown
	bool isActive();

	// show it if it is hidden, hide it if shown
	void toggle();

	// add a player to the list
	void addPlayer(String name);

	// add a player to the list
	void removePlayer(String name);

	// clear the list
	void clear();

	static PlayerList& getSingleton(void);

private:
	PlayerList();

	bool mActive; // wether or not the PlayerList mode is active
	Overlay* mOverlay; // the PlayerList overlay

	GuiElement* mList[MAX_PLAYERS]; // the GUI List
	std::vector<String> mPlayers; // a vector of the players names

	void updateList(); // make the gui match the vector
};

}

#endif
