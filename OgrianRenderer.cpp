
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"
#include "OgrianManaEntity.h"

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
    mCamera->setPosition(Vector3(0,0,10));
    // Look back along -Z
    mCamera->lookAt(Vector3(300,0,300));
    mCamera->setNearClipDistance(CAMERA_NEAR_CLIP);
	mCamera->setFOVy(CAMERA_FOV);

}
void Renderer::createFrameListener(void)
{
    mFrameListener= new OgrianFrameListener(mWindow, mCamera);
    mFrameListener->showDebugOverlay(true);
    mRoot->addFrameListener(mFrameListener);
}
	
Vector3 Renderer::getCameraPos(void)
{
	return mCamera->getPosition();
}

// Just override the mandatory create scene method
void Renderer::createScene(void)
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
    bedNode->translate(0, -.1, 0);

    // Set ambient light
    mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// set up the terrain
    mSceneMgr->setWorldGeometry( "world.cfg" );
	HeightMap::getSingleton().loadTerrain("world.cfg");

	// Define the required skyplane
    Plane plane;
    // num of world units from the camera
    plane.d = SKYPLANE_DISTANCE;
    // Above the camera, facing down
    plane.normal = -Vector3::UNIT_Y;
    // Create the plane 1000 units wide, tile the texture 3 times
    mSceneMgr->setSkyPlane(true, plane, "Ogrian/CloudySky",1000,300, true, SKYPLANE_BOW);

    mSceneMgr->setFog( FOG_EXP2, ColourValue::White, FOG_DENSITY, 2500,  5500 );

	// set up some foliage
	/*SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	BillboardSet* grassSet = sceneMgr->createBillboardSet("Foliage",1);
	grassSet->setBillboardType(BBT_ORIENTED_COMMON);
	grassSet->setCommonDirection(Vector3::UNIT_Y);
	grassSet->setMaterialName("Ogrian/Grass");

	int i=0;
	while (i<100)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * 500.0;
        Real z = Math::SymmetricRandom() * 500.0;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > 5)
		{
			i++;
			//Billboard* billboard = grassSet->createBillboard(x, y+5, z);
			//billboard->setDimensions(10,10);
			ManaEntity* e = new ManaEntity("Ogrian/Mana", 1);
			e->setPosition(x,y+5,z);
			e->setScale(10);
			Physics::getSingleton().addPhysicalEntity(e);
		}
	}

	sceneMgr->getRootSceneNode()->attachObject(grassSet);


	//toss in some smoke
	// Create shared node for 2 fountains
    mFountainNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());

    // smoke
    ParticleSystem* pSys2 = ParticleSystemManager::getSingleton().createSystem("fountain1", 
        "Examples/Smoke");
    // Point the fountain at an angle
    SceneNode* fNode = static_cast<SceneNode*>(mFountainNode->createChild());
    fNode->attachObject(pSys2);
	*/
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