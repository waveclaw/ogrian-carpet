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
#include "OgrianHud.h"
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
	mClientReady = false;
	mClient = 0;
	mServer = 0;

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

	// get the map name
	clientSendText(" ", ID_GET_MAP_NAME);

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
	server.wizardUID = 0; // the server cameraThing is always UID 0, since it is the first created
	server.teamNum = Physics::getSingleton().addTeam(server.wizardUID);
	mPlayers.push_back(server);
	
	updateScores();
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
		else if (mClientReady && Physics::getSingleton().handleClientPacket(packet, packetId)) {}

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

	LogManager::getSingleton().logMessage(String("PlayerID not found: #") << pid.binaryAddress);
	return -1;
}

//----------------------------------------------------------------------------

PlayerID Multiplayer::getPlayerID(int wizardUID)
{
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		if (mPlayers[i].wizardUID == wizardUID) return mPlayers[i].id;
	}

	LogManager::getSingleton().logMessage(String("WizardUID not found: #") << wizardUID);
	return mPlayers[0].id;
}

//----------------------------------------------------------------------------

PlayerInfo* Multiplayer::getPlayerInfo(PlayerID pid)
{
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		if (mPlayers[i].id == pid) return &mPlayers[i];
	}

	LogManager::getSingleton().logMessage(String("PlayerID not found: #") << pid.binaryAddress);
	return 0;
}

//----------------------------------------------------------------------------

void Multiplayer::updateScores()
{
	if (!isServer()) return;

	LogManager::getSingleton().logMessage("Updating Scores");

	// update the server's scores
	Hud::getSingleton().setScore(Physics::getSingleton().getTeam(0)->getScore());

	// send each client its score
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		serverSendText(String("Score: ") << 
			Physics::getSingleton().getTeam(mPlayers[i].teamNum)->getScore(),
			ID_SETSCORE, mPlayers[i].id);
	}

	// clear all the scoreboards
	serverSendAllText(" ", ID_CLEAR_SCOREBOARD);
	PlayerList::getSingleton().clear();

	// re-add all the players + scores
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		String player = String("") << Physics::getSingleton().getTeam(mPlayers[i].teamNum)->getScore()
			<< "   " << mPlayers[i].name;
			
		serverSendAllText(player, ID_ADD_SCORE);
		PlayerList::getSingleton().addPlayer(player);
	}

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
		case ID_SET_WIZUID://////////////////////////////////////////////////////
		{
			int id, uid;
			BitStream bs;
			bs.Read(id);
			bs.Read(uid);

			// set the camera UID
			Renderer::getSingleton().getCameraThing()->_setUID(uid);
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

				// load the new map as a client
				Renderer::getSingleton().loadMap(map, false);

				// reconnect
				clientStart();
				return true;
			}

			// send our name to the server
			clientSendText(mPlayerName,ID_ADD_PLAYER);

			// hide the menu
			Menu::getSingleton().hide();

			// the client is now Ready
			mClientReady = true;
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

		case ID_DIE: //////////////////////////////////////////////////////
		{
			Renderer::getSingleton().getCameraThing()->die();
		}
		case ID_SETSCORE: //////////////////////////////////////////////////////
		{
			// get the new score
			String score;
			packetToString(packet,score);

			// set it
			Hud::getSingleton().setScore(score);
		}
		case ID_CLEAR_SCOREBOARD: //////////////////////////////////////////////////////
		{
			PlayerList::getSingleton().clear();
		}
		case ID_ADD_SCORE: //////////////////////////////////////////////////////
		{
			// get the new scoreboard entry
			String player;
			packetToString(packet,player);

			// add it
			if (strlen(player) > 2)
				PlayerList::getSingleton().addPlayer(player);
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

			// update the player list
			PlayerInfo player;
			player.id = packet->playerId;
			player.name = playerName;
			player.wizardUID = Physics::getSingleton().newWizardThing()->getUID();
			player.teamNum = Physics::getSingleton().addTeam(player.wizardUID);
			mPlayers.push_back(player);

			// send a message to the client telling it what its wizardUID is
			BitStream bs;
			bs.Write(ID_SET_WIZUID);
			bs.Write(player.wizardUID);
			serverSend(&bs, player.id);

			// update everyone's scoreboard
			updateScores();

			return true;
		}

		case ID_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
		{
			// do nothing
			return true;
		}

		case ID_GET_MAP_NAME: //////////////////////////////////////////////////////
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
					// remove their ip from the list
					mPlayers.erase(mPlayers.begin()+i);

					// update the scoreboards
					updateScores();
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