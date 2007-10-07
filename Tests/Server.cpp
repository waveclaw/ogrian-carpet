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
 * \file Server.h
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Implementation of Server-side logic. 
 **/
#include "Server.h"

using namespace Ogre;

namespace Ogrian
{
	
/**
 * Create a server
 **/
Server::Server() : Multiplayer(CONS("SERVER"), CONI("PORT"), CONI("SLEEPTIME"))
{
	// get the configured default number of clients
	// TODO - get the map's number of clients
	mNumberClients = CONI("NUMBER_OF_CLIENTS"); // should always be 1 or more from the cfg file
	// get one or more peers
} // end constructor

/**
 * stop the network interface so we can disconnect
 **/
Server::~Server() 
{
	RakPeerInterface *peer = NULL:
	if ( isConnected() ) disconnect();
	int waitSeconds = CONI("SHUTDOWN_WAIT_SECONDS");
	// shutdown the peers	
	for (int i = 0;i < getNumberClients();i++) {
		peer = Multiplayer::getPeer(i);
		peer->Shutdown(waitSeconds);
		RakNetworkFactory::DestroyRakPeerInterface(peer);
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
				// TODO: real data.
				if (memcmp(p->data, randomData, RANDOM_DATA_SIZE)!=0)
				{
						Exception( Exception::ERR_INTERNAL_ERROR, 
  	           						"Bad data on server!",
  	           						"Multiplayer::Server");
				}
				break;
			}
		}
		mPeer[i]->DeallocatePacket(p);
		p = mPeer[i]->Receive();
	}
} // end for clients
}; // end updateState

}// end namespace Ogrian
