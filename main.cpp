#include "Ogre.h"
#include "OgrianApplication.h"
#include <windows.h>

using namespace Ogrian;

INT WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,INT) 
{
	OgrianApplication theApp;		// Instanciate our subclass
	try {			// This try-catch provides a nice way of popping up errors if they occur.
		theApp.go();	// go
	} catch (Ogre::Exception& e) {
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
}