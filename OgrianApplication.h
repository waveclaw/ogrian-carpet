/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    OgrianApplication.h
Description: Main class for Ogrian Carpet
-----------------------------------------------------------------------------
*/

#ifndef __OgrianApplication_H__
#define __OgrianApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"


using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class OgrianApplication
{
public:
    /// Standard constructor
    OgrianApplication()
    {
        mFrameListener = 0;
        mRoot = 0;
    }
    /// Standard destructor
    virtual ~OgrianApplication()
    {
        if (mFrameListener)
            delete mFrameListener;
        if (mRoot)
            delete mRoot;
    }

    /// Start the example
    virtual void go(void)
    {
        if (!setup())
            return;

        mRoot->startRendering();
    }

protected:
    Root *mRoot;
    Camera* mCamera;
    SceneManager* mSceneMgr;
    OgrianFrameListener* mFrameListener;
    RenderWindow* mWindow;

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    virtual bool setup(void)
    {
        mRoot = new Root();

        setupResources();

        bool carryOn = configure();
        if (!carryOn) return false;

        chooseSceneManager();
        createCamera();
        createViewports();

        // Set default mipmap level (NB some APIs ignore this)
        TextureManager::getSingleton().setDefaultNumMipMaps(5);

        // Create the scene
        createScene();

        createFrameListener();

        return true;

    }
    /** Configures the application - returns false if the user chooses to abandon configuration. */
    virtual bool configure(void)
    {
        // Show the configuration dialog and initialise the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
        if(mRoot->showConfigDialog())
        {
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
            mWindow = mRoot->initialise(true);
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void chooseSceneManager(void)
    {
        // Get the SceneManager, in this case a generic one
        mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_CLOSE);
    }
    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Position it 
        mCamera->setPosition(Vector3(120,10,120));
        // Look back along -Z
        mCamera->lookAt(Vector3(0,0,-300));
        mCamera->setNearClipDistance(CAMERA_NEAR_CLIP);
		mCamera->setFOVy(CAMERA_FOV);

    }
    virtual void createFrameListener(void)
    {
        mFrameListener= new OgrianFrameListener(mWindow, mCamera);
        mFrameListener->showDebugOverlay(true);
        mRoot->addFrameListener(mFrameListener);
    }

    // Just override the mandatory create scene method
    void createScene(void)
    {
		OgrianPhysics::getSingleton().test();

        Entity *waterEntity;
        Plane waterPlane;
      
        // create a water plane/scene node
        waterPlane.normal = Vector3::UNIT_Y; 
        waterPlane.d = -1.5; 
        MeshManager::getSingleton().createPlane(
            "WaterPlane",
            waterPlane,
            2800, 2800,
            20, 20,
            true, 1, 
            80, 80,
            Vector3::UNIT_Z
        );

        waterEntity = mSceneMgr->createEntity("water", "WaterPlane"); 
        waterEntity->setMaterialName("Ogrian/Ocean"); 

        SceneNode *waterNode = 
            mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode"); 
        waterNode->attachObject(waterEntity); 
        waterNode->translate(0, 0, 0);
		
        Entity *bedEntity;
        Plane bedPlane;
      
        // create a water plane/scene node
        bedPlane.normal = Vector3::UNIT_Y; 
        bedPlane.d = -1.5; 
        MeshManager::getSingleton().createPlane(
            "BedPlane",
            bedPlane,
            2800, 2800,
            20, 20,
            true, 1, 
            100, 100,
            Vector3::UNIT_Z
        );

        bedEntity = mSceneMgr->createEntity("bed", "BedPlane"); 
        bedEntity->setMaterialName("Ogrian/OceanBed"); 

        SceneNode *bedNode = 
            mSceneMgr->getRootSceneNode()->createChildSceneNode("BedNode"); 
        bedNode->attachObject(bedEntity); 
        bedNode->translate(0, -1, 0);

        // Set ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setPosition(20,80,50);

		// set up the terrain
        mSceneMgr -> setWorldGeometry( "terrain.cfg" );
		OgrianHeightMap::getSingleton().loadTerrain("terrain.cfg");

		// Define the required skyplane
        Plane plane;
        // num of world units from the camera
        plane.d = SKYPLANE_DISTANCE;
        // Above the camera, facing down
        plane.normal = -Vector3::UNIT_Y;
        // Create the plane 1000 units wide, tile the texture 3 times
        mSceneMgr->setSkyPlane(true, plane, "Ogrian/CloudySky",1000,300, false, 0.5f);

        mSceneMgr->setFog( FOG_EXP2, ColourValue::White, FOG_DENSITY, 2500,  5500 );
    }

    virtual void createViewports(void)
    {
        // Create one viewport, entire window
        Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0,0,0));
    }

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(void)
    {
        // Load resource paths from config file
        ConfigFile cf;
        cf.load("resources.cfg");

        // Go through all settings in the file
        ConfigFile::SettingsIterator i = cf.getSettingsIterator();

        String typeName, archName;
        while (i.hasMoreElements())
        {
            typeName = i.peekNextKey();
            archName = i.getNext();
            ResourceManager::addCommonArchiveEx( archName, typeName );
        }
    }



};


#endif
