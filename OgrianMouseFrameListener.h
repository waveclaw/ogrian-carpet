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
OgrianFrameListener.h
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: this handles mouse input for the menu, including buttons. 
/*------------------------------------*/



#ifndef __OgrianMouseFrameListener_H__
#define __OgrianMouseFrameListener_H__

#include "Ogre.h"
#include "OgreEventListeners.h"
#include "OgreException.h"
#include "OgreButtonGuiElement.h"

#include "OgrianMenu.h"

using namespace Ogre;

namespace Ogrian
{

class OgrianMouseFrameListener: public FrameListener, public ActionListener
{
public:
    // Constructor takes a RenderWindow because it uses that to determine input context
    OgrianMouseFrameListener(RenderWindow* win)
    {
		mQuit = false;

        mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
        OverlayManager::getSingleton().createCursorOverlay();
		mEventProcessor->startProcessingEvents();
		mInputDevice = mEventProcessor->getInputReader();

		registerButtons();
    }
    virtual ~OgrianMouseFrameListener()
    {
		delete mEventProcessor;
    }


    bool processUnbufferedMouseInput(const FrameEvent& evt)
    {
		return true;
	}

    // Override frameStarted event to process that (don't care about frameEnded)
    bool frameStarted(const FrameEvent& evt)
    {
		return !mQuit;
    }

    bool frameEnded(const FrameEvent& evt)
    {
        return true;
    }

	void actionPerformed(ActionEvent* e) 
	{
		std::string action = e->getActionCommand();

		// REMEMBER TO REGISTER THE BUTTON BELOW!
		if (action == "Ogrian/MainMenu/Quit")
		{
			Menu::getSingleton().button_quit();
			mQuit = true; 
		}
		else if (action == "Ogrian/SkirmishMenu/Load") Menu::getSingleton().button_skirmishload();
		else if (action == "Ogrian/SkirmishMenu/Back") Menu::getSingleton().button_back();
		else if (action == "Ogrian/SkirmishMenu/SelectNormal") Menu::getSingleton().button_skirmishSelectNormal();
		else if (action == "Ogrian/SkirmishMenu/SelectRandom") Menu::getSingleton().button_skirmishSelectRandom();
		else if (action == "Ogrian/SkirmishMenu/SelectBot") Menu::getSingleton().button_skirmishSelectBot();

		else if (action == "Ogrian/HostMenu/Load") Menu::getSingleton().button_hostload();
		else if (action == "Ogrian/HostMenu/Back") Menu::getSingleton().button_back();

		else if (action == "Ogrian/JoinMenu/Load") Menu::getSingleton().button_joinload();
		else if (action == "Ogrian/JoinMenu/Back") Menu::getSingleton().button_joinback();

		else if (action == "Ogrian/ConfigMenu/Yinvert") Menu::getSingleton().button_invertMouseToggle();
		else if (action == "Ogrian/ConfigMenu/MusicOff") Menu::getSingleton().button_musicOff();
		else if (action == "Ogrian/ConfigMenu/MusicSoft") Menu::getSingleton().button_musicSoft();
		else if (action == "Ogrian/ConfigMenu/MusicMed") Menu::getSingleton().button_musicMed();
		else if (action == "Ogrian/ConfigMenu/MusicLoud") Menu::getSingleton().button_musicLoud();
		else if (action == "Ogrian/ConfigMenu/Ok") Menu::getSingleton().button_configOk();

		else if (action == "Ogrian/MainMenu/Skirmish") Menu::getSingleton().button_skirmish();
		else if (action == "Ogrian/MainMenu/Host") Menu::getSingleton().button_host();
		else if (action == "Ogrian/MainMenu/Join") Menu::getSingleton().button_join();
		else if (action == "Ogrian/MainMenu/Config") Menu::getSingleton().button_config();
		else if (action == "Ogrian/MainMenu/Credits") Menu::getSingleton().button_credits();

		else if (action == "Ogrian/ConnectionMenu/Disconnect") Menu::getSingleton().button_disconnect();
		else if (action == "Ogrian/ConnectionMenu/StartGame") Menu::getSingleton().button_startGame();
		else if (action == "Ogrian/ConnectionMenu/Back") Menu::getSingleton().button_back();
		
		else if (action == "Ogrian/CreditsMenu/Back") Menu::getSingleton().button_back();
    }

	bool isMulticaster() {}

protected:
    EventProcessor* mEventProcessor;
    InputReader* mInputDevice;

	bool mQuit; 
	
	void registerButtons()
	{
		// REMEMBER TO CALL THE MENU ABOVE!!
		ActionTarget* button;

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/SkirmishMenu/Load"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/SkirmishMenu/SelectNormal"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/SkirmishMenu/SelectRandom"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/SkirmishMenu/SelectBot"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/SkirmishMenu/Back"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/HostMenu/Load"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/HostMenu/Back"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/JoinMenu/Load"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/JoinMenu/Back"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/CreditsMenu/Back"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Yinvert"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicOff"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicSoft"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicMed"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/MusicLoud"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConfigMenu/Ok"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Skirmish"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Host"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Join"));
		button->addActionListener(this);
		
		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Config"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Credits"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/MainMenu/Quit"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConnectionMenu/Disconnect"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/ConnectionMenu/StartGame"));
		button->addActionListener(this);
	}

};

}
#endif
