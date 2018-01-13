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
OgrianBuildingHeightMap.h
Original Author: Mike Prosser
Additional Authors: 

Description: the BuildingHeightMap is used to determine the height of the terrain
at an arbitrary Real position. It adds buildings onto the heightmap for good wizard movment.
It is a Singleton.

This is basically a carbon copy of HeightMap with added code for buildings. It would be best 
to make this a subclass of HeightMap, but because they are Singletons, subclassing
is... difficult. 

 *------------------------------------*/

#ifndef __OgrianBuildingHeightMap_H__
#define __OgrianBuildingHeightMap_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include <OgreImage.h>
#include <OgreConfigFile.h>

#include "OgrianThing.h"
#include "OgrianHeightMap.h"
#include "OgrianConst.h"
#include "OgreException.h"


using namespace Ogre;

namespace Ogrian
{

class BuildingHeightMap : public Singleton< BuildingHeightMap >
{
public:
	BuildingHeightMap();
	virtual ~BuildingHeightMap();

	// apply a building to the landscape
	void moldLandscape(Thing* building);

	// reset the landscape 
	void unmoldLandscape(Thing* building);

	// align a position vector to the array
	Vector3 alignPosition(Vector3 pos);

	// load in the heightmap
	void loadTerrain();

	// get the height at a Real point
	Real getHeightAt(Real x, Real z);

	// get the slope at a Real point
	Real getXSlopeAt(Real x, Real z);
	Real getZSlopeAt(Real x, Real z);

	// get the size of the world
	int getWorldSize();

	// get the distance between grid points
	Real getGridWidth();

    static BuildingHeightMap& getSingleton(void);

private:
	Vector3 mScale;
	int mSize;

	uchar* mData;
	Image* mImage;

	// do an array lookup
	inline int _getWorldHeight( int x, int z );
	inline void _setWorldHeight( int x, int z, int h );

};

}
#endif
