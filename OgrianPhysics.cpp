

#include "OgrianPhysics.h"
#include "OgrianRollingThing.h"
#include "OgrianConstants.h"

using namespace Ogre;

template<> Ogrian::Physics * Singleton< Ogrian::Physics >::ms_Singleton = 0;
namespace Ogrian
{


Physics::Physics()
{

}

void Physics::addThing(Thing* ent)
{
	assert(ent != NULL);

	things.push_back(ent);
}

void Physics::removeThing(Thing* ent)
{
	assert(ent != NULL);

	// find the entity
	for (unsigned int i=0; i<things.size(); i++)
	{
		if (things[i] == ent)
		{
			// erase it
			things.erase(things.begin()+i);

			break;
		}
	}
}

void Physics::removeAll()
{
	// delete each entity
	while(!things.empty())
	{
		delete things[things.size()-1];
		things.pop_back();
	}
}

void Physics::moveAll(Real time)
{
	for (unsigned int i=0; i<things.size(); i++)
	{
		things[i]->move(time);
	}
}

int Physics::numThings()
{
	return int(things.size());
}

/* this method works by keeping all of the entities in an ordered vector.
Each of the entities is check against a few of the following entities for
collisions. 
*/
void Physics::collisionCheck()
{
	// sort the vector (by x location)
	std::sort(things.begin(), things.end());

	Thing* a;
	Thing* b;

	// for each entity
	for (unsigned int i=0; i<things.size(); i++)
	{
		a = things[i];
		for (unsigned int j=i+1; j<things.size(); j++)
		{
			b = things[j];
			Real dist = b->pos.x - a->pos.x;

			// if they are too far apart, stop
			if (dist > MAX_THING_RADIUS + a->getRadius()) break;

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

	RollingThing* a = new RollingThing("Ogrian/Smoke", 2, 1, 1);
	RollingThing* b = new RollingThing("Ogrian/Smoke", 4, 1, 1);
	RollingThing* c = new RollingThing("Ogrian/Smoke", 6, 1, 1);
	RollingThing* d = new RollingThing("Ogrian/Smoke", 8, 1, 1);

	RollingThing* aa = new RollingThing("Ogrian/Smoke", 1, 1, 20);
	RollingThing* ab = new RollingThing("Ogrian/Smoke", 2, 1, 20);
	RollingThing* ac = new RollingThing("Ogrian/Smoke", 3, 1, 20);
	RollingThing* ad = new RollingThing("Ogrian/Smoke", 4, 1, 10);

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
	Physics::getSingleton().addThing(a);
	Physics::getSingleton().addThing(b);
	Physics::getSingleton().addThing(c);
	Physics::getSingleton().addThing(d);
	Physics::getSingleton().addThing(aa);
	Physics::getSingleton().addThing(ab);
	Physics::getSingleton().addThing(ac);
	Physics::getSingleton().addThing(ad);

	if (4 == Physics::getSingleton().numThings()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");
	
	LogManager::getSingleton().logMessage("physics testing - move");
	Physics::getSingleton().moveAll(10);
	
	LogManager::getSingleton().logMessage("physics testing - collision test");
	Physics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - move again");
	Physics::getSingleton().moveAll(10);

	LogManager::getSingleton().logMessage("physics testing - collision test again");
	Physics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - done");
}

}
