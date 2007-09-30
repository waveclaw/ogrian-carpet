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
 * \file Client.cpp
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Implement Client-side logic 
 **/ 
 #include "Client.h"

using namespace Ogre;

// Ogrian Namespace 
namespace Ogrian
{
	
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
					     CONI("NUM_CLIENT_PORTS") ) );
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
