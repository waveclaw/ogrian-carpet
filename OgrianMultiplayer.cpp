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

/*------------------------------------*
OgrianMultiplayer.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This handles all of the multiplayer networking code.

/*------------------------------------*/

#include "OgrianMultiplayer.h"
#include "OgrianMenu.h"
#include "OgrianRenderer.h"
#include "OgrianPlayerList.h"
#include "OgrianPhysics.h"
#include "OgreConfigFile.h"


using namespace Ogre;

template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

Multiplayer::Multiplayer()
{
	mActive = false;

	loadConfig();
}

//----------------------------------------------------------------------------

Multiplayer::~Multiplayer()
{
	// We're done with the network
	if (mActive && mIsServer) serverDisconnect();
	if (mActive && !mIsServer) clientDisconnect();
}

//----------------------------------------------------------------------------
void Multiplayer::loadConfig()
{
	/* Set up the options */
	ConfigFile config;
	config.load( "ogrian.cfg" );
	mPlayerName = config.getSetting( "name" );
	mServerName = config.getSetting( "server" );

	// trim the name
	std::string name = mPlayerName;
	mPlayerName = name.substr(0,PLAYER_NAME_MAX_LENGTH);
}

//----------------------------------------------------------------------------
void Multiplayer::clientStart()
{
	assert(!mActive);

	mIsServer = false;
	mActive = true;
	
	mClient = RakNetworkFactory::GetRakClientInterface();

	// massage the name because RakNet is Stupid and wont take a const char*
	std::string name = mServerName;
	name = name.substr(0,16);
	char cn[16];
	strcpy(cn, name.c_str());

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	bool b = mClient->Connect(cn, PORT, PORT-1, 0, true);

	// error
	if (!b) Except( Exception::ERR_INTERNAL_ERROR, "Error: Could Not Connect Client.",
				"Multiplayer::startClient" );

	Menu::getSingleton().setMessage("Client Started");
}

//----------------------------------------------------------------------------

void Multiplayer::serverStart()
{
	assert(!mActive);

	mIsServer = true; 
	mActive = true;

	mServer = RakNetworkFactory::GetRakServerInterface();

	// Starting the server is very simple.  8 players allowed.
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	bool b = mServer->Start(7, 0, true, PORT);
	
	// error
	if (!b) Except( Exception::ERR_INTERNAL_ERROR, "Error: Could Not Create Server.",
				"Multiplayer::startServer" );
	
	Menu::getSingleton().setMessage("Server Started");

	mPlayerNames.push_back(mPlayerName);
	PlayerList::getSingleton().addPlayer(mPlayerName);
}

//----------------------------------------------------------------------------

