#ifndef __OgrianPhysicalEntity_H__
#define __OgrianPhysicalEntity_H__

#include <Ogre.h>
#include "OgrianRenderer.h"

using namespace Ogre;

// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class OgrianPhysicalEntity
{
public:
	Vector3 pos;

	OgrianPhysicalEntity(String mesh, Real x=0, Real y=0, Real z=0)
	{
		SceneManager* sceneMgr = OgrianRenderer::getSingleton().getSceneManager();
		node = sceneMgr->getRootSceneNode()->createChildSceneNode();
        ent = sceneMgr->createEntity(node->getName(), mesh);
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

	virtual void setScale(Real s)
	{
		Real scale = s/ent->getBoundingRadius();
		node->setScale(scale,scale,scale);

		radius = s;
	}

	virtual void move(Real time)
	{
		setPosition(
			pos.x + vel.x * time,
			pos.y + vel.y * time,
			pos.z + vel.z * time);
	}

	virtual Real distance(OgrianPhysicalEntity* e)
	{
		return sqrt((pos.x - e->pos.x)*(pos.x - e->pos.x) 
			      + (pos.y - e->pos.y)*(pos.y - e->pos.y));
	}

	virtual Real getRadius()
	{
		return radius;
	}

	virtual void collided(OgrianPhysicalEntity* e)
	{
		// override this for interesting behaviors
	}

	// they are ordered by x location
	bool operator<(OgrianPhysicalEntity* other)
	{
		return (pos.x < other->pos.x);
	}

	
private:
	Vector3 vel;

	Entity* ent;
	SceneNode* node;
	Real radius;
	Real height;
};


#endif