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
OgrianHeightMap.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the HeightMap is used to determine the height of the terrain
at an arbitrary Real position.
It is a Singleton.

/*------------------------------------*/


#include "Ogre.h"
#include "OgrianHeightMap.h"
#include "OgrianConst.h"
#include <OgreImage.h>
#include <OgreConfigFile.h>
#include "OgreException.h"

using namespace Ogre;

template<> Ogrian::HeightMap * Singleton< Ogrian::HeightMap >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

// null the data pointer
HeightMap::HeightMap()
{
	mData = 0;
	mImage = 0;
}

//----------------------------------------------------------------------------

// clean up
HeightMap::~HeightMap()
{
	if (mImage) delete mImage;

	// mData is a pointer into mImage
}

//----------------------------------------------------------------------------

int HeightMap::getWorldSize()
{
	return mSize;
}

//----------------------------------------------------------------------------

// do a lookup in the array to find the height at a grid point
int HeightMap::_worldheight( int x, int z )
{
	Real min = CONR("HEIGTHMAP_MIN_HEIGHT");

	if (x < 0) return min;
	if (z < 0) return min;
	if (x > mSize) return min;
	if (z > mSize) return min;

	if (!mData) return min;

    int height = mData[ ( ( z * mSize ) + x ) ];

	if (height < min) return min;
	return height;
};

//----------------------------------------------------------------------------

// get the height at any point
Real HeightMap::getHeightAt(Real x, Real z)
{
	// This interpolates between the four corners

	// scale the coordinates
	x /= mScalex;
	z /= mScalez;

	// a smoothing constant
	const int c = CONR("HEIGHTMAP_SMOOTHING");

	// calculate the matrix indeces for the grid cell
	int fx = int(x/c)*c;
	int fz = int(z/c)*c;

	// calculate the relative location within the cell
	Real modx = (x - fx)/c;
	Real modz = (z - fz)/c;

	// calculate the height at each corner of the cell
	Real height00 = Real(_worldheight(fx  ,fz))   * mScaley;
	Real height01 = Real(_worldheight(fx  ,fz+c)) * mScaley;
	Real height10 = Real(_worldheight(fx+c,fz))   * mScaley;
	Real height11 = Real(_worldheight(fx+c,fz+c)) * mScaley;

	// calculate the weighted average
	Real height = height00*(1-modx)*(1-modz) 
					+ height01*(1-modx)*(modz) 
					+ height10*(modx)*(1-modz) 
					+ height11*(modx)*(modz);

	return height;
}

//----------------------------------------------------------------------------

// return the height difference between this point and a close other point
Real HeightMap::getXSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x+CONR("HEIGTHMAP_SLOPE_DIFF"),z);
}

//----------------------------------------------------------------------------

// return the height difference between this point and a close other point
Real HeightMap::getZSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x,z+CONR("HEIGTHMAP_SLOPE_DIFF"));
}

//----------------------------------------------------------------------------

// load the array from the image file
void HeightMap::loadTerrain( const String& filename )
{
	// make the image
	if (mImage) delete mImage;
	mImage = new Image();

	/* Set up the options */
	ConfigFile config;
	config.load( filename );

	mScalex = atof( config.getSetting( "Scale.x" ).c_str() );
	mScaley = atof( config.getSetting( "Scale.y" ).c_str() );
	mScalez = atof( config.getSetting( "Scale.z" ).c_str() );

	String terrain_filename = config.getSetting( "HeightMap" );

	mScale = Vector3( mScalex, mScaley, mScalez );

	mImage->load( terrain_filename );

	if ( mImage->getFormat() != PF_L8 )
	{
		Except( Exception::ERR_INVALIDPARAMS, "Error: Image is not a grayscale image.",
				"HeightMap::loadTerrain" );
	}

	mData = mImage->getData();
	mSize = mImage->getWidth();
}

//----------------------------------------------------------------------------

HeightMap& HeightMap::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new HeightMap();
	}
    return Singleton<HeightMap>::getSingleton();
}

//----------------------------------------------------------------------------

}