#include "Ogre.h"
#include "OgrianApplication.h"
#include <windows.h>

// First, we declare a subclass of the OgrianApplication class
/*class OgrianApp : public OgrianApplication
{
public:
	OgrianApp() { }
};*/

INT WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,INT) {
	OgrianApplication myApp;		// Instanciate our subclass
	try {			// This try-catch provides a nice way of popping up errors if they occur.
		myApp.go();	// ExampleApplication provides a go method, which starts the rendering.
	} catch (Ogre::Exception& e) {
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
}