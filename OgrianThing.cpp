#include "OgrianThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"

namespace Ogrian
{

unsigned long Thing::msNextGeneratedNameExt = 1;

Thing::Thing(String material, String prefix, bool fixed_y, Real scale, Real x, Real y, Real z)
{
	String name = prefix << "_" << msNextGeneratedNameExt++;
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

	mBbset = sceneMgr->createBillboardSet(name,1);
	mBillboard = mBbset->createBillboard(0, 0, 0);

	if (fixed_y)
	{
		// it doesn't really matter if its common or self, since there's only one per set
		mBbset->setBillboardType(BBT_ORIENTED_SELF);
		mBillboard->mDirection = Vector3::UNIT_Y;
	}

	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mBbset);

	if (material != "") setMaterial(material);
	setVelocity(0,0,0);
	Thing::setPosition(x, y, z);
	setScale(scale);

	mAlive = true;
}

bool Thing::isAlive()
{
	return mAlive;
}

void Thing::setVelocity(Vector3 vel)
{
	setVelocity(vel.x, vel.y, vel.z);
}
void Thing::setPosition(Vector3 pos)
{
	setPosition(pos.x, pos.y, pos.z);
}

void Thing::setPosition(Real x, Real y, Real z)
{
	mPos.x = x;
	mPos.y = y;
	mPos.z = z;

	mNode->setPosition(x,y,z);
}


void Thing::setVelocity(Real x, Real y, Real z)
{
	mVel.x = x;
	mVel.y = y;
	mVel.z = z;
}

void Thing::setScale(Real scale)
{
	mBillboard->setDimensions(scale,scale);

	mRadius = scale/2;
}

void Thing::setMaterial(String material)
{
	mBbset->setMaterialName(material);
}

void Thing::move(Real time)
{
	setPosition(
		mPos.x + mVel.x * time,
		mPos.y + mVel.y * time,
		mPos.z + mVel.z * time);
}

Real Thing::distance(Thing* e)
{
	return sqrt((mPos.x - e->mPos.x)*(mPos.x - e->mPos.x) 
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

Real Thing::getRadius()
{
	return mRadius;
}

ThingType Thing::getType()
{
	return THING;
}

void Thing::collided(Thing* e)
{
	// override this for interesting behaviors
	
}

void Thing::destroy()
{
	// mark it to be deleted
	mAlive = false;
}

Thing::~Thing()
{
	// remove it from the scene
	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 
}

// they are ordered by x location
bool Thing::operator<(Thing* other)
{
	return (mPos.x < other->mPos.x);
}

}