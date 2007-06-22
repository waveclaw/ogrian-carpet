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
    
    RakNet is Copyright rakkarsoft
    Ogrian Carpet licenses RakNet under the Creative Commons 
    Attribution - NonCommercial 2.5 license
    See: http://creativecommons.org/licenses/by-nc/2.5/
*****************************************************************************/

/**
 * \file OgrianMultiplayer.cpp
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Handles all of the multiplayer networking code.
 **/
#include "../Tests/Multiplayer.h"


using namespace Ogre;

template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;
namespace Ogrian
{

/**
 * Initialize an empty multiplayer configuration
 * @exception Warns if the Interface fails to create
 **/
Multiplayer::Multiplayer()
{
	mIsConnected = false;
	// TODO - get these from CONI, CONS and CONB
	mAddress = new String(CONS("SERVER")); // force raknet to use INADDR_ANY with ""
	mPort = CONI("PORT");
	mMaxConnections = CONI("MAXCONNECTIONS");
	mNumberPorts = CONI("NUMBEROFPORTS");
	mSleepTime = CONI("SLEEPTIME"); // milliseconds
	mIsServer = CONB("RUNASERVER");
	mPeer = RakNetworkFactory::GetRakPeerInterface();
	if (0 == mPeer) 
	{
		Exception( Exception::ERR_INTERNAL_ERROR, 
  	    String("Error pre-initalizing network system. RakNet had an error: RakNetworkFactory::GetRakPeerInterface"),
        String("Multiplayer::Multiplayer"));
	};		
} // end contructor

/**
 * We're done with the network
 **/
Multiplayer::~Multiplayer()
{
	if (mIsConnected) {
		if (isConnected()) disconnect();
	};
	mPeer->Shutdown(0,0); // wait 0 seconds, because we don't have a channel (0) to send notices out.
	RakNetworkFactory::DestroyRakPeerInterface(mPeer);
	delete mInputBuffer;
	delete mOutputBuffer;
} // end desctuctor

/**
 * Kick all the players, shutdown the server, clean up network
 * @exception logs the action of disconnecting
 **/
void Multiplayer::disconnect()
{
	if (mIsConnected) {
		if (mIsServer) {
		// kick all players first
		//serverSendAllText("kicked", ID_KICK);
		// TODO - other cleanup
		};
		Exception(Exception::ERR_NOT_IMPLEMENTED,"Disconnecting",
		 "Multiplayer::client");
		mIsConnected = false;					
	};
} // end disconnect

/**
 * Is this a server?
 * @return True if is a server, false otherwise
 **/
bool Multiplayer::isServer()
{
	return (mIsServer);
} // end isclient

/**
 * Is this a client?
 * @return True if is a client, false otherwise
 **/
bool Multiplayer::isClient()
{
	return (!mIsServer);
} // end isclient

/**
 * Is this a client?
 * @return True if is running and connected, false otherwise
 **/
bool Multiplayer::isConnected()
{
		return(mIsConnected); 
		// return mPeer->IsConnected(mAddress); <-- not what you think it is
} // end isconnected

/**
 * Connect to the network
 * @exception Writes the connection to the log or the error if failure
 **/
void Multiplayer::connect()
{
	if (isConnected())
	{
		Exception(Exception::ERR_INVALID_STATE,"Aready connected.",
         "Multiplayer::connect");
	} else {
		const char* password = CONS("PASSWORD").c_str();
		int passwordlength = strlen(password); 	
		if (mPeer->Connect(getAddress(), getPort(), password, passwordlength))
		{
			Exception(Exception::ERR_NOT_IMPLEMENTED,"Connected.",
			 "Multiplayer::connect");
			mIsConnected = true;
		} else {
			Exception(Exception::ERR_INVALID_STATE,"Unable to connect.",
			 "Multiplayer::connect");	
			mIsConnected = false;
		};
	}; // end if connected
} //  end connect

/**
 * Connect to the network
 * @exception Writes the connection to the log or the error if failure
 **/
void Multiplayer::listen() 
{
	if (isConnected()) 
	{
		Exception(Exception::ERR_INVALID_STATE,"Already connected, use disconnect first! ",
		 "Multiplayer::listen");			
	} else {
		if (mIsServer) 
		{
			mPeer->SetMaximumIncomingConnections(getMaxConnections());
			mIsConnected = true;			
			// connect();
		} else {
			Exception(Exception::ERR_INVALID_STATE,"Not a server, use connect! ",
			 "Multiplayer::listen");			
		}; // end if server		
	}; // end if active
} // end listen

/**
 * start the network interface so we can (dis)connect
 **/
void Multiplayer::startup(PeerType type) 
{
	if (mIsConnected) {
		Exception(Exception::ERR_INVALID_STATE,
			 "Starting client-server networking, but already started: ", 
			 "Multiplayer::startup");		
	} else {
	switch (type)  
	{
		case SERVER:
		{
			mIsServer = true;
			if (!mMaxConnections) setMaxConnections(DEFAULT_MAX_CONNECTIONS); // TODO - get this from the map?
			mPeer->Startup(mMaxConnections, mSleepTime, &SocketDescriptor(getPort(),0), mNumberPorts);
			Exception(Exception::ERR_NOT_IMPLEMENTED,
			 "Starting client-server networking, set to server: ", 
			 "Multiplayer::startup");
			 break;
		};
		case CLIENT:
		{
			mIsServer = false;
			
			setMaxConnections(NO_REMOTE_CONNECTIONS); // nobody likes you, nobody talks to you :-(
			mPeer->Startup(mMaxConnections,mSleepTime, &SocketDescriptor(), mNumberPorts);
			Exception(Exception::ERR_NOT_IMPLEMENTED,
			 "Starting client-server networking, set to server: ",
			  "Multiplayer::startup");
			  break;
		};
		case PEER2PEER:
		{
			Exception(Exception::ERR_INVALIDPARAMS,
			 "P2P Not currently supported: ", "Multiplayer::client::startup");							
			mIsConnected = false;
			 break;			
		};
		default:
		{
			Exception(Exception::ERR_INVALIDPARAMS,
			 "Unsupported networking style.", "Multiplayer::startup");						
			mIsConnected = false;
			 break;			
		};
	} // end which type
	}; // end if active
} // end startup

/**
 * get the current port for networking
 **/
unsigned short Multiplayer::getPort(void) {
	return mPort;
}; // end getPort

/**
 * get the current address for networking
 **/
const char* Multiplayer::getAddress(void) {
	return mAddress->c_str(); 
}; // end getAddress

/**
 * get the current maximum simultanious clients for networking
 **/
int Multiplayer::getMaxConnections(void) {
	return mMaxConnections;
} // end getMaxConnections
/**
 * set the port for the connection dynamically
 **/
void Multiplayer::setPort(unsigned short port) {
		mPort = port;
} // end setPort

/**
 * set the remote address for the connection dynamically
 **/
void Multiplayer::setAddress(char *address) {
	mAddress = new String(address);
}// end setAddress

/**
 * set the current maximum simultanious clients for networking (for map support)
 **/
void Multiplayer::setMaxConnections(int connections) {
	mMaxConnections = connections;
} // end setMaxConnections
	
/**
 * Implement the singleton pattern
 **/
Multiplayer& Multiplayer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Multiplayer();
	}
    return Singleton<Multiplayer>::getSingleton();
}

} // end namespace Ogrian
