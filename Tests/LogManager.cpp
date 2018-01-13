//this file is used to bypass Ogre's broken logManager class
//Ogre::Vector3 defined in <Ogre.h>

#include "../Tests/LogManager.hpp"

template<> Ogrian::LogManager* Singleton< Ogrian::LogManager >::ms_Singleton = 0;

namespace Ogrian {


LogManager& LogManager::getSingleton(void) { if (!ms_Singleton) ms_Singleton = new LogManager(); return Singleton<LogManager>::getSingleton(); }
LogManager::LogManager() {};
void LogManager::logMessage(String str) { cout << str << endl; };


} // end Ogrian namespace
