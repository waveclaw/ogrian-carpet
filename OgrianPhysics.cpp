

#include "OgrianPhysics.h"
#include "OgrianConstants.h"

using namespace Ogre;

template<> OgrianPhysics * Singleton< OgrianPhysics >::ms_Singleton = 0;

OgrianPhysics::OgrianPhysics()
{

}

void OgrianPhysics::addPhysicalEntity(OgrianPhysicalEntity* ent)
{
	assert(ent != NULL);

	entities.push_back(ent);
}

void OgrianPhysics::removePhysicalEntity(OgrianPhysicalEntity* ent)
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

void OgrianPhysics::removeAll()
{
	// delete each entity
	while(!entities.empty())
	{
		delete entities[entities.size()-1];
		entities.pop_back();
	}
}

void OgrianPhysics::moveAll(Real time)
{
	for (unsigned int i=0; i<entities.size(); i++)
	{
		entities[i]->move(time);
	}
}

int OgrianPhysics::numPhysicalEntities()
{
	return int(entities.size());
}

/* this method works by keeping all of the entities in an ordered vector.
Each of the entities is check against a few of the following entities for
collisions. 
*/
void OgrianPhysics::collisionCheck()
{
	// sort the vector (by x location)
	std::sort(entities.begin(), entities.end());

	OgrianPhysicalEntity* a;
	OgrianPhysicalEntity* b;

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
			if (a->distance(b) < maxdist && 
				fabs(a->pos.y - b->pos.y) < maxdist)
			{
				// they collide
				a->collided(b);
				b->collided(a);
			}
		}
	}
}


OgrianPhysics& OgrianPhysics::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new OgrianPhysics();
	}
    return Singleton<OgrianPhysics>::getSingleton();
}


OgrianPhysics::~OgrianPhysics()
{
	removeAll();
}

void OgrianPhysics::test()
{
	// test the physucks engine
	LogManager::getSingleton().logMessage("physics testing - start");

	OgrianPhysicalEntity* a = new OgrianPhysicalEntity("ogrehead.mesh", 1, 1, 1);
	OgrianPhysicalEntity* b = new OgrianPhysicalEntity("ogrehead.mesh", 2, 1, 1);
	OgrianPhysicalEntity* c = new OgrianPhysicalEntity("ogrehead.mesh", 3, 1, 1);
	OgrianPhysicalEntity* d = new OgrianPhysicalEntity("ogrehead.mesh", 4, 1, 1);

	OgrianPhysicalEntity* aa = new OgrianPhysicalEntity("ogrehead.mesh", 1, 1, 10);
	OgrianPhysicalEntity* ab = new OgrianPhysicalEntity("ogrehead.mesh", 2, 1, 10);
	OgrianPhysicalEntity* ac = new OgrianPhysicalEntity("ogrehead.mesh", 3, 1, 10);
	OgrianPhysicalEntity* ad = new OgrianPhysicalEntity("ogrehead.mesh", 4, 1, 10);

	a->setScale(1);
	b->setScale(1);
	c->setScale(1);
	d->setScale(1);

	a->setVelocity(.1,0,0);
	b->setVelocity(.2,0,0);
	c->setVelocity(.3,0,0);
	d->setVelocity(.4,0,0);

	aa->setScale(2);
	ab->setScale(2);
	ac->setScale(2);
	ad->setScale(2);

	LogManager::getSingleton().logMessage("physics testing - add");
	OgrianPhysics::getSingleton().addPhysicalEntity(a);
	OgrianPhysics::getSingleton().addPhysicalEntity(b);
	OgrianPhysics::getSingleton().addPhysicalEntity(c);
	OgrianPhysics::getSingleton().addPhysicalEntity(d);
	OgrianPhysics::getSingleton().addPhysicalEntity(aa);
	OgrianPhysics::getSingleton().addPhysicalEntity(ab);
	OgrianPhysics::getSingleton().addPhysicalEntity(ac);
	OgrianPhysics::getSingleton().addPhysicalEntity(ad);

	if (4 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");
	
	LogManager::getSingleton().logMessage("physics testing - move");
	OgrianPhysics::getSingleton().moveAll(10);
	
	LogManager::getSingleton().logMessage("physics testing - collision test");
	OgrianPhysics::getSingleton().collisionCheck();

	//LogManager::getSingleton().logMessage("physics testing - remove");
	//OgrianPhysics::getSingleton().removePhysicalEntity(b);
	//OgrianPhysics::getSingleton().removePhysicalEntity(c);
	//
	//if (2 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
	//	LogManager::getSingleton().logMessage("physics testing - correct number");
	//else 
	//	LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

	LogManager::getSingleton().logMessage("physics testing - move again");
	OgrianPhysics::getSingleton().moveAll(10);

	LogManager::getSingleton().logMessage("physics testing - collision test again");
	OgrianPhysics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - done");
}
