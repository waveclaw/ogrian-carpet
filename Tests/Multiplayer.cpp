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
	// get one or more peers
	mIsConnected = false;
	mAddress = new String(CONS("SERVER")); // force raknet to use INADDR_ANY with ""
	mPort = CONI("PORT");
	mMaxConnections = CONI("MAX_CONNECTIONS");
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
	delete mInputBuffer;
	delete mOutputBuffer;
} // end destructor


/**
 * Is this a good connection?
 * \return True if is running and connected, false otherwise
 **/
bool Multiplayer::isConnected() const
{
		return(mIsConnected); 
		// return mPeer->IsConnected(mAddress); <-- not what you think it is
} // end isconnected

/**
 * get the current port for networking
 **/
unsigned short Multiplayer::getPort(void) const {
	return mPort;
} // end getPort

/**
 * get the current address for networking
 **/
const char* Multiplayer::getAddress(void) const {
	return mAddress->c_str(); 
} // end getAddress

/**
 * get the current maximum simultanious clients for networking
 **/
int Multiplayer::getMaxConnections(void) const {
	return mMaxConnections;
} // end getMaxConnections

/**
 * get the current simultanious clients for networking
 * \returns the number of addresses accepting connections
 **/
int Multiplayer::getConnectionCount(void) const
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
void Multiplayer::setPort(const unsigned short port) 
{
		mPort = port;
} // end setPort

/**
 * set the remote address for the connection dynamically
 * \parameter the address to listen on or connect to.  Use "" to listen on all addresses.
 **/
void Multiplayer::setAddress(const char *address) 
{
	mAddress = new String(address);
} // end setAddress

/**
 * set the current maximum simultanious clients for networking (for map support)
 * \parameter the number of connections to use.
 **/
void Multiplayer::setMaxConnections(const int connections) 
{
	mMaxConnections = connections;
} // end setMaxConnections

/**
 * set the connection status
 * \parameter the state of the connection
 **/
void Multiplayer::setConnected(const bool status)
{
	mIsConnected = status;
} // end setConnected

/**
 * set Time to Sleep in the network code
 * \parameter time to sleep before returning
 **/
void Multiplayer::setSleepTime(const int time)
{
	if(time > 0) 
	{
		mSleepTime = time;
	} else {
		mSleepTime = OGRIAN_DEFAULT_SLEEP_TIME; // no cheating!
	}
} // end setSleepTime

/**
 * get the Time to Sleep in the network code
 * \returns time to sleep in millisecons before returning
 **/
int Multiplayer::getSleepTime(void) const
{
	return mSleepTime;
} // end getSleepTime

/**
 * Put this packet into the buffer to send at the next opportunity
 * the return value is meaningful when we don't let the buffers get beyond 1 packet
 * per cycle. This is also the best performance point for a spin-buffer based packet buffer.
 * \parameter the packet to send
 * \returns true if the buffer has "something" in it at least
 **/	
bool Multiplayer::send(Packet *p) const {
	mOutputBuffer->put(p);
	return !mOutputBuffer->empty();
} // end send

/**
 * Get the latest packet from the buffer
 * \returns the packet requested or null if no packet found
 **/
Packet* Multiplayer::receive() const
{
	return mInputBuffer->get();
};	// end receive


