// a list of global constants

#ifndef __OgrianHeightMap_H__
#define __OgrianHeightMap_H__


#include <OgreSingleton.h>
#include <OgreTerrainSceneManager.h>

namespace Ogre
{


class OgrianHeightMap : public Singleton< OgrianHeightMap >
{
public:
	OgrianHeightMap();
	virtual ~OgrianHeightMap();

	void loadTerrain( const String& filename );

	float getHeightAt(float x, float z);

        /** Override standard Singleton retrieval.
            @remarks
                Why do we do this? Well, it's because the Singleton implementation is in a .h file,
                which means it gets compiled into anybody who includes it. This is needed for the Singleton
                template to work, but we actually only want it compiled into the implementation of the
                class based on the Singleton, not all of them. If we don't change this, we get link errors
                when trying to use the Singleton-based class from an outside dll.
            @par
                This method just delegates to the template version anyway, but the implementation stays in this
                single compilation unit, preventing link errors.
        */
    static OgrianHeightMap& getSingleton(void);

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