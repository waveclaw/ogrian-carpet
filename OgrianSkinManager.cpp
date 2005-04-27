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

Description: A centralized Skin Manager for wizard skins and castle skins
/*------------------------------------*/

#include "OgrianSkinManager.h"


template<> Ogrian::SkinManager * Singleton< Ogrian::SkinManager >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

SkinManager::SkinManager()
{
	loadWizardSkins();
	loadCastleSkins();
	loadShrineSkins();
}

//----------------------------------------------------------------------------

SkinManager::~SkinManager()
{

}

//----------------------------------------------------------------------------

int SkinManager::numWizardSkins()
{
	return (int)mWizardSkins.size()-1;
}

//----------------------------------------------------------------------------

int SkinManager::numCastleSkins()
{
	return (int)mKeepSkins.size()-1;
}

//----------------------------------------------------------------------------

int SkinManager::numShrineSkins()
{
	return (int)mShrineSkins.size()-1;
}

//----------------------------------------------------------------------------

void SkinManager::loadCastleSkins()
{
	// copied from OgreConfigFile.cpp //

	String filename = "castleskins.txt";
	String separators = " ";

    /* Open the configuration file */
    std::ifstream fp(filename.c_str());
    if(!fp)
        Except(
            Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!", "ConfigFile::load" );
    
    /* Process the file line for line */
    String line, optName, optVal;
    while (std::getline(fp, line))
    {
        StringUtil::trim(line);
        /* Ignore comments & blanks */
        if (line.length() > 0 && line.at(0) != '#' && line.at(0) != '@')
        {
            /* Find the first seperator character and split the string there */
            int separator_pos = (int)line.find_first_of(separators, 0);
            if (separator_pos != std::string::npos)
            {
                optName = line.substr(0, separator_pos);
                /* Find the first non-seperator character following the name */
                int nonseparator_pos = (int)line.find_first_not_of(separators, separator_pos);
                /* ... and extract the value */
                optVal = line.substr(nonseparator_pos);
              
				mKeepSkins.push_back(optName);
				mTowerSkins.push_back(optVal);
            }
        }
    }
}
//----------------------------------------------------------------------------

void SkinManager::loadShrineSkins()
{
	String filename = "shrineskins.txt";

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

				mShrineSkins.push_back(optName);
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

void SkinManager::loadWizardSkins()
{
	String filename = "wizardskins.txt";

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

				mWizardSkins.push_back(optName);
			}
		}
		ret = fgets(rec, 100, fp);
	}

	fclose(fp);
}

//----------------------------------------------------------------------------

String SkinManager::getRawWizardSkin(int skin)
{
	if (skin >= (int)mWizardSkins.size()) skin = 0;

	return mWizardSkins[skin];
}

//----------------------------------------------------------------------------

String SkinManager::getRawCastleSkin(int skin)
{
	if (skin >= (int)mKeepSkins.size()) skin = 0;

	return mKeepSkins[skin] + " " + mTowerSkins[skin];
}

//----------------------------------------------------------------------------

String SkinManager::getWizardSkin(int skin)
{
	if (skin >= (int)mWizardSkins.size()) skin = 0;

	return String("Ogrian/") + mWizardSkins[skin] + "/";
}

//----------------------------------------------------------------------------

String SkinManager::getKeepSkin(int skin)
{
	if (skin >= (int)mKeepSkins.size()) skin = 0;

	return mKeepSkins[skin] + ".mesh";
}

//----------------------------------------------------------------------------

String SkinManager::getTowerSkin(int skin)
{
	if (skin >= (int)mTowerSkins.size()) skin = 0;

	return mTowerSkins[skin] + ".mesh";
}

//----------------------------------------------------------------------------

String SkinManager::getShrineSkin(int skin)
{
	if (skin >= (int)mShrineSkins.size()) skin = 0;

	return mShrineSkins[skin] + ".mesh";
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