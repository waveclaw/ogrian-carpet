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
OgrianSkinManager.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: A centralized Skin Manager
/*------------------------------------*/

#include "OgrianSkinManager.h"


template<> Ogrian::SkinManager * Singleton< Ogrian::SkinManager >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

SkinManager::SkinManager()
{
	loadSkins();
}

//----------------------------------------------------------------------------

SkinManager::~SkinManager()
{

}

//----------------------------------------------------------------------------

int SkinManager::numSkins()
{
	return (int)mSkins.size()-1;
}

//----------------------------------------------------------------------------

void SkinManager::loadSkins()
{
	String filename = "skins.txt";

	FILE *fp;
	char rec[100], *ret;
	String optName, optVal;

	// Open and parse entire file
	fp = fopen(filename.c_str(), "r");
	if( !fp )
		Except(
			Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!", "SkinManager::loadSkins" );

	ret = fgets(rec, 100, fp);
	while (ret != NULL)
	{
		String tst = rec;
		StringUtil::trim(tst);
		// Ignore comments & blanks
		if (tst.length() > 0 && tst.at(0) != '#' && tst.at(0) != '@' && tst.at(0) != '\n')
		{
			// Tokenise on newline
			char* pName = strtok(rec, "\n");
			if (pName)
			{
				String optName = pName;
				StringUtil::trim(optName);

				mSkins.push_back(optName);
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

String SkinManager::getSkin(int skin)
{
	return String("Ogrian/") + mSkins[skin] + "/";
}

//----------------------------------------------------------------------------

SkinManager& SkinManager::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new SkinManager();
	}
    return Singleton<SkinManager>::getSingleton();
}

//----------------------------------------------------------------------------

}