//this file is used to bypass files that prevent compling due to out of date dependancies.
//Ogre::Vector3 defined in <Ogre.h>

#include "../Patches/fake.hpp"

template<> Ogrian::Renderer * Singleton< Ogrian::Renderer >::ms_Singleton = 0;

namespace Ogrian {

Renderer& Renderer::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Renderer(); return Singleton<Renderer>::getSingleton(); }
Renderer::Renderer() {}


CameraThing *Renderer::getCameraThing() { return new CameraThing();}
Vector3 CameraThing::getDirection() { return Vector3(0.0f, 0.0f, 0.0f);}
Vector3 CameraThing::getVelocity() { return Vector3(0.0f, 0.0f, 0.0f);}
Vector3 CameraThing::getPosition() { return Vector3(0.0f, 0.0f, 0.0f);}

}
