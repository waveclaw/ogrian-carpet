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
OgrianSkinManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: A central manager for skins
/*------------------------------------*/

#ifndef __OgrianOgrianSkinManager_H__
#define __OgrianOgrianSkinManager_H__

#include <Ogre.h>

using namespace Ogre;

namespace Ogrian
{

class SkinManager : public Singleton< SkinManager >
{
public:
	virtual ~SkinManager();

	static SkinManager& getSingleton(void);

	String getWizardSkin(int skin);
	String getKeepSkin(int skin);
	String getTowerSkin(int skin);
	String getShrineSkin(int skin);

	int numWizardSkins();
	int numCastleSkins();
	int numShrineSkins();

	// neede by the menu for reverse lookups
	String getRawWizardSkin(int skin);
	String getRawCastleSkin(int skin);

private:
	SkinManager();
	
	void loadWizardSkins();
	void loadCastleSkins();
	void loadShrineSkins();

	std::vector<String> mWizardSkins;
	std::vector<String> mKeepSkins;
	std::vector<String> mTowerSkins;
	std::vector<String> mShrineSkins;
};

}

#endif