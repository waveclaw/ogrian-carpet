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
 * \file OgrianConst.h
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief A singleton that reads all of the constants from a text file
 **/
#ifndef __OgrianConst_H__
#define __OgrianConst_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include <OgreConfigFile.h>
#include "OgrianClock.h"

#define UNASSIGNED_PLAYER_ID -1
#define STRING_MAX_LENGTH 256
#define DEFAULT_MAX_CONNECTIONS 10
#define NO_REMOTE_CONNECTIONS 1
#define OGRIAN_DEFAULT_BUFFER_SIZE 500
#define OGRIAN_DEFAULT_SLEEP_TIME 30
#define OGRIAN_BASETIME 5000
#define OGRIAN_RANDOM_MULTIPLIER 10000

/**
 * Computes the value in the configuration file of the ineger variable \a x
 **/
#define CONI(x) ( Const::getSingleton().getConstantInt(x) )
/**
 * Computes the value in the configuration file of the Real variable \a x
 **/
#define CONR(x) ( Const::getSingleton().getConstantReal(x) )
/**
 * Computes the value in the configuration file of the Time variable \a x
 **/
#define CONT(x) ( Const::getSingleton().getConstantTime(x) )
/**
 * Computes the value in the configuration file of the String variable \a x
 **/
#define CONS(x) ( Const::getSingleton().getConstantString(x) )
/**
 * Computes the value in the configuration file of the bool variable \a x
 **/
#define CONB(x) ( Const::getSingleton().getConstantBool(x) )

using namespace Ogre;

namespace Ogrian
{
/*! \class Const OgrianConst.h "include/OgrianConst.h"
 * \brief A reader of configuration files which create constants.
 **/
class Const : public Singleton< Const >
{
public:
	/**
 	* The destructor for cleaning up the configuration object.
 	*/
	virtual ~Const();

	/**
	 * Implement Singlton.
	 * Per the singleton pattern, create a Const object if does not exist or return
	 * the current one.
	 * @return Address of the current Const object in memory.
	 */	
    static Const& getSingleton(void);
	
	/**
	 * Aquire the given value from the configration file(s) as an integer.
	 * @param The key for the value.
	 * @return The integer value of the key provided.
	 **/
	int getConstantInt(const String &key);
	
	/**
	 * Aquire the given value from the configration file(s) as a Real.
	 * @param The key for the value.
	 * @return The value of the key provided as a Real.
	 **/
	Real getConstantReal(const String &key);

	/**
	 * Aquire the given value from the configration file(s) as a Time object.
	 * @param The key for the value.
	 * @return The Time.
	 **/
	Time getConstantTime(const String &key);
	
	/**
	 * Aquire the given value from the configration file(s) as a String.
	 * @param The key for the value.
	 * @return The String of the key provided.
	 **/
	String getConstantString(const String &key);
	
	/**
	 * Aquire the given value from the configration file(s) as a boolean.
	 * @param The key for the value.
	 * @return True or False: the boolean value of the key provided.
	 **/
	bool getConstantBool(const String &key);
	

private:
	/**
	 * The private constructor.
	 * Initializes the Confuration system.   
	 **/
	Const();

	/**
	 * The name for the third configration file.
	 * Currently this is "constants.cfg"
	 **/
	ConfigFile mConstants; 
	
	/**
	 * The name for the third configration file.
	 * Currently this is "config.txt"
	 **/
	ConfigFile mConfig; 
	
	/**
	 * The name for the third configration file.
	 * Currently this is "strings.txt"
	 **/
	ConfigFile mStrings;
};

} // end namespace ogrian

#endif
