#include "OgrianPhysicalEntity.h"
namespace Ogrian
{

unsigned long PhysicalEntity::msNextGeneratedNameExt = 1;

PhysicalEntity::PhysicalEntity(String material, Real x, Real y, Real z)
{
    // Generate a name
    static char name[64];
	sprintf(name, "PE_%lu", msNextGeneratedNameExt++);

	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

	bbset = sceneMgr->createBillboardSet(name,1);
	billboard = bbset->createBillboard(x, y, z);
	bbset->setMaterialName(material);

	// it doesn't really matter if its common or self, since there's only one per set
	bbset->setBillboardType(BBT_ORIENTED_SELF);
	//bbset->setCommonDirection(Vector3::UNIT_Y);
	billboard->mDirection = Vector3::UNIT_Y;

	sceneMgr->getRootSceneNode()->attachObject(bbset);

	setPosition(x, y, z);
}

void PhysicalEntity::setVelocity(Vector3 vel)
{
	setVelocity(vel.x, vel.y, vel.z);
}
void PhysicalEntity::setPosition(Vector3 pos)
{
	setPosition(pos.x, pos.y, pos.z);
}

void PhysicalEntity::setPosition(Real x, Real y, Real z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;

	billboard->setPosition(x, y, z);
}


void PhysicalEntity::setVelocity(Real x, Real y, Real z)
{
	vel.x = x;
	vel.y = y;
	vel.z = z;
}

void PhysicalEntity::setScale(Real scale)
{
	billboard->setDimensions(scale,scale);

	radius = scale/2;
}

void PhysicalEntity::move(Real time)
{
	setPosition(
		pos.x + vel.x * time,
		pos.y + vel.y * time,
		pos.z + vel.z * time);
}

Real PhysicalEntity::distance(PhysicalEntity* e)
{
	return sqrt((pos.x - e->pos.x)*(pos.x - e->pos.x) 
			    + (pos.z - e->pos.z)*(pos.z - e->pos.z));
}

Real PhysicalEntity::getRadius()
{
	return radius;
}

void PhysicalEntity::collided(PhysicalEntity* e)
{
	// override this for interesting behaviors
	
}

// they are ordered by x location
bool PhysicalEntity::operator<(PhysicalEntity* other)
{
	return (pos.x < other->pos.x);
}

}