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

}

Menu::~Menu()
{

}

void Menu::showMenu()
{
	// A bit of a hacky test
	Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("SS/Setup/HostScreen/Overlay");
	ActionTarget* at = static_cast<BorderButtonGuiElement*>(GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Join"));
	at->addActionListener(this);
	at = static_cast<BorderButtonGuiElement*>(GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Exit"));
	at->addActionListener(this);
	o->show();
	
	ListChanger* list = static_cast<ListGuiElement*>(GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/AvailableGamesList"));

	list->addListItem(new StringResource("test1"));
	list->addListItem(new StringResource("test2"));
	list->addListItem(new StringResource("test3"));
	list->addListItem(new StringResource("test4"));
	list->addListItem(new StringResource("test5"));
	list->addListItem(new StringResource("test6"));
	list->addListItem(new StringResource("test7"));
	list->addListItem(new StringResource("test8"));

	(GuiManager::getSingleton().getGuiElement("Core/CurrFps"))->addMouseListener(this);

	GuiContainer* pCursorGui = OverlayManager::getSingleton().getCursorGui();
	pCursorGui->setMaterialName("Cursor/default");
	pCursorGui->setDimensions(32.0/640.0, 32.0/480.0);
}

void Menu::actionPerformed(ActionEvent* e) 
{
    // Think about doing something here
    std::string action = e->getActionCommand();
    LogManager::getSingleton().logMessage("Got event: " + action);

    if (action == "SS/Setup/HostScreen/Exit")
    {
        // Queue a shutdown
        //static_cast<GuiFrameListener*>(mFrameListener)->requestShutdown();
    }
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