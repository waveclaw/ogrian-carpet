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
main.cpp
Original Author: The OGRE Team
Additional Authors: Mike Prosser

Description: makes an app and makes it go

/*------------------------------------*/

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