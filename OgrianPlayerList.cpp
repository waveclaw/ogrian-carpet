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
OgrianPlayerList.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The Player List
/*------------------------------------*/

#include "OgrianPlayerList.h"
#include "OgrianRenderer.h"

#include "OgreOverlayManager.h"
#include "OgreBorderButtonGuiElement.h"
#include "OgreStringResource.h"

template<> Ogrian::PlayerList * Singleton< Ogrian::PlayerList >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

PlayerList::PlayerList()
{
	mActive = false;

	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/PlayerList/Overlay");
	mList = static_cast<ListGuiElement*>(GuiManager::getSingleton().getGuiElement("Ogrian/PlayerList/List"));

	// build the list
	addPlayer("Alice");
	addPlayer("Bob");
	addPlayer("Charlie");
	addPlayer("Dan");
	addPlayer("Ed");
	addPlayer("Fred");
	addPlayer("Ghandi");
	addPlayer("Harry");
	addPlayer("Ian");
	addPlayer("Joss");
}

//----------------------------------------------------------------------------

PlayerList::~PlayerList()
{
	if (mActive) hide();
}

//----------------------------------------------------------------------------

void PlayerList::addPlayer(String name)
{
	mList->addListItem(new StringResource(name));
}

//----------------------------------------------------------------------------

void PlayerList::show()
{
	if (mActive == true) return;

	mOverlay->show();

	mActive = true;
}

//----------------------------------------------------------------------------

void PlayerList::hide()
{
	if (mOverlay == false) return;

	mOverlay->hide();

	mActive = false;
}

//----------------------------------------------------------------------------

void PlayerList::toggle()
{
	if (mActive) hide();
	else show();
}

//----------------------------------------------------------------------------

bool PlayerList::isActive()
{
	return mActive;
}

//----------------------------------------------------------------------------

PlayerList& PlayerList::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new PlayerList();
	}
	return Singleton<PlayerList>::getSingleton();
}

}