#ifndef __OgrianPhysicalEntity_H__
#define __OgrianPhysicalEntity_H__

#include <Ogre.h>
#include "OgrianRenderer.h"
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class PhysicalEntity
{
public:
	Vector3 pos;

	PhysicalEntity(String material, Real x=0, Real y=0, Real z=0);

	virtual void setVelocity(Vector3 vel);
	virtual void setPosition(Vector3 pos);

	virtual void setPosition(Real x, Real y, Real z);


	virtual void setVelocity(Real x, Real y, Real z);

	virtual void setScale(Real scale);

	virtual void move(Real time);

	virtual Real distance(PhysicalEntity* e);

	virtual Real getRadius();

	virtual void collided(PhysicalEntity* e);

	// they are ordered by x location
	bool operator<(PhysicalEntity* other);

	
private:
	Vector3 vel;

	BillboardSet* bbset;
	Billboard* billboard;

	Real radius;
	Real height;

	        /// Incremented count for next name extension
        static unsigned long msNextGeneratedNameExt;
};

}
#endif