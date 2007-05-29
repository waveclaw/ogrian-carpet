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

/**
 * \file OgrianConst.cpp
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief A singleton that reads all of the constants from a text file
 **/
#include "OgrianConst.h"

using namespace Ogre;

/**
 * The reference to the Ogre Const. 
 **/
template<> Ogrian::Const * Singleton< Ogrian::Const >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

/**
 * The private constructor.
 * Initializes the Confuration system.   
 **/
Const::Const()
{
	mFile1.load("constants.cfg");
	mFile2.load("config.cfg");
	mFile3.load("strings.txt");
}

//----------------------------------------------------------------------------

Const::~Const()
{

}

//----------------------------------------------------------------------------

/**
 * Aquire the given value from the configration file(s) as an integer.
 * @param The key for the value.
 * @return The integer value of the key provided.
 **/
int Const::getConstantInt(const String &key)
{
	int c = atoi(mFile1.getSetting(key).c_str());
	if (c == 0.0) c = atoi(mFile2.getSetting(key).c_str());
	if (c == 0.0) 
		LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return c;
}

//----------------------------------------------------------------------------

/**
 * Aquire the given value from the configration file(s) as a Real.
 * @param The key for the value.
 * @return The value of the key provided as a Real.
 **/
Real Const::getConstantReal(const String &key)
{
	Real c = atof(mFile1.getSetting(key).c_str());
	if (c == 0.0) c = atof(mFile2.getSetting(key).c_str());
	if (c == 0.0) 
		LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return c;
}

//----------------------------------------------------------------------------

/**
 * Aquire the given value from the configration file(s) as a Time object.
 * @param The key for the value.
 * @return The Time.
 **/
Time Const::getConstantTime(const String &key)
{
	Real c = atof(mFile1.getSetting(key).c_str());
	if (c == 0.0) c = atof(mFile2.getSetting(key).c_str());
	if (c == 0.0) 
		LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return (Time) c*1000; /* re: see OgrianCock.h line 49 for definition - jdpowell */
}

//----------------------------------------------------------------------------

/**
 * Aquire the given value from the configration file(s) as a String.
 * @param The key for the value.
 * @return The String of the key provided.
 **/
String Const::getConstantString(const String &key)
{
	// only read config and strings
	String c = mFile2.getSetting(key).c_str();
	if (c == String("")) c = mFile3.getSetting(key).c_str();
	if (c == String("")) 
		LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");
	return c;
}

//----------------------------------------------------------------------------

/**
 * Aquire the given value from the configration file(s) as a boolean.
 * @param The key for the value.
 * @return True or False: the boolean value of the key provided.
 **/
bool Const::getConstantBool(const String &key)
{
	String c = mFile1.getSetting(key).c_str();
	if (c == String("")) c = mFile2.getSetting(key).c_str();
	if (c == String("")) 
		LogManager::getSingleton().logMessage(String("Warning, key ") + key + " read as 0.0");

	if (c == String("true") || c == String("yes") || c == String("on")) return true;
	return false;
}

//----------------------------------------------------------------------------

/**
 * Implement Singlton.
 * Per the singleton pattern, create a Const object if does not exist or return
 * the current one.
 * @return Address of the current Const object in memory.
 */	
Const& Const::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Const();
	}
    return Singleton<Const>::getSingleton();
}


}
