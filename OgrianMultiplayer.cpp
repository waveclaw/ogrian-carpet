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

void Multiplayer::clientRecieve()
{
	assert(!mIsServer);
	assert(mActive);

	Packet* p = mClient->Receive();

	// do stuff here
	if (p != 0)
		Menu::getSingleton().setMessage(p->data);

	mClient->DeallocatePacket(p);
}

//----------------------------------------------------------------------------

void Multiplayer::serverRecieve()
{
	assert(mIsServer);
	assert(mActive);

	Packet* p = mServer->Receive();

	// do stuff here
	if (p != 0)
		Menu::getSingleton().setMessage(p->data);

	mServer->DeallocatePacket(p);
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
	return mIsServer;
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