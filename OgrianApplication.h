
#ifndef __OgrianApplication_H__
#define __OgrianApplication_H__

#include "Ogre.h"
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

class OgrianApplication
{
public:
    /// Start the application
    virtual void go(void)
    {
		Renderer::getSingleton().go();
    }

};

}
#endif
