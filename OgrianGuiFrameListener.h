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
OgrianGuiFrameListener.h
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: this handles mouse input for the Gui, including buttons. 
/*------------------------------------*/

#ifndef __OgrianGuiFrameListener_H__
#define __OgrianGuiFrameListener_H__

#include "Ogre.h"
#include "OgreEventListeners.h"
#include "OgreException.h"
#include <OgreKeyEvent.h>

using namespace Ogre;

namespace Ogrian
{
	
class GuiFrameListener : public FrameListener, public MouseMotionListener, public MouseListener, public KeyListener
{
private:
    CEGUI::Renderer* mGUIRenderer;
    bool mShutdownRequested;
    EventProcessor* mEventProcessor;

public:
    // NB using buffered input, this is the only change
    GuiFrameListener(RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer)
        : mGUIRenderer(renderer),
          mShutdownRequested(false)
    {
		mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
		mEventProcessor->startProcessingEvents();
        mEventProcessor->addMouseMotionListener(this);
        mEventProcessor->addMouseListener(this);
		mEventProcessor->addKeyListener(this);
    }

	CEGUI::MouseButton convertOgreButtonToCegui(int buttonID)
	{
		switch (buttonID)
		{
		case MouseEvent::BUTTON0_MASK:
			return CEGUI::LeftButton;
		case MouseEvent::BUTTON1_MASK:
			return CEGUI::RightButton;
		case MouseEvent::BUTTON2_MASK:
			return CEGUI::MiddleButton;
		case MouseEvent::BUTTON3_MASK:
			return CEGUI::X1Button;
		default:
			return CEGUI::LeftButton;
		}
	}

    /// Tell the frame listener to exit at the end of the next frame
    void requestShutdown(void)
    {
        mShutdownRequested = true;
    }

    bool frameEnded(const FrameEvent& evt)
    {
        if (mShutdownRequested)
            return false;
        else
            return FrameListener::frameEnded(evt);
    }

    void mouseMoved (MouseEvent *e)
    {
        CEGUI::System::getSingleton().injectMouseMove(
                e->getRelX() * mGUIRenderer->getWidth(), 
                e->getRelY() * mGUIRenderer->getHeight());
        e->consume();
    }

    void mouseDragged (MouseEvent *e) 
    { 
        mouseMoved(e);
    }

    void mousePressed (MouseEvent *e)
    {
        CEGUI::System::getSingleton().injectMouseButtonDown(
			convertOgreButtonToCegui(e->getButtonID()));
        e->consume();
    }

    void mouseReleased (MouseEvent *e)
    {
        CEGUI::System::getSingleton().injectMouseButtonUp(
          convertOgreButtonToCegui(e->getButtonID()));
        e->consume();
    }

	void mouseClicked(MouseEvent* e) {}
	void mouseEntered(MouseEvent* e) {}
	void mouseExited(MouseEvent* e) {}

    void keyPressed(KeyEvent* e)
    {
        if(e->getKey() == KC_F11)
        {
            mShutdownRequested = true;
            e->consume();
            return;
        }

        CEGUI::System::getSingleton().injectKeyDown(e->getKey());
		CEGUI::System::getSingleton().injectChar(e->getKeyChar());
        e->consume();
    }

	void keyReleased(KeyEvent* e)
	{
		CEGUI::System::getSingleton().injectKeyUp(e->getKey());
		e->consume();
	}
	void keyClicked(KeyEvent* e) 
	{
		// Do nothing
		e->consume();
	}
};

}

#endif 