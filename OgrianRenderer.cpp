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
OgrianRenderer.cpp
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: This is the main interface with the Ogre rendering Engine. 
It also handles all scene creation. 
Later, Thing placing will be moved to a WorldMaker class. 
It is a Singleton.

/*------------------------------------*/


#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianFoliageThing.h"
#include "OgrianManaThing.h"

using namespace Ogre;

template<> Ogrian::Renderer * Singleton< Ogrian::Renderer >::ms_Singleton = 0;
namespace Ogrian
{

/// Standard constructor
Renderer::Renderer()
{
    mFrameListener = 0;
    mRoot = 0;
}
/// Standard destructor
Renderer::~Renderer()
{
    if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;
}

/// Start the example
void Renderer::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();
}

Renderer& Renderer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Renderer();
	}
	return Singleton<Renderer>::getSingleton();
}

SceneManager* Renderer::getSceneManager(void)
{
	return mSceneMgr;
}


// These internal methods package up the stages in the startup process
/** Sets up the application - returns false if the user chooses to abandon configuration. */
bool Renderer::setup(void)
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
bool Renderer::configure(void)
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

void Renderer::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_FAR);
}
void Renderer::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it 
    mCamera->setPosition(Vector3(100,0,100));
    // Look back along -Z
    mCamera->lookAt(Vector3(300,0,300));
    mCamera->setNearClipDistance(CAMERA_NEAR_CLIP);
	mCamera->setFOVy(CAMERA_FOV);

	// make the camera thing
	mCameraThing = new CameraThing(mCamera);
	Physics::getSingleton().addThing(mCameraThing);
	mCameraThing->setPosition(Vector3(100,0,100));

}
void Renderer::createFrameListener(void)
{
    mFrameListener= new OgrianFrameListener(mWindow, mCamera);
    mFrameListener->showDebugOverlay(true);
    mRoot->addFrameListener(mFrameListener);
}
	
CameraThing* Renderer::getCameraThing(void)
{
	return mCameraThing;
}

void Renderer::createSky(const String& material)
{
	// Define the required skyplane
    Plane plane;
    // num of world units from the camera
    plane.d = SKYPLANE_DISTANCE;
    // Above the camera, facing down
    plane.normal = -Vector3::UNIT_Y;
    // Create the plane 1000 units wide, tile the texture 3 times
    mSceneMgr->setSkyPlane(true, plane, material,1000,300, true, SKYPLANE_BOW);
}

void Renderer::createOcean(const String& material)
{
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
    waterEntity->setMaterialName(material); 

    SceneNode *waterNode = 
        mSceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode"); 
    waterNode->attachObject(waterEntity); 
    waterNode->translate(0, 0, 0);
}

void Renderer::createFoliage(const String& material, int num)
{
	// set up some foliage
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

	int i=0;
	//Real x = 0;
	while (i<num)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * 1000.0;
        Real z = Math::SymmetricRandom() * 1000.0;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > FOLIAGE_LINE_MIN && y < FOLIAGE_LINE_MAX)
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			Real scale = FOLIAGE_SCALE + (Math::SymmetricRandom()-.5) * FOLIAGE_SCALE_VAR;
			FoliageThing* fol = new FoliageThing(scale,pos);
			Physics::getSingleton().addThing(fol);
		}
	}

}

// Just override the mandatory create scene method
void Renderer::createScene(void)
{
	String filename = "world.cfg";

	/* Set up the options */
	ConfigFile config;
	config.load( filename );
	String skyMaterial = config.getSetting( "SkyMaterial" );
	String oceanMaterial = config.getSetting( "OceanMaterial" );
	String foliageMaterial = config.getSetting( "FoliageMaterial" );

	// set up the terrain
    mSceneMgr->setWorldGeometry( filename );
	HeightMap::getSingleton().loadTerrain(filename);
	createSky(skyMaterial);
	createOcean(oceanMaterial);
	createFoliage(foliageMaterial, FOLIAGE_NUM);

    // Set ambient light
    mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	// set the fog
    mSceneMgr->setFog( FOG_EXP2, ColourValue::White, FOG_DENSITY, 2500,  5500 );

	// toss in some smoke
	// Create shared node for 2 fountains
    mFountainNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());

    // smoke
    ParticleSystem* pSys2 = ParticleSystemManager::getSingleton().createSystem("fountain1", 
        "Examples/Smoke");
    // Point the fountain at an angle
    SceneNode* fNode = static_cast<SceneNode*>(mFountainNode->createChild());
    fNode->attachObject(pSys2);
	fNode->setPosition(315,50,862);
}

void Renderer::createViewports(void)
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
}

/// Method which will define the source of resources (other than current folder)
void Renderer::setupResources(void)
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

}