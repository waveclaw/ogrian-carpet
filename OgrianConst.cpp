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
OgrianConst.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: A singleton that reads all of the constants from a text file

/*------------------------------------*/


#include "OgrianConst.h"

using namespace Ogre;

template<> Ogrian::Const * Singleton< Ogrian::Const >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

Const::Const()
{
	mFile1.load("constants.txt");
	mFile2.load("config.txt");
}

//----------------------------------------------------------------------------

Const::~Const()
{

}

//----------------------------------------------------------------------------

int Const::getConstantInt(const String &key)
{
	int c = atoi(mFile1.getSetting(key).c_str());
	if (c == 0.0) c = atoi(mFile2.getSetting(key).c_str());
	if (c == 0.0) LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return c;
}

//----------------------------------------------------------------------------

Real Const::getConstantReal(const String &key)
{
	Real c = atof(mFile1.getSetting(key).c_str());
	if (c == 0.0) c = atof(mFile2.getSetting(key).c_str());
	if (c == 0.0) LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return c;
}

//----------------------------------------------------------------------------

Const& Const::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Const();
	}
    return Singleton<Const>::getSingleton();
}


}
