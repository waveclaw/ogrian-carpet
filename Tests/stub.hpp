//this file is used to bypass files that prevent compling due to out of date dependancies.
//Ogre::Vector3 defined in <Ogre.h>

#include <Ogre.h>
#include <iostream>
#include "../Tests/LogManager.hpp"

#ifndef _STUB_HPP_
#define _STUB_HPP_

using namespace std;
using namespace Ogre;

namespace Ogrian
{

//define CONR(X) 0 // not needed with patch for OgrianConst
//define CONI(X) 0 // not needed with patch for OgrianConst
#define PlayerID int

struct Packet {
	int playerId;
	int length;
	char *data;
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
 	int setMessage(void *);	
	Vector3 getDirection(void);
	Vector3 getVelocity(void);
	void _setUID(int uid);		 	
 	void die(void);
 	void destroy(void);
 	void makeGhost(void);
	void handleMessage(String, Vector3, int);	
	void addHealth(int);
 	void setHealth(int);	
	private:
};

class CameraThing : public Thing {
	public:
};

class Wizard : public Thing {
	public:
};


class Hud : public Singleton< Hud >, Thing {
	public:
	static Hud& getSingleton(void);
	private:
	Hud(void);
};
	
class SpellManager : public Singleton< SpellManager >, Thing {
	public:
	static SpellManager& getSingleton(void);
	void setLevel(int);
	private:
	SpellManager(void);	
};
	
class WizardThing: public Thing {
	public:
	WizardThing(bool);
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
	void addThing(WizardThing *&);
 	void handleClientPacket(void);
 	void handleServerPacket(void);
 	void reset(void);
	private:
	Physics(void);
};

class Game : public Singleton< Game >, Thing {
	public:
	static Game& getSingleton(void);
	void victory(void);
	Vector3 getStartPos(void);
	private:
	Game(void);
};

class Menu : public Singleton< Menu >, Thing {
	public:
	static Menu& getSingleton(void);
 	String getChosenName(void);
 	int getChosenServer(void);
 	int getChosenCastleSkin(void);
 	void hide(bool);
 	void clearLoadingScreen(void);
 	void show(bool);
 	void button_disconnect(void);
 	private:
	Menu(void);
};
} // end Ogrian Namespace

#endif
