


#include "Ogre.h"
#include "OgrianHeightMap.h"
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

namespace Ogre
{
	template<> OgrianHeightMap * Singleton< OgrianHeightMap >::ms_Singleton = 0;

	OgrianHeightMap::OgrianHeightMap()
	{
		mData = 0;
	}

	int OgrianHeightMap::_worldheight( int x, int z )
    {
		if (x < 0) return 0;
		if (z < 0) return 0;
		if (x > mSize) return 0;
		if (z > mSize) return 0;

		if (!mData) return 0;

        return mData[ ( ( z * mSize ) + x ) ];
    };

	float OgrianHeightMap::getHeightAt(float x, float z)
	{
		x /= mScalex;
		z /= mScalez;

		float modx = x - int(x);
		float modz = z - int(z);

		int height00 = _worldheight(x,z) * mScaley;
		int height01 = _worldheight(x,z+1) * mScaley;
		int height10 = _worldheight(x+1,z) * mScaley;
		int height11 = _worldheight(x+1,z+1) * mScaley;

		float height = height00*(1-modx)*(1-modz) 
					 + height01*(1-modx)*(modz) 
					 + height10*(modx)*(1-modz) 
					 + height11*(modx)*(modz);

		return height;
	}

	void OgrianHeightMap::loadTerrain( const String& filename )
	{
		/* Set up the options */
		ConfigFile config;

		config.load( filename );

		mScalex = atof( config.getSetting( "ScaleX" ) );

		mScaley = atof( config.getSetting( "ScaleY" ) );

		mScalez = atof( config.getSetting( "ScaleZ" ) );

		String terrain_filename = config.getSetting( "Terrain" );

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

	OgrianHeightMap& OgrianHeightMap::getSingleton(void)
    {
		if (!ms_Singleton) 
		{
			ms_Singleton = new OgrianHeightMap();
		}
        return Singleton<OgrianHeightMap>::getSingleton();
    }

	OgrianHeightMap::~OgrianHeightMap()
	{
		
	}
}