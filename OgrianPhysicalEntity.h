#ifndef __OgrianPhysicalEntity_H__
#define __OgrianPhysicalEntity_H__

#include <Ogre.h>
#include "OgrianRenderer.h"

using namespace Ogre;

// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class OgrianPhysicalEntity
{
public:
	OgrianPhysicalEntity(String mesh, Real x=0, Real y=0, Real z=0)
	{
		SceneManager* sceneMgr = OgrianRenderer::getSingleton().getSceneManager();
		node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        Entity *ent = sceneMgr->createEntity(node->getName(), mesh);
        node->attachObject(ent);

		setPosition(x, y, z);
	}

	virtual void setPosition(Real x, Real y, Real z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;

		node->setPosition(x, y, z);
	}

	virtual void setVelocity(Real x, Real y, Real z)
	{
		vel.x = x;
		vel.y = y;
		vel.z = z;
	}

	virtual void move(long time)
	{
		time /= 1000;
		pos.x += vel.x * time;
		pos.y += vel.y * time;
		pos.z += vel.z * time;
	}

	// they are ordered by x location
	bool operator<(OgrianPhysicalEntity* other)
	{
		return (pos.x < other->pos.x);
	}

	
private:
	Vector3 pos;
	Vector3 vel;

	SceneNode* node;
};


#endif