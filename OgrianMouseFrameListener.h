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

Description: 
/*------------------------------------*/



#ifndef __OgrianMouseFrameListener_H__
#define __OgrianMouseFrameListener_H__

#include "Ogre.h"
#include "OgreEventListeners.h"
#include "OgreException.h"

#include "OgreButtonGuiElement.h"

using namespace Ogre;

namespace Ogrian
{

class OgrianMouseFrameListener: public FrameListener, public ActionListener
{
public:
    // Constructor takes a RenderWindow because it uses that to determine input context
    OgrianMouseFrameListener(RenderWindow* win)
    {
        mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
        OverlayManager::getSingleton().createCursorOverlay();
		mEventProcessor->startProcessingEvents();
		mInputDevice = mEventProcessor->getInputReader();

		ActionTarget* quitButton = static_cast<ButtonGuiElement*>(GuiManager::getSingleton().getGuiElement("SS/Setup/HostScreen/Exit"));
		quitButton->addActionListener(this);

		mQuit = false;
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
      if (action == "SS/Setup/HostScreen/Exit")
      {
        mQuit = true;
      }
    }
	bool isMulticaster() {}

protected:
    EventProcessor* mEventProcessor;
    InputReader* mInputDevice;

	bool mQuit; 
};

}
#endif
