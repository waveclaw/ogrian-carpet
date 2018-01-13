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
OgrianClock.h
Original Author: RakkarSoft
Additional Authors: Mike Prosser

Description: this is a re-do of the RakNet GetTime class to make it a proper singleton

 *------------------------------------*/

#ifndef __OgrianClock_H__
#define __OgrianClock_H__

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#include <Ogre.h>
#include <OgreSingleton.h>


using namespace Ogre;


namespace Ogrian
{

typedef long Time;

// A utility class to get a more accurate time than timeGetTime()
class Clock : public Singleton< Clock >
{
public:
	virtual ~Clock();

	// this should be called at the start of main()
	void init();

	// this should be called every frame
	void frame();

	// Call this to get the current time
	Time getTime();

    static Clock& getSingleton(void);

private:
	Clock();

    #ifdef _WIN32
		LARGE_INTEGER yo;
		LONGLONG counts;
	#else
		timeval tp, initialTime;
	#endif

	bool mInitialized;
	Time mTime;
};

}
#endif

