

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
		Thing* thing = things[i];

		// delete anything thats not alive
		if (!thing->isAlive()) removeThing(thing);			
		else thing->move(time);
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

	// for each thing
	for (unsigned int i=0; i<things.size(); i++)
	{
		a = things[i];

		// look at the following things
		for (unsigned int j=i+1; j<things.size(); j++)
		{
			b = things[j];

			// they should be sorted by x coord
			assert(a->mPos.x <= b->mPos.x);

			if (b->mPos.x - a->mPos.x > MAX_THING_RADIUS*2)
			{
				// if they are very far apart, dont look at the following things - WHY DOESN'T THIS WORK?
				//j=(unsigned int)things.size();
			}
			else
			{
				Real maxdist = a->getRadius() + b->getRadius();
				// if they are close enough, they collide
				if (a->distance(b) < maxdist)
				{
					Real ay = a->mPos.y;
					Real by = b->mPos.y;

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
