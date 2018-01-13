/* \file this file is used to bypass the broken Ogre LogManager class
 * \author: Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * Licensed under the GNU GPL v2 or (at your option) later.
 **/

/* \Note Ogre::Vector3 defined in <Ogre.h>
 **/

#include <Ogre.h>
#include <iostream>

#ifndef _LOGMANAGER_HPP_
#define _LOGMANAGER_HPP_

using namespace std;
using namespace Ogre;

namespace Ogrian
{

#define Exception(X,Y,Z) Ogrian::LogManager::getSingleton().logMessage( Ogre::Exception(X, Y, Z).getDescription());
	
class LogManager : public Singleton< LogManager > {
	public:
	static LogManager& getSingleton(void);
	void logMessage(String);
	private:
	LogManager(void);
};


} // end Ogrian Namespace

#endif
