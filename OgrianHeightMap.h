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
OgrianHeightMap.h
Original Author: Mike Prosser
Additional Authors: 

Description: the HeightMap is used to determine the height of the terrain
at an arbitrary Real position. 
It is a Singleton.

/*------------------------------------*/

#ifndef __OgrianHeightMap_H__
#define __OgrianHeightMap_H__


#include <OgreSingleton.h>
#include <OgreTerrainSceneManager.h>

using namespace Ogre;

namespace Ogrian
{

class HeightMap : public Singleton< HeightMap >
{
public:
	HeightMap();
	virtual ~HeightMap();

	// load in the heightmap from a file
	void loadTerrain( const String& filename );

	// get the height at a Real point
	Real getHeightAt(Real x, Real z);

	// get the slope at a Real point
	Real getXSlopeAt(Real x, Real z);
	Real getZSlopeAt(Real x, Real z);

	// get the size of the world
	int getWorldSize();

    static HeightMap& getSingleton(void);

private:
	Vector3 mScale;
	int mSize;

	Real mScalex, mScaley, mScalez;
	const uchar* mData;

	Image mImage;

	// do an array lookup
	inline int _worldheight( int x, int z );
};

}
#endif