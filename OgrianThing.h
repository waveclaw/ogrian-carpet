#ifndef __OgrianThing_H__
#define __OgrianThing_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class Thing
{
public:
	Vector3 pos;

	Thing(String material, String prefix="thing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0);

	virtual void setVelocity(Vector3 vel);
	virtual void setPosition(Vector3 pos);

	virtual void setPosition(Real x, Real y, Real z);


	virtual void setVelocity(Real x, Real y, Real z);

	virtual void setScale(Real scale);

	virtual void move(Real time);

	virtual Real distance(Thing* e);

	virtual Real getRadius();

	virtual void collided(Thing* e);

	// they are ordered by x location
	bool operator<(Thing* other);

	
private:
	Vector3 vel;

	BillboardSet* bbset;
	Billboard* billboard;

	SceneNode* node;

	Real radius;
	Real height;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;
};

}
#endif