void Multiplayer::clientSend(BitStream* bitStream)
{
	assert(!mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	mClient->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSend(BitStream* bitStream, PlayerID player)
{
	assert(mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	// We arbitrarily pick 0 for the ordering stream
	// false to send to only one player
	// true for security
	mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player, false, true);
}

//----------------------------------------------------------------------------

void Multiplayer::clientSend(char* message)
{
	assert(!mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	mClient->Send(message, (long)strlen(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSend(char* message, PlayerID player)
{
	assert(mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	// We arbitrarily pick 0 for the ordering stream
	// false to send to only one player
	// true for security
	mServer->Send(message, (long)strlen(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, player, false, true);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendAll(char* message)
{
	assert(mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	// We arbitrarily pick 0 for the ordering stream
	// false to send to only one player
	// true for security
	mServer->Send(message, (long)strlen(message), HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true, true);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendAll(BitStream* bitStream)
{
	assert(mIsServer);
	assert(mActive);

	// bitstream is the data to send
	// strlen(message)+1 is to send the null terminator
	// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
	// RELIABLE_ORDERED means make sure the message arrives in the right order
	// We arbitrarily pick 0 for the ordering stream
	// false to send to only one player
	// true for security
	mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true, true);
}

//----------------------------------------------------------------------------

void Multiplayer::frame()
{
	if (!mActive) return;

	if (mIsServer) serverRecieve();
	else clientRecieve();
}

//----------------------------------------------------------------------------

void Multiplayer::clientRecieve()
{
	Packet* packet = mClient->Receive();

	while (packet != 0)
	{
		// We got a packet, get the identifier with our handy function
		PacketID packetId = getPacketIdentifier(packet);

		// handle the packet
		if      (clientHandlePacket(packet, packetId)) {}
		else if (handleRakPacket(packet, packetId)) {}
		//else if (Physics::getSingleton().handleClientPacket(packet, packetId)) {}

		mClient->DeallocatePacket(packet);
		packet = mClient->Receive();
	}
}

//----------------------------------------------------------------------------

void Multiplayer::serverRecieve()
{
	Packet* packet = mServer->Receive();

	while (packet != 0)
	{
		// We got a packet, get the identifier with our handy function
		PacketID packetId = getPacketIdentifier(packet);

		// handle the packet
		if      (serverHandlePacket(packet, packetId)) {}
		else if (handleRakPacket(packet, packetId)) {}
		//else if (Physics::getSingleton().handleServerPacket(packet, packetId)) {}
			
		mServer->DeallocatePacket(packet);
		packet = mServer->Receive();
	}
}

//----------------------------------------------------------------------------

void Multiplayer::clientDisconnect()
{
	assert(!mIsServer);
	assert(mActive);

	// disconnect
	mClient->Disconnect();
	RakNetworkFactory::DestroyRakClientInterface(mClient);

	// clear the player list
	PlayerList::getSingleton().clear();
	
	mActive = false;
}

//----------------------------------------------------------------------------

void Multiplayer::serverDisconnect()
{
	assert(mIsServer);
	assert(mActive);

	// should probably kick all players first

	// disconnect
	mServer->Disconnect();
	RakNetworkFactory::DestroyRakServerInterface(mServer);

	// clear the player list
	PlayerList::getSingleton().clear();
	mActive = false;
}

//----------------------------------------------------------------------------

bool Multiplayer::isServer()
{
	return (mIsServer && mActive);
}

//----------------------------------------------------------------------------

bool Multiplayer::isClient()
{
	return (!mIsServer && mActive);
}

//----------------------------------------------------------------------------

int Multiplayer::serverNumPlayers()
{
	assert(mIsServer);
	assert(mActive);
	return mServer->GetConnectedPlayers();
}

//----------------------------------------------------------------------------

bool Multiplayer::isConnected()
{
	if (!mActive) return false;

	if (mIsServer) 
		return mServer->IsActive();
	else 
		return mClient->IsConnected();
}

//----------------------------------------------------------------------------

// Copied from Multiplayer.cpp (from RakNet)
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
PacketID Multiplayer::getPacketIdentifier(Packet* p)
{
	if (p==0)
		return (PacketID)255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (PacketID) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (PacketID) p->data[0];
}

//----------------------------------------------------------------------------

bool Multiplayer::clientHandlePacket(Packet* packet, PacketID pid)
{
	// Check if this is a network message packet
	switch (pid)
	{
		case ID_ADD_PLAYER: //////////////////////////////////////////////////////
		{
			// get the name
			char name[PLAYER_NAME_MAX_LENGTH];
			int len, UID;

			BitStream bs(packet->data,packet->length,false);
			bs.Read(UID);
			bs.Read(len);
			bs.Read(name,len);

			// update the player list
			PlayerList::getSingleton().addPlayer(name);

			return true;
		}

		case ID_CONNECTION_REQUEST_ACCEPTED: //////////////////////////////////////////////////////
		{
			return true;
		}

		case ID_MAP_NAME:
		{
			// get the name
			char mname[MAP_NAME_MAX_LENGTH];
			int mlen, UID;

			BitStream mbs(packet->data,packet->length,false);
			mbs.Read(UID);
			mbs.Read(mlen);
			mbs.Read(mname,mlen);	

			// if the map is different from the one we have loaded
			if (mname != Renderer::getSingleton().getMapName())
			{
				// disconnect
				clientDisconnect();

				// load the new map
				Renderer::getSingleton().loadMap(mname);

				// reconnect
				clientStart();
			}

			// send our name to the server
			char name[PLAYER_NAME_MAX_LENGTH];
			strcpy(name, mPlayerName);
			int len = (int)strlen(name) + 1;

			BitStream bs;
			bs.Write(ID_ADD_PLAYER);
			bs.Write(len);
			bs.Write(name,len);

			clientSend(&bs);
			return true;

		}

		case ID_CONNECTION_LOST: //////////////////////////////////////////////////////
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally terminated
			//Except( Exception::ERR_INTERNAL_ERROR, "Error: Connection to Server lost.",
				//"Multiplayer::handleOtherPacket" );
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------

bool Multiplayer::serverHandlePacket(Packet* packet, PacketID pid)
{
	// Check if this is a network message packet
	switch (pid)
	{
		case ID_ADD_PLAYER: //////////////////////////////////////////////////////
		{
			// get the name
			char name[PLAYER_NAME_MAX_LENGTH];
			int len, UID;

			BitStream bs(packet->data,packet->length,true);
			bs.Read(UID);
			bs.Read(len);
			bs.Read(name,len);

			// update the player list
			mPlayerNames.push_back(name);
			PlayerList::getSingleton().addPlayer(name);

			// forward the message to all clients
			BitStream bt(packet->data, packet->length, false);
			serverSendAll(&bt);

			return true;
		}

		case ID_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
		{
			// Somebody connected.  We have their IP now
			mPlayers.push_back(packet->playerId);

			// send the names of the existing players
			char name[PLAYER_NAME_MAX_LENGTH];
			for (int i=0; i<(int)mPlayerNames.size(); i++)
			{
				strcpy(name, mPlayerName);
				int len = (int)strlen(name) + 1;

				BitStream bs;
				bs.Write(ID_ADD_PLAYER);
				bs.Write(len);
				bs.Write(name,len);

				serverSend(&bs,packet->playerId);
			}
				
			// send the name of the map
			char mname[MAP_NAME_MAX_LENGTH];
			strcpy(mname, Renderer::getSingleton().getMapName());
			int len = (int)strlen(mname) + 1;

			BitStream bs;
			bs.Write(ID_MAP_NAME);
			bs.Write(len);
			bs.Write(mname,len);

			serverSend(&bs,packet->playerId);

			return true;
		}
		case ID_DISCONNECTION_NOTIFICATION: //////////////////////////////////////////////////////
		case ID_CONNECTION_LOST: 
		{
			// client no longer connected

			// remove their name from the list

			// remove their ip from the list

			
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------

bool Multiplayer::handleRakPacket(Packet* packet, PacketID pid)
{
	// Check if this is a network message packet
	switch (pid)
	{
		case ID_DISCONNECTION_NOTIFICATION: // Connection lost normally
			return true;

		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.
			return true;

		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.
			return true;

		case ID_REMOTE_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
			return true;

		case ID_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
			return true;
			
		case ID_CONNECTION_LOST: //////////////////////////////////////////////////////
			return true;

		case ID_REMOTE_EXISTING_CONNECTION: // Server telling you of an existing connection that was there before you connected
			return true;

		case ID_RECEIVED_STATIC_DATA: // Got static data
			return true;

		case ID_NO_FREE_INCOMING_CONNECTIONS: // Sorry, the server is full.  I don't do anything here but A real app should tell the user
			return true;

		case ID_MODIFIED_PACKET: // Cheater!
			return true;

		case ID_CONNECTION_REQUEST_ACCEPTED: //////////////////////////////////////////////////////
			return true;

		case ID_CONNECTION_RESUMPTION: // Client reconnected before getting disconnected
			return true;
	}
	return false;
}
//----------------------------------------------------------------------------

Multiplayer& Multiplayer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Multiplayer();
	}
    return Singleton<Multiplayer>::getSingleton();
}

//----------------------------------------------------------------------------

}