/**
 * update the internal State of the server
**/	
void Multiplayer::updateState(const RakNetTime curTime)
{
	Packet *p = mPeer->Receive();
	while (p)
	{
		switch (p->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			isConnected=true;
			break;
			// print out errors
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Client Error: ID_CONNECTION_ATTEMPT_FAILED\n");
			isConnected=false;
			break;
		case ID_ALREADY_CONNECTED:
			printf("Client Error: ID_ALREADY_CONNECTED\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("Client Error: ID_NO_FREE_INCOMING_CONNECTIONS\n");
			isConnected=false;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			//printf("ID_DISCONNECTION_NOTIFICATION\n");
			isConnected=false;
			break;
		case ID_CONNECTION_LOST:
			printf("Client Error: ID_CONNECTION_LOST\n");
			isConnected=false;
			break;
		case ID_MODIFIED_PACKET:
			printf("Client Error: ID_MODIFIED_PACKET\n");
			break;
		}
		peer->DeallocatePacket(p);
		p = peer->Receive();
	
	}

	if (curTime>flipConnectionTime)
	{
		if (isConnected)
		{
			Disconnect();
			flipConnectionTime=curTime+5000+(randomMT()%10000);
		}
		else
		{
			Connect();
			flipConnectionTime=curTime+5000+(randomMT()%15000);
		}
	}

	if (curTime>nextSendTime && isConnected)
	{
		peer->Send((const char*)&randomData,RANDOM_DATA_SIZE,HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
		nextSendTime=curTime+500+(randomMT()%1000);
	}
}; // end update



	
//----------------------------------------------------------------------------
// server parts 

/**
 * Create a server
 **/
Server::Server()
{
	// get the configured default number of clients
	// TODO - get the map's number of clients
	mNumberClients = CONI("NUMBER_OF_CLIENTS"); // should always be 1 or more from the cfg file
	// get one or more peers
	mIsConnected = false;
	mAddress = new String(CONS("SERVER")); // force raknet to use INADDR_ANY with ""
	mPort = CONI("PORT");
	mMaxConnections = CONI("MAX_CONNECTIONS");
	mSleepTime = CONI("SLEEPTIME"); // milliseconds
	mPeer = RakNetworkFactory::GetRakPeerInterface(); 
	if (0 == mPeer) 
	{
	Exception( Exception::ERR_INTERNAL_ERROR, 
  	           String("Error pre-initalizing network system. RakNet had an error: RakNetworkFactory::GetRakPeerInterface"),
                   String("Multiplayer::Multiplayer"));
	};		
		
} // end constructor

/**
 * stop the network interface so we can disconnect
 **/
Server::~Server() 
{
	if ( isConnected() ) disconnect();
	int waitSeconds = CONI("SHUTDOWN_WAIT_SECONDS");
	// shutdown the peers	
	for (int i = 0;i < getNumberClients();i++) {
		Multiplayer::mPeer[i]->Shutdown(waitSeconds);
		RakNetworkFactory::DestroyRakPeerInterface(Multiplayer::mPeer[i]);
	};	
    // super
    	
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
		if (0 == getMaxConnections()) setMaxConnections(DEFAULT_MAX_CONNECTIONS); // TODO - get this from the map?
		// TODO - init an array of peers
		peerStart = mPeer->Startup(getMaxConnections(), 
					   getSleepTime(), 
					   &mSocket, 
					   getNumberClients());
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
	// initialize the system
	startup();
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


/**
 * set the number of Ports
 * \parameter the number of ports > 1
 **/
void Server::setNumberClients(const int nClients)
{
	if (nClients > 0)
	{
		mNumberClients = nClients;
	} else {
		mNumberClients = 1; // must be 1 or greater!
	}
} // end  setNumberClients

/**
 * Get the number of Clients
 * \returns the number of Clients
 **/
int Server::getNumberClients(void) const
{
	return mNumberClients;
} // end getNumberClients

    /**
     * update the internal State of the server
     **/	
void Server::updateState(const RakNetTime curTime)
{
for (int i = 0; i < getNumberClients(); i++) {
	Packet *p = mPeer[i]->Receive();
	while (p)
	{
		switch (p->data[0])
		{
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_NEW_INCOMING_CONNECTION:
			printf("Connections = %i\n", ConnectionCount());
			break;
		case ID_USER_PACKET_ENUM:
			{
				if (memcmp(p->data, randomData, RANDOM_DATA_SIZE)!=0)
				{
					printf("Bad data on server!\n");
				}
				break;
			}
		}
		mPeer[i]->DeallocatePacket(p);
		p = mPeer[i]->Receive();
	}
} // end for clients
}
		
//----------------------------------------------------------------------------
// client parts 

/**
 * stop the network interface so we can disconnect
 **/
Client::~Client() 
{
	if (isConnected()) disconnect();
	int waitSeconds = CONI("SHUTDOWN_WAIT_SECONDS");
	// for a given program, there is only 1 peer 'object' which treats
	// servers as N-ary connection objects and clients as unary connection objects	
	Multiplayer::mPeer->Shutdown(waitSeconds);
	RakNetworkFactory::DestroyRakPeerInterface(Multiplayer::mPeer);		
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
		setConnected( Multiplayer::mPeer->Startup(getMaxConnections(),
					     getSleepTime(),
					     &mSocket,
					     NUM_CLIENT_PORTS ) );
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
	// initialize the system
	startup();
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
