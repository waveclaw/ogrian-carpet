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
OgrianRenderer.h
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: This is the main interface with the Ogre rendering Engine. 
It also handles all scene creation. 
Later, Thing placing will be moved to a WorldMaker class. 
It is a Singleton.

 *------------------------------------*/

#ifndef __OgrianRenderer_H__
#define __OgrianRenderer_H__

#include <Ogre.h>
#include <OgreConfigFile.h>
#include "OgrianFrameListener.h"
#include "OgrianPhysics.h"
#include "OgrianDotManager.h"
#include "OgrianFoliageThing.h"
#include "OgrianGame.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianMenu.h"
#include "OgrianHud.h"
#include "OgrianManaThing.h"
#include "OgrianFrameListener.h"
#include "OgrianMouseFrameListener.h"
#include "OgrianCameraThing.h"

using namespace Ogre;

namespace Ogrian
{

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class Renderer : public Singleton< Renderer >
{
public:

    /// Standard destructor
    virtual ~Renderer();

    /// Start the renderer
    void go(void);

	// get the SceneManager
	SceneManager* getSceneManager(void);

	// get the Camera Thing
	CameraThing* getCameraThing();

	// get the Camera (use this when the camerathing has not been made yet)
	Camera* getCamera();

	// get the frame listener
	OgrianFrameListener* getFrameListener();

	// load a map
	void loadMap(String configfile, bool server=true);

	// get the name of the current map
	String getMapName();

	// get the foliage name for the current map
	String getFoliageMaterial();

	// unload the map
	void unloadMap();

	void createCameraThing(void);

	static Renderer& getSingleton(void);

protected:
    Root *mRoot;
    Camera* mCamera;
    CameraThing* mCameraThing;
    SceneManager* mSceneMgr;
    OgrianFrameListener* mFrameListener;
    OgrianMouseFrameListener* mMouseFrameListener;
    RenderWindow* mWindow;
    SceneNode* mFountainNode;
	SceneNode* mWaterNode;
	Entity* mWaterEntity;

	bool mMapLoaded;
	String mMapName;
	String mFoliageMaterial;

    /// Standard constructor
    Renderer();

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    bool setup(void);
    /** Configures the application - returns false if the user chooses to abandon configuration. */
    bool configure(void);

    void chooseSceneManager(void);
    void createCamera(void);
    void createFrameListener(void);

	// Create a sky using the specified material
	void createSky(const String& material);

	// create an ocean using the specified material
	void createOcean(const String& material);

    // Just override the mandatory create scene method
    void createScene(void);

    void createViewports(void);

    /// Method which will define the source of resources (other than current folder)
    void setupResources(void);

};

}
#endif
