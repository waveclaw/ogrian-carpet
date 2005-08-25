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
OgrianConst.h
Original Author: Mike Prosser
Additional Authors: 

Description: A singleton that reads all of the constants from a text file

 *------------------------------------*/


#ifndef __OgrianConst_H__
#define __OgrianConst_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include <OgreConfigFile.h>
#include "OgrianClock.h"

#define CONI(x) Const::getSingleton().getConstantInt(x)
#define CONR(x) Const::getSingleton().getConstantReal(x)
#define CONT(x) Const::getSingleton().getConstantTime(x)
#define CONS(x) Const::getSingleton().getConstantString(x)
#define CONB(x) Const::getSingleton().getConstantBool(x)

using namespace Ogre;

namespace Ogrian
{

class Const : public Singleton< Const >
{
public:
	virtual ~Const();
    static Const& getSingleton(void);

	int getConstantInt(const String &key);
	Real getConstantReal(const String &key);
	Time getConstantTime(const String &key);
	String getConstantString(const String &key);
	bool getConstantBool(const String &key);

private:
	Const();

	ConfigFile mFile1; // constants.txt
	
	ConfigFile mFile2; // config.txt
	
	ConfigFile mFile3; // strings.txt
};

}

#endif
