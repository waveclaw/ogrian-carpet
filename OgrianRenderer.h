

#ifndef __OgrianRenderer_H__
#define __OgrianRenderer_H__

#include "Ogre.h"
#include "OgrianFrameListener.h"

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

	SceneManager* getSceneManager(void);

	Vector3 getCameraPos(void);

	static Renderer& getSingleton(void);

protected:
    Root *mRoot;
    Camera* mCamera;
    SceneManager* mSceneMgr;
    OgrianFrameListener* mFrameListener;
    RenderWindow* mWindow;
    SceneNode* mFountainNode;

    /// Standard constructor
    Renderer();

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    virtual bool setup(void);
    /** Configures the application - returns false if the user chooses to abandon configuration. */
    virtual bool configure(void);

    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);

	// Create a sky using the specified material
	virtual void createSky(const String& material);

	// create an ocean using the specified material
	virtual void createOcean(const String& material);

	// create random foilage using the specified material
	virtual void createFoliage(const String& material, int num);

    // Just override the mandatory create scene method
    void createScene(void);

    virtual void createViewports(void);

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(void);
};

}
#endif
