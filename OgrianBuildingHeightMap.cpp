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
OgrianBuildingHeightMap.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the BuildingHeightMap is used to determine the height of the terrain
at an arbitrary Real position.
It is a Singleton.

/*------------------------------------*/


#include "Ogre.h"
#include "OgrianHeightMap.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianConst.h"
#include <OgreImage.h>
#include <OgreConfigFile.h>
#include "OgreException.h"

using namespace Ogre;

template<> Ogrian::BuildingHeightMap * Singleton< Ogrian::BuildingHeightMap >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

// null the data pointer
BuildingHeightMap::BuildingHeightMap()
{
	mData = 0;
	mImage = 0;
}

//----------------------------------------------------------------------------

// clean up
BuildingHeightMap::~BuildingHeightMap()
{
	if (mImage) delete mImage;

	// mData is a pointer into mImage
}

//----------------------------------------------------------------------------

Real BuildingHeightMap::getGridWidth()
{
	return mScale.x;
}

//----------------------------------------------------------------------------

Vector3 BuildingHeightMap::alignPosition(Vector3 pos)
{
	// scale the coordinates
	pos.x /= mScale.x;
	pos.z /= mScale.z;

	// calculate the matrix indeces for the grid cell
	int fx = int(pos.x);
	int fz = int(pos.z);

	pos.x = ((Real)fx + 0.5) * mScale.x;
	pos.z = ((Real)fz + 0.5) * mScale.z;

	return pos;
}

//----------------------------------------------------------------------------

int BuildingHeightMap::getWorldSize()
{
	return mSize * mScale.x;
}

//----------------------------------------------------------------------------

// do a lookup in the array to find the height at a grid point
int BuildingHeightMap::_getWorldHeight( int x, int z )
{
	Real min = CONR("HEIGTHMAP_MIN_HEIGHT");

	if (x <= 0) return min;
	if (z <= 0) return min;
	if (x >= mSize-1) return min;
	if (z >= mSize-1) return min;

	if (!mData) return min;

    int height = mData[ ( ( z * mSize ) + x ) ];

	if (height < min) return min;
	return height;
};

//----------------------------------------------------------------------------

// do a lookup in the array to set the height at a grid point
void BuildingHeightMap::_setWorldHeight( int x, int z, int height )
{
	int min = HeightMap::getSingleton()._worldheight(x, z);
	if (height < min) height = min;
	if (height > 255) height = 255;

	if (x <= 0) return;
	if (z <= 0) return;
	if (x >= mSize-1) return;
	if (z >= mSize-1) return;

	if (!mData) return;

    mData[ ( ( z * mSize ) + x ) ] = height;
};

//----------------------------------------------------------------------------

// get the height at any point
Real BuildingHeightMap::getHeightAt(Real x, Real z)
{
	// This interpolates between the four corners

	// scale the coordinates
	x /= mScale.x;
	z /= mScale.z;

	// calculate the matrix indeces for the grid cell
	int fx = int(x);
	int fz = int(z);

	// calculate the relative location within the cell
	Real modx = (x - fx);
	Real modz = (z - fz);

	// calculate the height at each corner of the cell
	Real height00 = Real(_getWorldHeight(fx  ,fz))   * mScale.y;
	Real height01 = Real(_getWorldHeight(fx  ,fz+1)) * mScale.y;
	Real height10 = Real(_getWorldHeight(fx+1,fz))   * mScale.y;
	Real height11 = Real(_getWorldHeight(fx+1,fz+1)) * mScale.y;

	// calculate the weighted average
	Real height = height00*(1-modx)*(1-modz) 
					+ height01*(1-modx)*(modz) 
					+ height10*(modx)*(1-modz) 
					+ height11*(modx)*(modz);

	return height*2;
}

//----------------------------------------------------------------------------

void BuildingHeightMap::moldLandscape(Thing* building)
{
	// get the coordinates
	Vector3 pos = building->getPosition();
	Real x = pos.x;
	Real y = pos.y + building->getHeight()/2;
	Real z = pos.z;

	// scale the coordinates
	x /= mScale.x;
	y /= mScale.y;
	z /= mScale.z;

	y++;
	y /= 2;

	// calculate the matrix indeces for the grid cell
	int fx = int(x);
	int fz = int(z);

	int width = building->getWidth() / mScale.x;
	if (fx - width - 1 <= 0) fx = width + 2;
	if (fx + width + 2 >= mSize - 1) fx = mSize - width - 1;
	if (fz - width - 1 <= 0) fz = width + 2;
	if (fz + width + 2 >= mSize - 1) fz = mSize - width - 1;

	//std::ostringstream msg("setting height: ");
	//msg << "(" << fx << ", " << y << ", " << fz << ")";
	//LogManager::getSingleton().logMessage(msg.str());

	// set the height at each grid point covered by the building
	int rowf = fx - width +1; // the first row
	int rowl = fx + width ; // the last row
	int colf = fz - width +1; // the first col
	int coll = fz + width ; // the last col
	for (int i=rowf; i<=rowl; i++)
		for (int j=colf; j<=coll; j++)
			_setWorldHeight(i, j, y);
}

//----------------------------------------------------------------------------

void BuildingHeightMap::unmoldLandscape(Thing* building)
{
	// get the coordinates
	Vector3 pos = building->getPosition();
	Real x = pos.x;
	Real z = pos.z;

	// scale the coordinates
	x /= mScale.x;
	z /= mScale.z;

	// calculate the matrix indeces for the grid cell
	int fx = int(x);
	int fz = int(z);

	int width = building->getWidth() / mScale.x;
	if (fx - width - 1 <= 0) fx = width + 2;
	if (fx + width + 2 >= mSize - 1) fx = mSize - width - 1;
	if (fz - width - 1 <= 0) fz = width + 2;
	if (fz + width + 2 >= mSize - 1) fz = mSize - width - 1;

	//std::ostringstream msg("setting height: ");
	//msg << "(" << fx << ", " << y << ", " << fz << ")";
	//LogManager::getSingleton().logMessage(msg.str());

	// set the height at each grid point covered by the building
	int rowf = fx - width +1; // the first row
	int rowl = fx + width ; // the last row
	int colf = fz - width +1; // the first col
	int coll = fz + width ; // the last col
	for (int i=rowf; i<=rowl; i++)
		for (int j=colf; j<=coll; j++)
			_setWorldHeight(i, j, 0);
}

//----------------------------------------------------------------------------

// return the height difference between this point and a close other point
Real BuildingHeightMap::getXSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x+CONR("HEIGTHMAP_SLOPE_DIFF"),z);
}

//----------------------------------------------------------------------------

// return the height difference between this point and a close other point
Real BuildingHeightMap::getZSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x,z+CONR("HEIGTHMAP_SLOPE_DIFF"));
}

//----------------------------------------------------------------------------

// load the array from the image file
void BuildingHeightMap::loadTerrain()
{
	// make the data
	if (mImage) delete mImage;
	mImage = new Image(*(HeightMap::getSingleton().getImage()));

	mData = mImage->getData();
	mSize = mImage->getWidth();

	mScale = HeightMap::getSingleton().getScale();
}

//----------------------------------------------------------------------------

BuildingHeightMap& BuildingHeightMap::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new BuildingHeightMap();
	}
    return Singleton<BuildingHeightMap>::getSingleton();
}

//----------------------------------------------------------------------------

}