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
OgrianDotManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the 'dots' that are used in place of sprites
far from the camera

 *------------------------------------*/

#ifndef __OgrianDotManager_H__
#define __OgrianDotManager_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

class DotManager : public Singleton< DotManager >
{
public:
	virtual ~DotManager();
    static DotManager& getSingleton(void);

	Billboard* newDot(Vector3 pos, ColourValue colour);
	Billboard* newFoliageDot(Vector3 pos);

	void remove(Billboard* dot);
	void removeFoliage(Billboard* dot);

	void setFoliageMaterial(String material);

	void reset();

private:
	DotManager();

	BillboardSet* mDotSet;
	SceneNode* mNode;

	BillboardSet* mFoliageDotSet;
	SceneNode* mFoliageNode;

};

}
#endif
