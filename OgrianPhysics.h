#ifndef __OgrianPhysics_H__
#define __OgrianPhysics_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianPhysicalEntity.h"

using namespace Ogre;

// The OgrianPysics class handles collision detection and notification among all OgrianPhysicalEntity objects.
class OgrianPhysics : public Singleton< OgrianPhysics >
{
public:
	virtual ~OgrianPhysics();

	// add a physical entity to the world.
	virtual void addPhysicalEntity(OgrianPhysicalEntity* ent);

	// remove a physical entity from the world. 
	virtual void removePhysicalEntity(OgrianPhysicalEntity* ent);

	// return the number of entities in the world
	virtual int numPhysicalEntities();

	// remove all physical entities from the world.
	virtual void removeAll();

	// notify all physical objects of a frame move
	virtual void moveAll(long time);

	// run a collision check and notify all affected physical objects
	virtual void collisionCheck();

	// run a simple test
	virtual void test();

    static OgrianPhysics& getSingleton(void);

private:
	OgrianPhysics();
	
	bool entCompare(OgrianPhysicalEntity* a, OgrianPhysicalEntity* b);

	std::vector<OgrianPhysicalEntity*> entities;
};

#endif