
/*
-----------------------------------------------------------------------------
Filename:    OgrianFrameListener.h
Description: Defines an example frame listener which responds to frame events.
             This frame listener just moves a specified camera around based on
             keyboard and mouse movements.
             Mouse:    Freelook
             W or Up:  Forward
             S or Down:Backward
             A:           Step left
             D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             O/P:       Yaw the root scene node (and it's children)
             I/K:       Pitch the root scene node (and it's children)
             F:           Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
-----------------------------------------------------------------------------
*/

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

	void keyClicked(KeyEvent* e) ;
	void keyPressed(KeyEvent* e) {}
	void keyReleased(KeyEvent* e) {}

protected:
    EventProcessor* mEventProcessor;
    InputReader* mInputDevice;
    Camera* mCamera;

    Vector3 mTranslateVector;
    RenderWindow* mWindow;
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

};

}
#endif
