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
OgrianFrameListener.cpp
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: This handles keyboard and mouse input. 
See OgrianFrameListener.h for command listings

/*------------------------------------*/


#include "OgreConfigFile.h"

#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianAudio.h"
#include "OgrianInput.h"
#include "OgrianCameraThing.h"
#include "OgrianRenderer.h"
#include "OgrianMenu.h"

namespace Ogrian
{

//----------------------------------------------------------------------------

void OgrianFrameListener::updateStats(void)
{
    static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";

    // update stats when necessary
    GuiElement* guiAvg = GuiManager::getSingleton().getGuiElement("Core/AverageFps");
    GuiElement* guiCurr = GuiManager::getSingleton().getGuiElement("Core/CurrFps");
    GuiElement* guiBest = GuiManager::getSingleton().getGuiElement("Core/BestFps");
    GuiElement* guiWorst = GuiManager::getSingleton().getGuiElement("Core/WorstFps");

    const RenderTarget::FrameStats& stats = mWindow->getStatistics();

    guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
    guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
    guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
        +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
    guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
        +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

    GuiElement* guiTris = GuiManager::getSingleton().getGuiElement("Core/NumTris");
    guiTris->setCaption(tris + StringConverter::toString(unsigned int(stats.triangleCount)));

    GuiElement* guiDbg = GuiManager::getSingleton().getGuiElement("Core/DebugText");
    guiDbg->setCaption(mWindow->getDebugText());
}

//----------------------------------------------------------------------------

// Constructor takes a RenderWindow because it uses that to determine input context
OgrianFrameListener::OgrianFrameListener(RenderWindow* win, Camera* cam, bool useBufferedInputKeys, bool useBufferedInputMouse)
{
    mUseBufferedInputKeys = useBufferedInputKeys;
	mUseBufferedInputMouse = useBufferedInputMouse;
	mInputTypeSwitchingOn = mUseBufferedInputKeys || mUseBufferedInputMouse;

	//if (mInputTypeSwitchingOn)
	//{
 //       mEventProcessor = new EventProcessor();
	//	mEventProcessor->initialise(win);
	//	mEventProcessor->startProcessingEvents();
	//	mEventProcessor->addKeyListener(this);
	//	mInputDevice = mEventProcessor->getInputReader();

	//	Menu::getSingleton().show();
	//}
 //   else
 //   {
 //       mInputDevice = PlatformManager::getSingleton().createInputReader();
 //       mInputDevice->initialise(win,true, true);
 //   }

        mInputDevice = PlatformManager::getSingleton().createInputReader();
        mInputDevice->initialise(win,true, true);
        mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
		mEventProcessor->startProcessingEvents();
		mEventProcessor->addKeyListener(this);

		Menu::getSingleton().show();


    mCamera = cam;
    mWindow = win;
    mStatsOn = false;
	mNumScreenShots = 0;
	mTimeUntilNextToggle = 0;
    mSceneDetailIndex = 0;
    mMoveScale = 0.0f;
    mRotScale = 0.0f;
	mTranslateVector = Vector3::ZERO;
    mAniso = 1;
    mFiltering = TFO_BILINEAR;

	mGameRunning = false;
	mYinvert = true;
	mCameraFrozen = true;
}

//----------------------------------------------------------------------------

OgrianFrameListener::~OgrianFrameListener()
{
	if (mInputTypeSwitchingOn)
	{
        delete mEventProcessor;
	}
    else
    {
        PlatformManager::getSingleton().destroyInputReader( mInputDevice );
    }
}

//----------------------------------------------------------------------------

void OgrianFrameListener::setCameraFrozen(bool frozen)
{
	mCameraFrozen = frozen;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::getCameraFrozen()
{
	return mCameraFrozen;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
{
	// handle universal keypresses //

	// see if switching is on, and you want to toggle 
    if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_F5) && mTimeUntilNextToggle <= 0)
    {
		switchMouseMode();
        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }

    if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_F6) && mTimeUntilNextToggle <= 0)
    {
		// must be going from immediate keyboard to buffered keyboard
		switchKeyMode();
        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }
    if (mInputDevice->isKeyDown(KC_F1) && mTimeUntilNextToggle <= 0)
    {
        mStatsOn = !mStatsOn;
        showDebugOverlay(mStatsOn);

        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }
    if (mInputDevice->isKeyDown(KC_F2) && mTimeUntilNextToggle <= 0)
    {
        switch(mFiltering)
        {
        case TFO_BILINEAR:
            mFiltering = TFO_TRILINEAR;
            mAniso = 1;
            break;
        case TFO_TRILINEAR:
            mFiltering = TFO_ANISOTROPIC;
            mAniso = 8;
            break;
        case TFO_ANISOTROPIC:
            mFiltering = TFO_BILINEAR;
            mAniso = 1;
            break;
        }
        MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
        MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);


        showDebugOverlay(mStatsOn);

        mTimeUntilNextToggle = CONR("KEY_DELAY");
    }

    if (mInputDevice->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
    {
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
        mWindow->writeContentsToFile(tmp);
        mTimeUntilNextToggle = CONR("KEY_DELAY");
		mWindow->setDebugText(String("Wrote ") + tmp);
    }
	
	if (mInputDevice->isKeyDown(KC_F3) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setDetailLevel(SDL_SOLID) ; break ;
			case 1 : mCamera->setDetailLevel(SDL_WIREFRAME) ; break ;
			case 2 : mCamera->setDetailLevel(SDL_POINTS) ; break ;
		}
		mTimeUntilNextToggle = CONR("KEY_DELAY");
	}

	// handle menu or game keypresses
	if (Menu::getSingleton().isActive()) 
	{
		return Menu::getSingleton().processKeyInput(mInputDevice);
	}
	else
	{
		// handle camera stuff //
		CameraThing* cam = Renderer::getSingleton().getCameraThing();

		// Move camera forward
		if (mInputDevice->isKeyDown(KC_W)) cam->moveForward();
		if (mInputDevice->isKeyDown(KC_UP)) cam->moveForward();
		
		// Move camera back
		if (mInputDevice->isKeyDown(KC_S)) cam->moveBack();
		if (mInputDevice->isKeyDown(KC_DOWN)) cam->moveBack();

		// Move camera left
		if (mInputDevice->isKeyDown(KC_A)) cam->moveLeft();
		if (mInputDevice->isKeyDown(KC_LEFT)) cam->moveLeft();

		// Move camera right
		if (mInputDevice->isKeyDown(KC_D)) cam->moveRight();
		if (mInputDevice->isKeyDown(KC_RIGHT)) cam->moveRight();

		// handle game input //
		return Input::getSingleton().processKeyInput(mInputDevice);
	}
}

