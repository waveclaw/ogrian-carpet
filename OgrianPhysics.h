#ifndef __OgrianPhysics_H__
#define __OgrianPhysics_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

// The OgrianPysics class handles collision detection and notification among all OgrianPhysicalEntity objects.
class Physics : public Singleton< Physics >
{
public:
	virtual ~Physics();

	// add a physical entity to the world.
	virtual void addThing(Thing* ent);

	// return the number of entities in the world
	virtual int numThings();

	// remove all physical entities from the world.
	virtual void removeAll();

	// notify all physical objects of a frame move
	virtual void moveAll(Real time);

	// run a collision check and notify all affected physical objects
	virtual void collisionCheck();

    static Physics& getSingleton(void);

private:
	Physics();

	// remove a physical entity from the world. 
	virtual void removeThing(Thing* ent);

	std::vector<Thing*> things;
};

}
#endif