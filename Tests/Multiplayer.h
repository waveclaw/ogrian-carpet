/*****************************************************************************
	Copyright 2007 Jeremiah Powell, 
	Based on OgrianMultiplayer Copyright 2004 Mike Prosser.

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
 * \author Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Abstract the networking layer. 
 **/
#ifndef __OgrianMultiplayer_H__
#define __OgrianMultiplayer_H__

#include <Ogre.h>
#include <OgreSingleton.h>  // make sure your ldd and plugins.cfg are 'clean'
#include <OgreException.h>
using namespace Ogre;

#include <RakNetworkFactory.h>
#include <RakPeerInterface.h>
#include <RakNetTypes.h>
//include "MessageIdentifiers.h" <-- redundant with OgrianPacketEnum.h
using namespace RakNet;

#include "SpinBuffer.h"
#include <stdlib.h>
using namespace util;

#include "OgrianConst.h"
#include "OgrianPacketEnum.h"

namespace Ogrian
{

/**
 * \class Multiplayer 
 * \brief The Multiplayer class handles all of the multiplayer networking code.
 */
class Multiplayer
{
	bool mIsConnected; 
	int mSleepTime; // milliseconds
	int mNumberPorts; // usually 1 or more, never 0
	unsigned short mMaxConnections;	
	unsigned short mPort;
	String* mAddress; // To force raknet to use INADDR_ANY set this.address to "".
	//Packet *mPacket; // current packet (see spin buffers)
	SpinBuffer<Packet *, OGRIAN_DEFAULT_BUFFER_SIZE> *mInputBuffer;
	SpinBuffer<Packet *, OGRIAN_DEFAULT_BUFFER_SIZE> *mOutputBuffer;	
protected:
	RakPeerInterface* mPeer; // either 1 server or N clients
	SocketDescriptor mSocket;

	/**
	 * set the connection status
	 * \parameter the state of the connection
	 **/
	void setConnected(bool);

	/**
	 * set Time to Sleep in the network code
	 * \parameter time to sleep before returning
	 **/
	void setSleepTime(int);

	/**
	 * set the number of Ports
	 * \parameter the number of ports > 1
	 **/
	void setNumberPorts(int);

	/**
	 * get the Time to Sleep in the network code
	 * \returns time to sleep in millisecons before returning
	 **/
	int getSleepTime(void);

	/**
	 * Get the number of ports
	 * \returns the number of ports
	 **/
	int getNumberPorts(void);
public:
	/**
	 * Create a Multiplayer instance,  client or a server
	 */
	Multiplayer();

	/**
	 * Clean up after the multiplayer code, including kicking everyone, closing ports and thread shutdown. 
	 */
	virtual ~Multiplayer();

	/**
	 * start the network interface so we can (dis)connect
 	**/
	virtual void startup(void) =0;

	/**
	 * disconnect from the connection
	 **/
	virtual void disconnect(void) =0;

	/**
	 * connect to the network (convenience function)
	 **/
	virtual void listen(void) =0;

	/**
	 * connect to the network
	 **/
	virtual void connect(void) =0;

	/**
	 * Is this connected?
	 * \return True if is a client and is running, false otherwise
	 **/
	bool isConnected(void);
		
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
	 * \returns the current max connections
	 **/
	int getMaxConnections(void);
	
	/**
	 * get the current simultanious clients for networking
	 * \returns the number of addresses accepting connections
	 **/
	int getConnectionCount(void);		

	/**
	 * set the port for the connection dynamically
	 * \parameter port (0 through 4,294,967,295). Uses 26000, the quake port, by default)
	 **/
	void setPort(unsigned short);
	/**
	 * set the remote address for the connection dynamically
	 * \parameter address the address to listen on or connect to.  Use "" to listen on all addresses.
	 **/
	void setAddress(char*);
	
	/**
	 * set the current maximum simultanious clients for networking (for map support)
	 * \parameter the number of connections to use.
	 **/
	void setMaxConnections(int);	
}; // end class Multiplayer

/**
 * \class Client 
 * \brief We are a client, implement good client behavior
 */
class Client: public Multiplayer 
{
public:	
	/**
	 * stop the network interface so we can disconnect
	 **/
	~Client();

	/**
	 * connect to the network (convenience function)
	 **/
	void listen(void);	

	/**
	 * connect to the network
	 **/
	void connect(void);

	/**
	 * disconnect from the network
	 **/
	void disconnect(void);

	/**
	 * start the network interface so we can (dis)connect
 	**/
	void startup(void);
}; // end class Client

/**
 * \class Server 
 * \brief Server class, which has several client connections to manage.
 */
class Server: public Multiplayer 
{
public:
	/**
	 * stop the network interface so we can disconnect
	 **/
	~Server();

	/**
	 * connect to the network (convenience function)
	 **/
	void listen(void);	

	/**
	 * connect to the network
	 **/
	void connect(void);
	
	/**
	 * disconnect from the network
	 **/
	void disconnect(void);

	/**
	 * start the network interface so we can (dis)connect
 	**/
	void startup(void);
}; // end class Server

class Streamable {
};

} // end namespace ogrian
#endif
