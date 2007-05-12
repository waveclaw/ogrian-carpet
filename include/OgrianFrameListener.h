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

Description: Defines a frame listener which responds to frame events.
             This frame listener handles keyboard inputs and mouse movements.
             Mouse:    Freelook (y inverted)
             W:        move forward
             S:        move backward
             A:        move left
             D:        move right
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)

 *------------------------------------*/



#ifndef __OgrianFrameListener_H__
#define __OgrianFrameListener_H__

#include "Ogre.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

using namespace Ogre;

namespace Ogrian
{

class OgrianFrameListener: public FrameListener, public KeyListener
{
private:
	int mSceneDetailIndex ;

    void updateStats(void);

public:
    // Constructor takes a RenderWindow because it uses that to determine input context
    OgrianFrameListener(RenderWindow* win, Camera* cam, bool useBufferedInputKeys = false, bool useBufferedInputMouse = false);

    virtual ~OgrianFrameListener();

    virtual bool processUnbufferedKeyInput(const FrameEvent& evt);

    bool processUnbufferedMouseInput(const FrameEvent& evt);

	void moveCamera();

    void showDebugOverlay(bool show);

    // Override frameStarted event to process that (don't care about frameEnded)
    bool frameStarted(const FrameEvent& evt);
    
    bool frameEnded(const FrameEvent& evt);

	void switchMouseMode() ;

	void switchKeyMode() ;

	void setInvertY(bool yinv);
	bool getInvertY();

	void setCameraFrozen(bool frozen);
	bool getCameraFrozen();

	void setGameRunning(bool running);
	bool getGameRunning();

	void keyClicked(KeyEvent* e) {}
	void keyPressed(KeyEvent* e) ;
	void keyReleased(KeyEvent* e) {}

protected:
    EventProcessor* mEventProcessor;
    InputReader* mInputDevice;
    Camera* mCamera;

    Vector3 mTranslateVector;
    RenderWindow* mWindow;
	bool mYinvert;
    bool mStatsOn;
    bool mUseBufferedInputKeys, mUseBufferedInputMouse, mInputTypeSwitchingOn;
	unsigned int mNumScreenShots;
    float mMoveScale;
    float mRotScale;
    // just to stop toggles flipping too fast
    Real mTimeUntilNextToggle ;
    float mRotX, mRotY;
    TextureFilterOptions mFiltering;
    int mAniso;

	bool mGameRunning;
	bool mCameraFrozen;

};

}
#endif
