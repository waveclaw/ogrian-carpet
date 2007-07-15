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
    
    RakNet is Copyright rakkarsoft
    Ogrian Carpet licenses RakNet under the Creative Commons 
    Attribution - NonCommercial 2.5 license
    See: http://creativecommons.org/licenses/by-nc/2.5/
*****************************************************************************/

/**
 * \file OgrianMultiplayer.cpp
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Abstract the networking layer. 
 **/
#include "../Tests/Multiplayer.h"


using namespace Ogre;

// Dear Lord, abusing Singleton to make global variables.... :-(
//template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;

namespace Ogrian
{

/**
 * Initialize an empty multiplayer configuration
 * \exception Warns if the Interface fails to create
 **/
Multiplayer::Multiplayer()
{
	mIsConnected = false;
	// TODO - get these from CONI, CONS and CONB
	mAddress = new String(CONS("SERVER")); // force raknet to use INADDR_ANY with ""
	mPort = CONI("PORT");
	mMaxConnections = CONI("MAX_CONNECTIONS");
	mNumberPorts = CONI("NUMBER_OF_PORTS"); // should always be 1 or more from the cfg file
	mSleepTime = CONI("SLEEPTIME"); // milliseconds
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
	int waitSeconds = CONI("SHUTDOWN_WAIT_SECONDS");
	// TODO - push this down into server, client or treat as an array
	mPeer->Shutdown(waitSeconds);
	RakNetworkFactory::DestroyRakPeerInterface(mPeer);
	delete mInputBuffer;
	delete mOutputBuffer;
} // end destructor


/**
 * Is this a good connection?
 * \return True if is running and connected, false otherwise
 **/
bool Multiplayer::isConnected()
{
		return(mIsConnected); 
		// return mPeer->IsConnected(mAddress); <-- not what you think it is
} // end isconnected

/**
 * get the current port for networking
 **/
unsigned short Multiplayer::getPort(void) {
	return mPort;
} // end getPort

/**
 * get the current address for networking
 **/
const char* Multiplayer::getAddress(void) {
	return mAddress->c_str(); 
} // end getAddress

/**
 * get the current maximum simultanious clients for networking
 **/
int Multiplayer::getMaxConnections(void) {
	return mMaxConnections;
} // end getMaxConnections

/**
 * get the current simultanious clients for networking
 * \returns the number of addresses accepting connections
 **/
int Multiplayer::getConnectionCount(void)
{
	int count = 0;
	for (int i = 0; i < getMaxConnections();i++)
		if (mPeer->GetSystemAddressFromIndex(i) != UNASSIGNED_SYSTEM_ADDRESS)
			count++;
	return count;
} // end getConnectCount

/**
 * set the port for the connection dynamically
 * \parameter the port (0 through 4,294,967,295). Uses 26000, the quake port, by default)
 **/
void Multiplayer::setPort(unsigned short port) 
{
		mPort = port;
} // end setPort

/**
 * set the remote address for the connection dynamically
 * \parameter the address to listen on or connect to.  Use "" to listen on all addresses.
 **/
void Multiplayer::setAddress(char *address) 
{
	mAddress = new String(address);
} // end setAddress

/**
 * set the current maximum simultanious clients for networking (for map support)
 * \parameter the number of connections to use.
 **/
void Multiplayer::setMaxConnections(int connections) 
{
	mMaxConnections = connections;
} // end setMaxConnections

/**
 * set the connection status
 * \parameter the state of the connection
 **/
void Multiplayer::setConnected(bool status)
{
	mIsConnected = status;
} // end setConnected

/**
 * set Time to Sleep in the network code
 * \parameter time to sleep before returning
 **/
void Multiplayer::setSleepTime(int time)
{
	if(time > 0) 
	{
		mSleepTime = time;
	} else {
		mSleepTime = OGRIAN_DEFAULT_SLEEP_TIME; // no cheating!
	}
} // end setSleepTime

/**
 * set the number of Ports
 * \parameter the number of ports > 1
 **/
void Multiplayer::setNumberPorts(int nPorts)
{
	if (nPorts > 0)
	{
		mNumberPorts = nPorts;
	} else {
		mNumberPorts = 1; // must be 1 or greater!
	}
} // end  setNumberPorts

/**
 * get the Time to Sleep in the network code
 * \returns time to sleep in millisecons before returning
 **/
int Multiplayer::getSleepTime(void)
{
	return mSleepTime;
} // end getSleepTime

/**
 * Get the number of ports
 * \returns the number of ports
 **/
int Multiplayer::getNumberPorts(void)
{
	return mNumberPorts;
} // end getNumberPorts
	
//----------------------------------------------------------------------------
// server parts 

/**
 * stop the network interface so we can disconnect
 **/
Server::~Server() 
{
	if ( isConnected() ) disconnect();
} // end destroyer

/**
 * start the network interface so we can (dis)connect
 **/
void Server::startup(void) 
{
	bool peerStart = false;
	if (isConnected()) 
	{
		Exception(Exception::ERR_INVALID_STATE,
			 "Starting client-server networking, but already started: ", 
			 "Multiplayer::startup");		
	} else {
		mSocket = SocketDescriptor(getPort(),0);
		if (!getMaxConnections()) setMaxConnections(DEFAULT_MAX_CONNECTIONS); // TODO - get this from the map?
		// TODO - init an array of peers
		peerStart = mPeer->Startup(getMaxConnections(), 
					   getSleepTime(), 
					   &mSocket, 
					   getNumberPorts());
		if (peerStart) 
		{
			Exception(Exception::ERR_NOT_IMPLEMENTED,
		 	"Starting client-server networking, set to server: ", 
		 	"Multiplayer::startup");
			mPeer->SetMaximumIncomingConnections(getMaxConnections());			 	
		} else {
			Exception(Exception::ERR_INVALID_STATE,
		 	"Failed client-server networking, server failed to startup: ", 
		 	"Multiplayer::startup");
		}; // end if started
	}; // end if active
} // end startup

/**
 * Connect to the network
 * \exception Writes the connection to the log or the error if failure
 **/
void Server::listen(void) 
{
	if (isConnected()) 
	{
		Exception(Exception::ERR_INVALID_STATE,"Already connected, use disconnect first! ",
		 "Multiplayer::listen");			
	} else {
		// TODO - spawn a thread to handle filling the input buffer
		setConnected(true);			
		// connect();
	}; // end if active
} // end listen

/**
 * Connect to the network
 * \exception Writes the connection to the log or the error if failure
 **/
void Server::connect(void)
{
	Exception(Exception::ERR_NOT_IMPLEMENTED,"Use Connect on a listen, not connect.",
	 "Server::listen");
}; // end listen

/**
 * Kick all the players, shutdown the server, clean up network
 * \exception logs the action of disconnecting
 **/
void Server::disconnect(void)
{
	if (isConnected()) 
	{
		// kick all players first
		//sendAll("kicked", ID_KICK);
		// TODO - other cleanup

		Exception(Exception::ERR_NOT_IMPLEMENTED,"Disconnecting",
		 "Multiplayer::server");
		setConnected(false);					
	};
} // end disconnect

//----------------------------------------------------------------------------
// client parts 

/**
 * stop the network interface so we can disconnect
 **/
Client::~Client() 
{
	if (isConnected()) disconnect();
} // end destroyer

/**
 * start the network interface so we can (dis)connect
 **/
void Client::startup(void) 
{
	if (isConnected()) 
	{
		Exception(Exception::ERR_INVALID_STATE,
			 "Starting client-server networking, but already started: ", 
			 "Multiplayer::startup");		
	} else {
		mSocket = SocketDescriptor();
		setMaxConnections(NO_REMOTE_CONNECTIONS); // nobody likes you, nobody talks to you :-(
		setConnected( mPeer->Startup(getMaxConnections(),
					     getSleepTime(),
					     &mSocket,
					     getNumberPorts() ) );
		if (isConnected()) 
		{
			Exception(Exception::ERR_NOT_IMPLEMENTED,
		 	"Starting client-server networking, set to client: ", 
		 	"Multiplayer::startup");
		} else {
			Exception(Exception::ERR_INVALID_STATE,
		 	"Failed client-server networking, server failed to startup: ", 
		 	"Multiplayer::startup");
		}; // end if started
	}; // end if active
} // end startup

/**
 * Connect to the network
 * \exception Writes the connection to the log or the error if failure
 **/
void Client::connect(void)
{
	if (isConnected())
	{
		Exception(Exception::ERR_INVALID_STATE,"Aready connected.",
         "Multiplayer::connect");
	} else {
		const char* password = CONS("PASSWORD").c_str();
		int passwordlength = strlen(password); 	
		setConnected( mPeer->Connect(getAddress(), 
					     getPort(),
					     password,
					     passwordlength) );
		if (isConnected()) 
		{
			Exception(Exception::ERR_NOT_IMPLEMENTED,"Connected.",
			 "Multiplayer::connect");
			// TODO - spawn a thread to handle filling the input buffer			
		} else {
			Exception(Exception::ERR_INVALID_STATE,"Unable to connect.",
			 "Multiplayer::connect");	
		};
	}; // end if connected
} //  end connect

/**
 * Connect to the network
 * \exception Writes the connection to the log or the error if failure
 **/
void Client::listen(void) 
{
	Exception(Exception::ERR_NOT_IMPLEMENTED,"Use Connect on a client, not listen.",
	 "Client::listen");
}; // end listen

/**
 * Quit the network connection
 * \exception logs the action of disconnecting
 **/
void Client::disconnect(void)
{
	if (isConnected()) 
	{
		// send leave notice to server
		Exception(Exception::ERR_NOT_IMPLEMENTED,"Disconnecting",
		 "Multiplayer::client");
		setConnected(false);					
	};
} // end disconnect
} // end namespace Ogrian
