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

	Real getHeightAt(Real x, Real z);

	Real getXSlopeAt(Real x, Real z);
	Real getZSlopeAt(Real x, Real z);

    static HeightMap& getSingleton(void);

private:
	Vector3 mScale;
	int mSize;

	Real mScalex, mScaley, mScalez;
	const uchar * mData;

	Image image;

	int _worldheight( int x, int z );
};

}
#endif