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

#include "GetTime.h"

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
	mPlayerName = name.substr(0,STRING_MAX_LENGTH);
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

	PlayerInfo server;
	server.id.binaryAddress = 0;
	server.name = mPlayerName + " (Serving)";
	mPlayers.push_back(server);
	PlayerList::getSingleton().addPlayer(server.name);
}

//----------------------------------------------------------------------------

void Multiplayer::clientSend(BitStream* bitStream, bool reliable)
{
	assert(!mIsServer);
	assert(mActive);

	if (reliable)
	{
		// bitstream is the data to send
		// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
		// RELIABLE_ORDERED means make sure the message arrives in the right order
		mClient->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
	}
	else
	{
		mClient->Send(bitStream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}

//----------------------------------------------------------------------------

void Multiplayer::serverSend(BitStream* bitStream, PlayerID player, bool reliable)
{
	assert(mIsServer);
	assert(mActive);

	if (reliable)
	{
		// bitstream is the data to send
		// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
		// RELIABLE_ORDERED means make sure the message arrives in the right order
		// We arbitrarily pick 0 for the ordering stream
		// false to send to only one player
		// true for security
		mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player, false, true);
	}
	else
	{
		mServer->Send(bitStream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, player, false, true);
	}

}

//----------------------------------------------------------------------------

void Multiplayer::serverSendAll(BitStream* bitStream, bool reliable)
{
	assert(mIsServer);
	assert(mActive);

	if (reliable)
	{
		// bitstream is the data to send
		// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
		// RELIABLE_ORDERED means make sure the message arrives in the right order
		// We arbitrarily pick 0 for the ordering stream
		// true to send to all palyers
		// true for security
		mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true, true);	
	}
	else
	{
		mServer->Send(bitStream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_PLAYER_ID, true, true);	
	}
}

//----------------------------------------------------------------------------

void Multiplayer::clientSendText(String message, int type)
{
	BitStream bs;
	stringToBitStream(message,bs,type);

	clientSend(&bs);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendText(String message, int type, PlayerID player)
{
	BitStream bs;
	stringToBitStream(message,bs,type);

	serverSend(&bs,player);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendAllText(String message, int type)
{
	BitStream bs;
	stringToBitStream(message,bs,type);

	serverSendAll(&bs);
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
		else if (Physics::getSingleton().handleClientPacket(packet, packetId)) {}

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
		else if (Physics::getSingleton().handleServerPacket(packet, packetId)) {}
			
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

	// kick all players first
	serverSendAllText("kicked", ID_KICK);

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

int Multiplayer::getWizardUID(PlayerID pid)
{
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		if (mPlayers[i].id == pid) return mPlayers[i].wizardUID;
	}

	return -1;
}

//----------------------------------------------------------------------------

int Multiplayer::numClients()
{
	return (int)mPlayers.size()-1;
}

//----------------------------------------------------------------------------

PlayerInfo Multiplayer::getClient(int i)
{
	return mPlayers[i+1];
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
			String name;
			packetToString(packet,name);

			// update the player list
			PlayerList::getSingleton().addPlayer(name);

			return true;
		}

		case ID_REMOVE_PLAYER: //////////////////////////////////////////////////////
		{
			// get the name
			String name;
			packetToString(packet,name);

			// update the player list
			PlayerList::getSingleton().removePlayer(name);

			return true;
		}

		case ID_CONNECTION_REQUEST_ACCEPTED: //////////////////////////////////////////////////////
		{
			// oddly, nothing is done here
			return true;
		}

		case ID_MAP_NAME: //////////////////////////////////////////////////////
		{
			// get the name
			String map;
			packetToString(packet,map);

			// if the map is different from the one we have loaded
			if (map != Renderer::getSingleton().getMapName())
			{
				// disconnect
				clientDisconnect();

				// load the new map
				Renderer::getSingleton().loadMap(map);

				// reconnect
				clientStart();
				return true;
			}

			// send our name to the server
			clientSendText(mPlayerName,ID_ADD_PLAYER);

			// hide the menu
			Menu::getSingleton().hide();
			return true;
		}

		case ID_KICK: //////////////////////////////////////////////////////
		{
			// disconnect
			clientDisconnect();

			// get the message
			String msg;
			packetToString(packet, msg);
			Menu::getSingleton().setMessage("Kicked: " + msg);
			Menu::getSingleton().show();
			return true;
		}
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
			String playerName;
			packetToString(packet,playerName);

			// forward the new player name to all clients
			serverSendAllText(playerName,ID_ADD_PLAYER);

			// send the names of the existing players to the one that connected
			for (int i=0; i<(int)mPlayers.size(); i++)
				serverSendText( mPlayers[i].name,ID_ADD_PLAYER,packet->playerId);

			// update the player list
			PlayerInfo player;
			player.id = packet->playerId;
			player.name = playerName;
			player.wizardUID = Physics::getSingleton().newWizardThing()->getUID();
			mPlayers.push_back(player);
			PlayerList::getSingleton().addPlayer(playerName);

			return true;
		}

		case ID_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
		{
			// send the name of the map
			serverSendText(Renderer::getSingleton().getMapName(),ID_MAP_NAME,packet->playerId);
			return true;
		}

		case ID_DISCONNECTION_NOTIFICATION: //////////////////////////////////////////////////////
		case ID_CONNECTION_LOST: 
		{
			// find the client no longer connected
			for (int i=0; i<(int)mPlayers.size(); i++)
			{
				if (mPlayers[i].id == packet->playerId)
				{
                    // remove their name from the server list
					PlayerList::getSingleton().removePlayer(mPlayers[i].name);

					// remove their name from the clients lists
					serverSendAllText(mPlayers[i].name,ID_REMOVE_PLAYER);

					// remove their ip from the list
					mPlayers.erase(mPlayers.begin()+i);
				}
			}

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

void Multiplayer::stringToBitStream(String& string, BitStream& bs, int type)
{
	char cstr[STRING_MAX_LENGTH];
	strcpy(cstr, string);
	int len = (int)strlen(cstr) + 1;

	bs.Write(type);
	bs.Write(len);
	bs.Write(cstr,len);
}

//----------------------------------------------------------------------------

void Multiplayer::packetToString(Packet* packet, String& string)
{
	char cstr[STRING_MAX_LENGTH];
	int len, UID;

	BitStream bs(packet->data,packet->length,false);
	bs.Read(UID);
	bs.Read(len);
	bs.Read(cstr,len);

	string << cstr;
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