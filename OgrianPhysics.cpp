

#include "OgrianPhysics.h"
#include "OgrianRollingEntity.h"
#include "OgrianConstants.h"

using namespace Ogre;

template<> Ogrian::Physics * Singleton< Ogrian::Physics >::ms_Singleton = 0;
namespace Ogrian
{


Physics::Physics()
{

}

void Physics::addPhysicalEntity(PhysicalEntity* ent)
{
	assert(ent != NULL);

	entities.push_back(ent);
}

void Physics::removePhysicalEntity(PhysicalEntity* ent)
{
	assert(ent != NULL);

	// find the entity
	for (unsigned int i=0; i<entities.size(); i++)
	{
		if (entities[i] == ent)
		{
			// erase it
			entities.erase(entities.begin()+i);

			break;
		}
	}
}

void Physics::removeAll()
{
	// delete each entity
	while(!entities.empty())
	{
		delete entities[entities.size()-1];
		entities.pop_back();
	}
}

void Physics::moveAll(Real time)
{
	for (unsigned int i=0; i<entities.size(); i++)
	{
		entities[i]->move(time);
	}
}

int Physics::numPhysicalEntities()
{
	return int(entities.size());
}

/* this method works by keeping all of the entities in an ordered vector.
Each of the entities is check against a few of the following entities for
collisions. 
*/
void Physics::collisionCheck()
{
	// sort the vector (by x location)
	std::sort(entities.begin(), entities.end());

	PhysicalEntity* a;
	PhysicalEntity* b;

	// for each entity
	for (unsigned int i=0; i<entities.size(); i++)
	{
		a = entities[i];
		for (unsigned int j=i+1; j<entities.size(); j++)
		{
			b = entities[j];
			Real dist = b->pos.x - a->pos.x;

			// if they are too far apart, stop
			if (dist > MAX_ENTITY_RADIUS + a->getRadius()) break;

			Real maxdist = a->getRadius() + b->getRadius();

			// if they are close enough, they collide
			if (a->distance(b) < maxdist)
			{
				Real ay = a->pos.y;
				Real by = b->pos.y;

				// if they are close enough in altitude
				if (ay-by < maxdist && by-ay < maxdist)
				{
					// they collide
					a->collided(b);
					b->collided(a);
				}
			}
		}
	}
}


Physics& Physics::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Physics();
	}
    return Singleton<Physics>::getSingleton();
}


Physics::~Physics()
{
	removeAll();
}

void Physics::test()
{
	// test the physucks engine

	LogManager::getSingleton().logMessage("physics testing - start");

	RollingEntity* a = new RollingEntity("Ogrian/Smoke", 2, 1, 1);
	RollingEntity* b = new RollingEntity("Ogrian/Smoke", 4, 1, 1);
	RollingEntity* c = new RollingEntity("Ogrian/Smoke", 6, 1, 1);
	RollingEntity* d = new RollingEntity("Ogrian/Smoke", 8, 1, 1);

	RollingEntity* aa = new RollingEntity("Ogrian/Smoke", 1, 1, 20);
	RollingEntity* ab = new RollingEntity("Ogrian/Smoke", 2, 1, 20);
	RollingEntity* ac = new RollingEntity("Ogrian/Smoke", 3, 1, 20);
	RollingEntity* ad = new RollingEntity("Ogrian/Smoke", 4, 1, 10);

	a->setScale(1);
	b->setScale(1);
	c->setScale(1);
	d->setScale(1);

	a->setVelocity(1,0,1);
	b->setVelocity(1,0,2);
	c->setVelocity(2,0,1);
	d->setVelocity(1.5,0,1.5);

	aa->setScale(2);
	ab->setScale(2);
	ac->setScale(2);
	ad->setScale(2);

	LogManager::getSingleton().logMessage("physics testing - add");
	Physics::getSingleton().addPhysicalEntity(a);
	Physics::getSingleton().addPhysicalEntity(b);
	Physics::getSingleton().addPhysicalEntity(c);
	Physics::getSingleton().addPhysicalEntity(d);
	Physics::getSingleton().addPhysicalEntity(aa);
	Physics::getSingleton().addPhysicalEntity(ab);
	Physics::getSingleton().addPhysicalEntity(ac);
	Physics::getSingleton().addPhysicalEntity(ad);

	if (4 == Physics::getSingleton().numPhysicalEntities()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");
	
	LogManager::getSingleton().logMessage("physics testing - move");
	Physics::getSingleton().moveAll(10);
	
	LogManager::getSingleton().logMessage("physics testing - collision test");
	Physics::getSingleton().collisionCheck();

	//LogManager::getSingleton().logMessage("physics testing - remove");
	//Physics::getSingleton().removePhysicalEntity(b);
	//Physics::getSingleton().removePhysicalEntity(c);
	//
	//if (2 == Physics::getSingleton().numPhysicalEntities()) 
	//	LogManager::getSingleton().logMessage("physics testing - correct number");
	//else 
	//	LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

	LogManager::getSingleton().logMessage("physics testing - move again");
	Physics::getSingleton().moveAll(10);

	LogManager::getSingleton().logMessage("physics testing - collision test again");
	Physics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - done");
}

}
