

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

}