//----------------------------------------------------------------------------

void OgrianFrameListener::setInvertY(bool yinv)
{
	mYinvert = yinv;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::getInvertY()
{
	return mYinvert;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::processUnbufferedMouseInput(const FrameEvent& evt)
{
	// skip if the camera is frozen
	if (mCameraFrozen) return true;

    /* Rotation factors, may not be used if the second mouse button is pressed. */
	mRotX = -mInputDevice->getMouseRelativeX() * 0.13;

	// this handles inverting the mouse y axis
	mRotY =  mInputDevice->getMouseRelativeY() * 0.13 * (mYinvert ? 1 : -1);

	return true;
}

//----------------------------------------------------------------------------

void OgrianFrameListener::moveCamera()
{
	if (mGameRunning)
		Renderer::getSingleton().getCameraThing()->moveCamera(mRotX, mRotY);
}

//----------------------------------------------------------------------------

void OgrianFrameListener::showDebugOverlay(bool show)
{
    Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
    if (!o)
        Except( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay Core/DebugOverlay",
            "showDebugOverlay" );
    if (show)
    {
        o->show();
    }
    else
    {
        o->hide();
    }
}

//----------------------------------------------------------------------------

// Override frameStarted event to process that (don't care about frameEnded)
bool OgrianFrameListener::frameStarted(const FrameEvent& evt)
{
	Game::getSingleton().frame(evt.timeSinceLastFrame);

	// handle input //
    if (!mInputTypeSwitchingOn)
    {
        mInputDevice->capture();
    }


	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
		if (mTimeUntilNextToggle >= 0) 
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			mMoveScale = 1;
			mRotScale = 0.1;
		}
		// Otherwise scale movement units by time passed since last frame
		else
		{
			// Move about 100 units per second,
			mMoveScale = CONR("CAMERA_MOVE_SPEED") * evt.timeSinceLastFrame;
			// Take about 10 seconds for full rotation
			mRotScale = 36 * evt.timeSinceLastFrame;
		}
		mRotX = 0;
        mRotY = 0;
	    mTranslateVector = Vector3::ZERO;
	}

    if (mUseBufferedInputKeys)
    {
        // no need to do any processing here, it is handled by event processor and 
		// you get the results as KeyEvents
    }
    else
    {
        if (processUnbufferedKeyInput(evt) == false)
		{
			return false;
		}
    }
    if (mUseBufferedInputMouse)
    {
        // no need to do any processing here, it is handled by event processor and 
		// you get the results as MouseEvents
    }
    else
    {
        if (processUnbufferedMouseInput(evt) == false)
		{
			return false;
		}
    }

	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so update the movement vector
		moveCamera();
	}

	return true;
}

//----------------------------------------------------------------------------

void OgrianFrameListener::setGameRunning(bool running)
{
	mGameRunning = running;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::getGameRunning()
{
	return mGameRunning;
}

//----------------------------------------------------------------------------

bool OgrianFrameListener::frameEnded(const FrameEvent& evt)
{
    updateStats();
    return true;
}

//----------------------------------------------------------------------------

void OgrianFrameListener::switchMouseMode() 
{
    mUseBufferedInputMouse = !mUseBufferedInputMouse;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}

//----------------------------------------------------------------------------

void OgrianFrameListener::switchKeyMode() 
{
    mUseBufferedInputKeys = !mUseBufferedInputKeys;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}

//----------------------------------------------------------------------------

void OgrianFrameListener::keyPressed(KeyEvent* e) 
{
	// forwared the keypress to the menu for text entry
	if (Menu::getSingleton().isActive())
		Menu::getSingleton().keyPressed(e);
}

//----------------------------------------------------------------------------

}