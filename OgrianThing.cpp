#include "OgrianThing.h"
namespace Ogrian
{

unsigned long Thing::msNextGeneratedNameExt = 1;

Thing::Thing(String material, String prefix, bool fixed_y, Real scale, Real x, Real y, Real z)
{
	String name = prefix << "_" << msNextGeneratedNameExt++;
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

	bbset = sceneMgr->createBillboardSet(name,1);
	billboard = bbset->createBillboard(0, 0, 0);
	bbset->setMaterialName(material);

	if (fixed_y)
	{
		// it doesn't really matter if its common or self, since there's only one per set
		bbset->setBillboardType(BBT_ORIENTED_SELF);
		billboard->mDirection = Vector3::UNIT_Y;
	}

	node = sceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(bbset);

	setPosition(x, y, z);
	setScale(scale);
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
	pos.x = x;
	pos.y = y;
	pos.z = z;

	node->setPosition(x,y,z);
}


void Thing::setVelocity(Real x, Real y, Real z)
{
	vel.x = x;
	vel.y = y;
	vel.z = z;
}

void Thing::setScale(Real scale)
{
	billboard->setDimensions(scale,scale);

	radius = scale/2;
}

void Thing::move(Real time)
{
	setPosition(
		pos.x + vel.x * time,
		pos.y + vel.y * time,
		pos.z + vel.z * time);
}

Real Thing::distance(Thing* e)
{
	return sqrt((pos.x - e->pos.x)*(pos.x - e->pos.x) 
			    + (pos.z - e->pos.z)*(pos.z - e->pos.z));
}

Real Thing::getRadius()
{
	return radius;
}

void Thing::collided(Thing* e)
{
	// override this for interesting behaviors
	
}

// they are ordered by x location
bool Thing::operator<(Thing* other)
{
	return (pos.x < other->pos.x);
}

}