//this file is used to bypass files that prevent compling due to out of date dependancies.
//Ogre::Vector3 defined in <Ogre.h>

#include "../Patches/stub.hpp"

template<> Ogrian::Renderer* Singleton< Ogrian::Renderer >::ms_Singleton = 0;
template<> Ogrian::LogManager* Singleton< Ogrian::LogManager >::ms_Singleton = 0;
template<> Ogrian::Game* Singleton< Ogrian::Game >::ms_Singleton = 0;
template<> Ogrian::Hud* Singleton< Ogrian::Hud >::ms_Singleton = 0;
template<> Ogrian::SpellManager* Singleton< Ogrian::SpellManager >::ms_Singleton = 0;
template<> Ogrian::Menu* Singleton< Ogrian::Menu >::ms_Singleton = 0;
template<> Ogrian::Physics* Singleton< Ogrian::Physics >::ms_Singleton = 0;


namespace Ogrian {

Physics& Physics::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Physics(); return Singleton<Physics>::getSingleton(); }
Physics::Physics() {}
Thing *Physics::getThing(int id) { return new Thing();}
int Physics::getTeam(int team) { return 0;}
void Physics::addThing(WizardThing*&) {}
void Physics::syncClient(void *) {}

Renderer& Renderer::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Renderer(); return Singleton<Renderer>::getSingleton(); }
Renderer::Renderer() {}
CameraThing *Renderer::getCameraThing() { return new CameraThing();};
String Renderer::getMapName(void) {return "foo";}
void Renderer::loadMap(String map) {}

Game& Game::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Game(); return Singleton<Game>::getSingleton(); }
Game::Game() {}
void victory(void) {};
Vector3 getStartPos(void) { return Vector3(0.0f, 0.0f, 0.0f);}

Menu& Menu::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Menu(); return Singleton<Menu>::getSingleton(); }
Menu::Menu() {}
String Menu::getChosenName(void) { return String(" ");};
int Menu::getChosenServer(void) {return 0;};
int Menu::getChosenCastleSkin(void) { return 0;}
void Menu::hide(bool flag) {}
void Menu::clearLoadingScreen(void) {}
void show(bool) {}
void button_disconnect(void) {}

LogManager& LogManager::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new LogManager(); return Singleton<LogManager>::getSingleton(); }
LogManager::LogManager() {};
void LogManager::logMessage(String str) { cout << str << endl; };

WizardThing::WizardThing(bool visible) {};

Hud& Hud::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new Hud(); return Singleton<Hud>::getSingleton(); }
Hud::Hud() {}

SpellManager& SpellManager::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new SpellManager(); return Singleton<SpellManager>::getSingleton(); }
SpellManager::SpellManager() {}

Thing* Thing::getThing(int id) {return new Thing();}
Thing::Thing() {}
Vector3 Thing::getDirection(void) { return Vector3(0.0f, 0.0f, 0.0f);}
Vector3 Thing::getVelocity(void) { return Vector3(0.0f, 0.0f, 0.0f);}
Vector3 Thing::getPosition(void) { return Vector3(0.0f, 0.0f, 0.0f);}
int Thing::getUID(void) { return 0;}
int Thing::getTeamNum(void) { return 0;}
void Thing::setPosition(Vector3) {}
int Thing::setMessage(void *message) {return 0;}
void Thing::_setUID(int uid) {}	 	
void Thing::playSound(int sound) {}
void Thing::addHealth(int hp) {}
void Thing::setHealth(int hp) {}
void Thing::die(void) {}
void Thing::makeGhost(void) {}
void Thing::handleMessage(String, Vector3, int) {}

} // end Ogrian namespace
