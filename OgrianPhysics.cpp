

#include "OgrianPhysics.h"

namespace Ogre
{
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
		entities.clear();
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
		
	}

	void OgrianPhysics::test()
	{
		// test the physucks engine
		LogManager::getSingleton().logMessage("physics testing - start");

		OgrianPhysicalEntity* a = new OgrianPhysicalEntity();
		OgrianPhysicalEntity* b = new OgrianPhysicalEntity();
		OgrianPhysicalEntity* c = new OgrianPhysicalEntity();
		OgrianPhysicalEntity* d = new OgrianPhysicalEntity();

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
		
		LogManager::getSingleton().logMessage("physics testing - remove");
		OgrianPhysics::getSingleton().removePhysicalEntity(b);
		OgrianPhysics::getSingleton().removePhysicalEntity(c);
		
		if (2 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
			LogManager::getSingleton().logMessage("physics testing - correct number");
		else 
			LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

		LogManager::getSingleton().logMessage("physics testing - move again");
		OgrianPhysics::getSingleton().moveAll(10);

		LogManager::getSingleton().logMessage("physics testing - remove all");
		OgrianPhysics::getSingleton().removeAll();

		if (0 == OgrianPhysics::getSingleton().numPhysicalEntities()) 
			LogManager::getSingleton().logMessage("physics testing - correct number");
		else 
			LogManager::getSingleton().logMessage("physics testing - ERRRO! incorrect number");

		delete a;
		delete b;
		delete c;
		delete d;
		LogManager::getSingleton().logMessage("physics testing - done");
	}
}