

#include "OgrianPhysics.h"

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

void OgrianPhysics::moveAll(long time)
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

void OgrianPhysics::collisionCheck()
{
	// sort the vector (by x location)
	std::sort(entities.begin(), entities.end());

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

	OgrianPhysicalEntity* a = new OgrianPhysicalEntity(1, 1, 1);
	OgrianPhysicalEntity* b = new OgrianPhysicalEntity(2, 2, 2);
	OgrianPhysicalEntity* c = new OgrianPhysicalEntity(4, 4, 4);
	OgrianPhysicalEntity* d = new OgrianPhysicalEntity(3, 3, 3);

	LogManager::getSingleton().logMessage("physics testing - add");
	OgrianPhysics::getSingleton().addPhysicalEntity(a);
	OgrianPhysics::getSingleton().addPhysicalEntity(b);
	OgrianPhysics::getSingleton().addPhysicalEntity(c);
	OgrianPhysics::getSingleton().addPhysicalEntity(d);

	if (4 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");
	
	LogManager::getSingleton().logMessage("physics testing - move");
	OgrianPhysics::getSingleton().moveAll(10);
	
	LogManager::getSingleton().logMessage("physics testing - collision test");
	OgrianPhysics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - remove");
	OgrianPhysics::getSingleton().removePhysicalEntity(b);
	OgrianPhysics::getSingleton().removePhysicalEntity(c);
	
	if (2 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

	LogManager::getSingleton().logMessage("physics testing - move again");
	OgrianPhysics::getSingleton().moveAll(10);

	LogManager::getSingleton().logMessage("physics testing - collision test again");
	OgrianPhysics::getSingleton().collisionCheck();

	LogManager::getSingleton().logMessage("physics testing - remove all");
	OgrianPhysics::getSingleton().removeAll();

	if (0 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
		LogManager::getSingleton().logMessage("physics testing - correct number");
	else 
		LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

	LogManager::getSingleton().logMessage("physics testing - done");
}
