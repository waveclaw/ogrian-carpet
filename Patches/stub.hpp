//this file is used to bypass files that prevent compling due to out of date dependancies.
//Ogre::Vector3 defined in <Ogre.h>

#include <Ogre.h>
#include <iostream>

#ifndef _STUB_HPP_
#define _STUB_HPP_

using namespace std;
using namespace Ogre;

namespace Ogrian
{

#define CONR(X) 0
#define CONI(X) 0
#define Except(X,Y,Z) LogManager::getSingleton().logMessage(X + Y + Z);
#define PlayerID int

struct Packet {
	int playerId;
	int length;
	char *data;
};
	
class LogManager : public Singleton< LogManager > {
	public:
	static LogManager& getSingleton(void);
	void logMessage(String);
	private:
	LogManager(void);
};

class Thing {
	public:
	Thing(void);
	~Thing(void);
	void playSound(int);
	Thing *getThing(int id);
 	int getUID(void);
 	int getTeamNum(void);
 	Vector3 getPosition(void);
	void setPosition(Vector3);
	Vector3 getDirection(void);
	Vector3 getVelocity(void);
	void _setUID(int uid);		 	
 	void die(void);
 	void destroy(void);
 	void makeGhost(void);
	void handleMessage(String, Vector3, int);	
	void addHealth(int);
	private:
};

class CameraThing : public Thing {
	public:
};

class Wizard : public Thing {
	public:
};

class WizardThing: public Wizard {
	public:
	WizardThing(bool);	
};

class Game: public Thing {
	public:
};

class Renderer : public Singleton< Renderer >, Thing {
	public:
	static Renderer& getSingleton(void);
	CameraThing *getCameraThing(void);
	String getMapName(void);
	void loadMap(String);
	private:
	Renderer(void);
};

class Physics : public Singleton< Physics >, Thing {
	public:
	static Physics& getSingleton(void);
	Thing *getThing(int);
 	int getTeam(int);	
	void syncClient(void *);
	void syncClient(void &);
	void addThing(Thing &);
	void addThing(WizardThing &);	
	private:
	Physics(void);
};

} // end Ogrian Namespace

#endif
