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

	PhysicalEntity(String material, Real x=0, Real y=0, Real z=0)
	{
		LogManager::getSingleton().logMessage("adding physical entity");

        // Generate a name
        static char name[64];
		sprintf(name, "Unnamed_%lu", msNextGeneratedNameExt++);

		SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

		bbset = sceneMgr->createBillboardSet(name,1);
		billboard = bbset->createBillboard(x, y, z);
		bbset->setMaterialName(material);

		//bbset->setBillboardType(BBT_ORIENTED_COMMON);
		//bbset->setCommonDirection(Vector3::UNIT_Y);

		sceneMgr->getRootSceneNode()->attachObject(bbset);

		setPosition(x, y, z);

		LogManager::getSingleton().logMessage("added physical entity");
	}

	virtual void setVelocity(Vector3 vel)
	{
		setVelocity(vel.x, vel.y, vel.z);
	}
	virtual void setPosition(Vector3 pos)
	{
		setPosition(pos.x, pos.y, pos.z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;

		billboard->setPosition(x, y, z);
	}


	virtual void setVelocity(Real x, Real y, Real z)
	{
		vel.x = x;
		vel.y = y;
		vel.z = z;
	}

	virtual void setScale(Real scale)
	{
		billboard->setDimensions(scale,scale);

		radius = scale/2;
	}

	virtual void move(Real time)
	{
		setPosition(
			pos.x + vel.x * time,
			pos.y + vel.y * time,
			pos.z + vel.z * time);
	}

	virtual Real distance(PhysicalEntity* e)
	{
		return sqrt((pos.x - e->pos.x)*(pos.x - e->pos.x) 
			      + (pos.z - e->pos.z)*(pos.z - e->pos.z));
	}

	virtual Real getRadius()
	{
		return radius;
	}

	virtual void collided(PhysicalEntity* e)
	{
		// override this for interesting behaviors
		
	}

	// they are ordered by x location
	bool operator<(PhysicalEntity* other)
	{
		return (pos.x < other->pos.x);
	}

	
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