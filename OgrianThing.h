#ifndef __OgrianThing_H__
#define __OgrianThing_H__

#include <Ogre.h>
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

enum ThingType
{
	THING,
	ROLLINGTHING,
	FLOATINGTHING,
	MANATHING,
	FOLIAGETHING,
	CAMERATHING,
};


// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class Thing
{
public:
	Vector3 mPos;

	Thing(String material, String prefix="thing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0);
	virtual ~Thing();

	virtual void setVelocity(const Vector3 vel);
	virtual void setPosition(const Vector3 pos);

	virtual void setPosition(Real x, Real y, Real z);

	virtual void setVelocity(Real x, Real y, Real z);

	virtual void setScale(Real scale);

	virtual void setMaterial(String material);

	virtual void move(Real time);

	virtual Real distance(Thing* e);

	virtual Real getRadius();

	virtual Vector3 getPosition();
	virtual Vector3 getVelocity();
	virtual Real getScale();

	virtual ThingType getType(); 

	virtual void collided(Thing* e);

	virtual void destroy();

	virtual bool isAlive();

	// they are ordered by x location
	bool operator<(Thing* other);

	
protected:
	Vector3 mVel;

	BillboardSet* mBbset;
	Billboard* mBillboard;
	bool mFixed_y;
	String mMaterial;

	SceneNode* mNode;
	String mName;

	Real mRadius;
	Real mHeight;

	bool mAlive;

	bool mInRenderer;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	virtual void _addToRenderer();
	virtual void _removeFromRenderer();
	virtual void _updateVisibility();
};

}
#endif