
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"

using namespace Ogre;

template<> OgrianRenderer * Singleton< OgrianRenderer >::ms_Singleton = 0;

/// Standard constructor
OgrianRenderer::OgrianRenderer()
{
    mFrameListener = 0;
    mRoot = 0;
}
/// Standard destructor
OgrianRenderer::~OgrianRenderer()
{
    if (mFrameListener)
        delete mFrameListener;
    if (mRoot)
        delete mRoot;
}

/// Start the example
void OgrianRenderer::go(void)
{
    if (!setup())
        return;

    mRoot->startRendering();
}

OgrianRenderer& OgrianRenderer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new OgrianRenderer();
	}
	return Singleton<OgrianRenderer>::getSingleton();
}

SceneManager* OgrianRenderer::getSceneManager(void)
{
	return mSceneMgr;
}


// These internal methods package up the stages in the startup process
/** Sets up the application - returns false if the user chooses to abandon configuration. */
bool OgrianRenderer::setup(void)
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
bool OgrianRenderer::configure(void)
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

void OgrianRenderer::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->getSceneManager(ST_EXTERIOR_CLOSE);
}
void OgrianRenderer::createCamera(void)
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
void OgrianRenderer::createFrameListener(void)
{
    mFrameListener= new OgrianFrameListener(mWindow, mCamera);
    mFrameListener->showDebugOverlay(true);
    mRoot->addFrameListener(mFrameListener);
}

// Just override the mandatory create scene method
void OgrianRenderer::createScene(void)
{
	OgrianPhysics::getSingleton().test();

        // Add a head, give it it's own node
	    SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        Entity *ent = mSceneMgr->createEntity("head", "ogrehead.mesh");
        headNode->attachObject(ent);

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

void OgrianRenderer::createViewports(void)
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));
}

/// Method which will define the source of resources (other than current folder)
void OgrianRenderer::setupResources(void)
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
