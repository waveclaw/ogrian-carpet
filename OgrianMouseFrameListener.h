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
		if (action == "Ogrian/Menu/Exit")
		{
			Menu::getSingleton().button_quit();
			mQuit = true; 
		}
		else if (action == "Ogrian/Menu/Load") Menu::getSingleton().button_load();
		else if (action == "Ogrian/Menu/Yinvert") Menu::getSingleton().button_invertMouseToggle();
		else if (action == "Ogrian/Menu/Music") Menu::getSingleton().button_musicToggle();
		else if (action == "Ogrian/Menu/Host") Menu::getSingleton().button_host();
		else if (action == "Ogrian/Menu/Join") Menu::getSingleton().button_join();
		else if (action == "Ogrian/Menu/Disconnect") Menu::getSingleton().button_disconnect();
		else if (action == "Ogrian/Menu/StartGame") Menu::getSingleton().button_startGame();
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
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Exit"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Load"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Yinvert"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Music"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Host"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Join"));
		button->addActionListener(this);
		
		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/Disconnect"));
		button->addActionListener(this);

		button = static_cast<ButtonGuiElement*>
			(GuiManager::getSingleton().getGuiElement("Ogrian/Menu/StartGame"));
		button->addActionListener(this);
	}

};

}
#endif
