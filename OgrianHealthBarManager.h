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
OgrianHealthBarManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the health bars that are shown above nearby damageable things

/*------------------------------------*/

#ifndef __OgrianHealthBarManager_H__
#define __OgrianHealthBarManager_H__

#include <Ogre.h>
#include <OgreSingleton.h>

using namespace Ogre;

namespace Ogrian
{

class HealthBarManager : public Singleton< HealthBarManager >
{
public:
	virtual ~HealthBarManager();
    static HealthBarManager& getSingleton(void);

	// make a new bar
	Billboard* newBar(Vector3 pos, ColourValue colour);

	// remove a bar 
	void remove(Billboard* bar);

	// clear all bars
	void clear();

private:
	HealthBarManager();

	BillboardSet* mBarSet;
	SceneNode* mNode;

};

}
#endif