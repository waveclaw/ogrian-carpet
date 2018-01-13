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
Later, Non-Foliage world population is handled by Game 
It is a Singleton.

 *------------------------------------*/

#include "OgrianRenderer.h"

using namespace Ogre;

template<> Ogrian::Renderer * Singleton< Ogrian::Renderer >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

/// Standard constructor
Renderer::Renderer()
{
    mFrameListener = 0;
    mRoot = 0;
	mWaterNode = 0;
	mCamera = 0;
	mCameraThing = 0;

	mMapName = "";
	mMapLoaded = false;
}
//----------------------------------------------------------------------------

/// Standard destructor
Renderer::~Renderer()
{
    if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;
}

//----------------------------------------------------------------------------

/// Start the example
void Renderer::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();
}

//----------------------------------------------------------------------------

OgrianFrameListener* Renderer::getFrameListener()
{
	return mFrameListener;
}

//----------------------------------------------------------------------------

Renderer& Renderer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Renderer();
	}
	return Singleton<Renderer>::getSingleton();
}

//----------------------------------------------------------------------------

SceneManager* Renderer::getSceneManager(void)
{
	return mSceneMgr;
}

//----------------------------------------------------------------------------

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

	Menu::getSingleton().readConfig();

    return true;

}

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

void Renderer::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_CLOSE);
}

//----------------------------------------------------------------------------

void Renderer::createCamera(void)
{
	if (mCamera) return;

    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    mCamera->setNearClipDistance(CONR("CAMERA_NEAR_CLIP"));
	mCamera->setFOVy(Degree(CONR("CAMERA_FOV")));
}

//----------------------------------------------------------------------------

void Renderer::createCameraThing()
{
	// ensure there's a camera
	if (!mCamera) createCamera();

	// make the camera thing
	if (!mCameraThing)
		mCameraThing = new CameraThing(mCamera);
}
//----------------------------------------------------------------------------

void Renderer::createFrameListener(void)
{
    mFrameListener = new OgrianFrameListener(mWindow, mCamera);
    mRoot->addFrameListener(mFrameListener);
	
    mMouseFrameListener = new OgrianMouseFrameListener(mWindow);
    mRoot->addFrameListener(mMouseFrameListener);
}

//----------------------------------------------------------------------------
	
CameraThing* Renderer::getCameraThing(void)
{
	return mCameraThing;
}

//----------------------------------------------------------------------------

void Renderer::createSky(const String& material)
{
	LogManager::getSingleton().logMessage(String("Making Sky... ") += material);

    // Create the skydome
	mSceneMgr->setSkyDome(true, material,10,8,40,true);
}

//----------------------------------------------------------------------------

void Renderer::createOcean(const String& material)
{
	if (mWaterNode != 0)
	{
		mWaterEntity->setMaterialName(material);
		return;
	}

	LogManager::getSingleton().logMessage("Making Ocean...");

    Plane waterPlane;
    
    // create a water plane/scene node
    waterPlane.normal = Vector3::UNIT_Y; 
    waterPlane.d = -1.5; 
    MeshManager::getSingleton().createPlane(
        "WaterPlane",
        waterPlane,
        5000, 5000,
        20, 20,
        true, 1, 
        80, 80,
        Vector3::UNIT_Z
    );

	mWaterNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(); 
	mWaterEntity = mSceneMgr->createEntity(mWaterNode->getName(), "WaterPlane"); 
	mWaterEntity->setMaterialName(material); 

	mWaterNode->attachObject(mWaterEntity); 
	mWaterNode->translate(0, 0, 0);
}

//----------------------------------------------------------------------------

void Renderer::loadMap(String configfile, bool server)
{
	unloadMap();

	/* Set up the options */
	ConfigFile config;
	config.load( configfile );
	String skyMaterial = config.getSetting( "SkyMaterial" );
	String oceanMaterial = config.getSetting( "OceanMaterial" );
	mFoliageMaterial = config.getSetting( "FoliageMaterial" ).c_str();
	int lava = atoi(config.getSetting( "Lava" ).c_str());

	DotManager::getSingleton().setFoliageMaterial(mFoliageMaterial);

	// set the fog
	mSceneMgr->setFog(FOG_NONE);
	mSceneMgr->setAmbientLight(ColourValue::White);

	// load new terrain 
	if (configfile != mMapName)
		mSceneMgr->setWorldGeometry( configfile );

	mMapName = configfile;

	HeightMap::getSingleton().loadTerrain(configfile);
	BuildingHeightMap::getSingleton().loadTerrain();
	Physics::getSingleton().setWorldSize(HeightMap::getSingleton().getWorldSize());

	createSky(skyMaterial);
	createOcean(oceanMaterial);
	
	// set the lava
	Game::getSingleton().setLava(lava > 0);

	// start the game
	Game::getSingleton().startGame(config);

	mMapLoaded = true;
}

//----------------------------------------------------------------------------

String Renderer::getMapName()
{
	return mMapName;
}

//----------------------------------------------------------------------------

String Renderer::getFoliageMaterial()
{
	return mFoliageMaterial;
}

//----------------------------------------------------------------------------

void Renderer::unloadMap()
{
	if (!mMapLoaded) return;

	// remove all Things
	Physics::getSingleton().reset();

	mMapLoaded = false;
}

//----------------------------------------------------------------------------

// Just override the mandatory create scene method
void Renderer::createScene(void)
{
	// show the menu
	Menu::getSingleton().show();

	// show the hud
	Hud::getSingleton().show();
}

//----------------------------------------------------------------------------

// return the camera
Camera* Renderer::getCamera()
{
	return mCamera;
}

//----------------------------------------------------------------------------
void Renderer::createViewports(void)
{
	// ensure the camera exists
	createCamera();

    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
}

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

}
