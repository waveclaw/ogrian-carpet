
#ifndef __OgrianApplication_H__
#define __OgrianApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"

using namespace Ogre;


class OgrianApplication
{
public:
    /// Start the application
    virtual void go(void)
    {
		OgrianRenderer::getSingleton().go();
    }

};

#endif
