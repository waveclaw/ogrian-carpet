
#include "Ogre.h"
#include "OgrianHeightMap.h"
#include "OgrianConstants.h"
#include <OgreTerrainSceneManager.h>
#include <OgreImage.h>
#include <OgreConfigFile.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreCamera.h>
#include "OgreException.h"
#include "OgreStringConverter.h"
#include <OgreRenderSystem.h>
#include <fstream>

using namespace Ogre;

template<> Ogrian::HeightMap * Singleton< Ogrian::HeightMap >::ms_Singleton = 0;
namespace Ogrian
{


HeightMap::HeightMap()
{
	mData = 0;
}

int HeightMap::_worldheight( int x, int z )
{
	if (x < 0) return 0;
	if (z < 0) return 0;
	if (x > mSize) return 0;
	if (z > mSize) return 0;

	if (!mData) return 0;

    return mData[ ( ( z * mSize ) + x ) ];
};

Real HeightMap::getHeightAt(Real x, Real z)
{
/*	x /= mScalex;
	z /= mScalez;

	Real modx = x - int(x);
	Real modz = z - int(z);

	int height00 = _worldheight(x,z) * mScaley;
	int height01 = _worldheight(x,z+1) * mScaley;
	int height10 = _worldheight(x+1,z) * mScaley;
	int height11 = _worldheight(x+1,z+1) * mScaley;

	Real height = height00*(1-modx)*(1-modz) 
					+ height01*(1-modx)*(modz) 
					+ height10*(modx)*(1-modz) 
					+ height11*(modx)*(modz);

	return height;*/

	x /= mScalex;
	z /= mScalez;

	int fx = Math::Floor(x);
	int fz = Math::Floor(z);
	Real modx = (x - fx);
	Real modz = (z - fz);

	int height00 = _worldheight(fx  ,fz)   * mScaley;
	int height01 = _worldheight(fx  ,fz+1) * mScaley;
	int height10 = _worldheight(fx+1,fz)   * mScaley;
	int height11 = _worldheight(fx+1,fz+1) * mScaley;

	Real height =     height00*(1-modx)*(1-modz) 
					+ height01*(1-modx)*(modz) 
					+ height10*(modx)  *(1-modz) 
					+ height11*(modx)  *(modz);

	return height;
}

Real HeightMap::getXSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x+HEIGTHMAP_SLOPE_DIFF,z);
}

Real HeightMap::getZSlopeAt(Real x, Real z)
{
	return getHeightAt(x,z) - getHeightAt(x,z+HEIGTHMAP_SLOPE_DIFF);
}

void HeightMap::loadTerrain( const String& filename )
{
	/* Set up the options */
	ConfigFile config;

	config.load( filename );

	mScalex = atof( config.getSetting( "Scale.x" ) );

	mScaley = atof( config.getSetting( "Scale.y" ) );

	mScalez = atof( config.getSetting( "Scale.z" ) );

	String terrain_filename = config.getSetting( "HeightMap" );

	mScale = Vector3( mScalex, mScaley, mScalez );

	//Image image;

	image.load( terrain_filename );

	if ( image.getFormat() != PF_L8 )
	{
		Except( Exception::ERR_INVALIDPARAMS, "Error: Image is not a grayscale image.",
				"TerrainSceneManager::setWorldGeometry" );
	}

	mData = image. getData();

	mSize = image.getWidth();
}

HeightMap& HeightMap::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new HeightMap();
	}
    return Singleton<HeightMap>::getSingleton();
}

HeightMap::~HeightMap()
{
	
}

}