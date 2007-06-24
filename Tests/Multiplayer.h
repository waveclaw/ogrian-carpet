/*****************************************************************************
	Copyright 2004 Mike Prosser

    This file is part of Ogrian Carpet.

    Ogrian Carpet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Ogrian Carpet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ogrian Carpet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

/**
 * \file OgrianMultiplayer.h
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief Handles all of the multiplayer networking code.
 **/
#ifndef __OgrianMultiplayer_H__
#define __OgrianMultiplayer_H__

#include <Ogre.h>
#include <OgreSingleton.h>  // make sure your ldd and plugins.cfg are 'clean'
#include <OgreException.h>

using namespace Ogre;

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
//include "MessageIdentifiers.h" <-- incompatible with OgrianPacketEnum.h
using namespace RakNet;

#include "SpinBuffer.h"
#include <stdlib.h>

using namespace util;

#include "OgrianConst.h"
#include "OgrianPacketEnum.h"
#define UNASSIGNED_PLAYER_ID -1
#define STRING_MAX_LENGTH 256
#define DEFAULT_MAX_CONNECTIONS 10
#define NO_REMOTE_CONNECTIONS 1

namespace Ogrian
{

/* \class Multiplayer OgrianMultiplayer.h "include/OgrianMultiplayer.h"
 * \brief The Multiplayer class handles all of the multiplayer networking code.
 */
class Multiplayer : public Singleton< Multiplayer >
{
public:
	/**
	 * Clean up after the multiplayer code, including kicking everyone, closing ports and thread shutdown. 
	 */
	virtual ~Multiplayer();

	/**
	 * Implement singleton, since the constructor is private.
	 * @return If no object exists, return a new object, else reference the existing 'single' object
	 */
    static Multiplayer& getSingleton(void);

	/**
	 * Is this a server?
	 * @return True if is a server and is running, false otherwise
	 **/
	bool isServer();
	
	/**
	 * Is this a client?
	 * @return True if is a client and is running, false otherwise
	 **/
	bool isClient(void);
	
	/**
	 * Is this a client?
	 * @return True if is a client and is running, false otherwise
	 **/
	bool isConnected(void);
	
	/**
	 * connect to the network
	 **/
	void connect(void);
	
	/**
	 * connect to the network (convenience function)
	 **/
	void listen(void);	

	/**
	 * disconnect from the network
	 **/
	void disconnect(void);

	/**
	 * start the network interface so we can (dis)connect
 	**/
	void startup(PeerType);
	
	/**
	 * get the current port for networking
	 **/
	unsigned short getPort(void);
	
	/**
	 * get the current address for networking
	 **/
	const char* getAddress(void);
	
	/**
	 * get the current maximum simultanious clients for networking
	 **/
	int getMaxConnections(void);
	
	/**
	 * set the port for the connection dynamically
	 **/
	void setPort(unsigned short);
	
	/**
	 * set the remote address for the connection dynamically
	 **/
	void setAddress(char*);
	
	/**
	 * set the current maximum simultanious clients for networking (for map support)
	 **/
	void setMaxConnections(int connections);
	
	 
private:
	String* mAddress; // To force raknet to use INADDR_ANY set this.address to "".
	unsigned short mPort;
	unsigned short mMaxConnections;
	int mSleepTime; // milliseconds
	int mNumberPorts; // usually 1 or more, never 0
	bool mIsServer;
	bool mIsConnected; 
	RakPeerInterface* mPeer;
	SocketDescriptor mSocket;
	//Packet *mPacket; // current packet (see spin buffers)
	SpinBuffer<Packet *,500> *mInputBuffer;
	SpinBuffer<Packet *,500> *mOutputBuffer;	
	Multiplayer();
	
}; // end class Multiplayer

} // end namespace ogrian
#endif
