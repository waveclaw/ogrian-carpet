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
#include "OgrianPlayerList.h"
#include "OgrianPhysics.h"


using namespace Ogre;

template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

Multiplayer::Multiplayer()
{
	mActive = false;
}

//----------------------------------------------------------------------------

Multiplayer::~Multiplayer()
{
	// We're done with the network
	if (mActive && mIsServer) serverDisconnect();
	if (mActive && !mIsServer) clientDisconnect();
}

//----------------------------------------------------------------------------

void Multiplayer::clientStart(char* server)
{
	assert(!mActive);

	mIsServer = false;
	mActive = true;
	
	mClient = RakNetworkFactory::GetRakClientInterface();

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	bool b = mClient->Connect(server, PORT, PORT-1, 0, false);

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
	bool b = mServer->Start(7, 0, false, PORT);
	
	// error
	if (!b) Except( Exception::ERR_INTERNAL_ERROR, "Error: Could Not Create Server.",
				"Multiplayer::startServer" );
	
	Menu::getSingleton().setMessage("Server Started");

	PlayerList::getSingleton().addPlayer("Server");
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

		if(handleOtherPacket(packet, packetId))
		{
			//Physics::getSingleton().handleClientPacket(packet, packetId);
		}
			
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

		if(handleOtherPacket(packet, packetId))
		{
			//Physics::getSingleton().handleServerPacket(packet, packetId);
		}
			
		mServer->DeallocatePacket(packet);
		packet = mServer->Receive();
	}
}

//----------------------------------------------------------------------------

void Multiplayer::clientDisconnect()
{
	assert(!mIsServer);
	assert(mActive);

	mClient->Disconnect();
	
	RakNetworkFactory::DestroyRakClientInterface(mClient);
	
	mActive = false;
}

//----------------------------------------------------------------------------

void Multiplayer::serverDisconnect()
{
	assert(mIsServer);
	assert(mActive);

	// should probably kick all players first

	mServer->Disconnect();
	
	RakNetworkFactory::DestroyRakServerInterface(mServer);

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

bool Multiplayer::handleOtherPacket(Packet* p, PacketID pid)
{
	// Check if this is a network message packet
	switch (pid)
	{
		case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
			break;

		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			
			break;
		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
			
			break;
		case ID_REMOTE_EXISTING_CONNECTION: // Server telling you of an existing connection that was there before you connected
			
			break;
		case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				mPlayers.push_back(p->playerId);
				PlayerList::getSingleton().addPlayer( String("(") << p->playerId.binaryAddress << ")" );
			break;

		case ID_RECEIVED_STATIC_DATA:
				// Got static data
		
			break;


		case ID_NO_FREE_INCOMING_CONNECTIONS:
			// Sorry, the server is full.  I don't do anything here but
			// A real app should tell the user
			
			break;
		case ID_MODIFIED_PACKET:
			// Cheater!
			
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			// This tells the client they have connected
			
			break;

		case ID_CONNECTION_RESUMPTION:
			// Client reconnected before getting disconnected
			
			break;

		default:
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