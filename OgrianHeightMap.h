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

	void loadTerrain( const String& filename );

	float getHeightAt(float x, float z);

    static HeightMap& getSingleton(void);

private:
	Vector3 mScale;
	int mSize;

	float mScalex, mScaley, mScalez;
	const uchar * mData;

	Image image;

	int _worldheight( int x, int z );
};

}
#endif