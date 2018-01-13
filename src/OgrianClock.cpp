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

#include "OgrianClock.h"

using namespace Ogre;

template<> Ogrian::Clock * Singleton< Ogrian::Clock >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

Clock::Clock()
{
	mInitialized=false;
	mTime = 0;
}

//----------------------------------------------------------------------------

Clock::~Clock()
{
	
}

//----------------------------------------------------------------------------

Time Clock::getTime()
{
	return mTime;
}

//----------------------------------------------------------------------------

void Clock::init()
{
	#ifdef _WIN32
		QueryPerformanceFrequency(&yo);
		counts = yo.QuadPart / 1000;
	#else
		gettimeofday(&initialTime, 0); 
	#endif

	mInitialized=true; 
}

//----------------------------------------------------------------------------

void Clock::frame()
{
	if (mInitialized==false) return;

	#ifdef _WIN32
		LARGE_INTEGER PerfVal;
		QueryPerformanceCounter(&PerfVal);
		mTime = (Time)(PerfVal.QuadPart/counts);
	#else
		gettimeofday(&tp, 0);
		// Seconds to ms and microseconds to ms
		mTime = (tp.tv_sec - initialTime.tv_sec) * 1000 + (tp.tv_usec - initialTime.tv_usec) / 1000;
	#endif
}

//----------------------------------------------------------------------------

Clock& Clock::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Clock();
	}
    return Singleton<Clock>::getSingleton();
}

//----------------------------------------------------------------------------

}
