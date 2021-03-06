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
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief Handles all of the multiplayer networking code.
 **/
#include "OgrianMultiplayer.h"

using namespace Ogre;

template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

Multiplayer::Multiplayer()
{
	mActive = false;
	mClientReady = false;
	mClientReadyToDisconnect = false;
	mClient = 0;
	mServer = 0;
	mWasClient = false;

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
	mPlayerName = Menu::getSingleton().getChosenName();
	mServerName = Menu::getSingleton().getChosenServer();

	// trim the name
	std::string name = mPlayerName;
	mPlayerName = name.substr(0,STRING_MAX_LENGTH);
	
	// trim the server
	std::string server = mServerName;
	mServerName = server.substr(0,STRING_MAX_LENGTH);
}

//----------------------------------------------------------------------------

bool Multiplayer::wasClient()
{
	return mWasClient;
}

//----------------------------------------------------------------------------

void Multiplayer::clientStart()
{
	assert(!mActive);

	loadConfig();

	mIsServer = false;
	mActive = true;
	mWasClient = true;

	mClient = RakNetworkFactory::GetRakClientInterface();

	// massage the name because RakNet is Stupid and wont take a const char*
	std::string name = mServerName;
	name = name.substr(0,16);
	char cn[16];
	strcpy(cn, name.c_str());

	Exception(Exception::ERR_NOT_IMPLEMENTED,"Connecting to: " + cn, "Multiplayer::client");

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger
	bool b = mClient->Connect(cn, CONI("PORT"), CONI("PORT")-1, 0, CONI("RAKNET_THREAD_SLEEP_TIME"));

	if (!b)
	{
		Exception(Exception::ERR_INTERNAL_ERROR, "Failed To Connect", "Multiplayer::clientStart");
		// Menu::getSingleton().setMessage(CONS("MSG_JOIN_CONNECT_FAIL"));
	}
	else Exception(Exception::ERR_NOT_IMPLEMENTED,"Client Started", "Multiplayer::client");
}

//----------------------------------------------------------------------------

void Multiplayer::serverStart()
{
	assert(!mActive);

	loadConfig();

	mIsServer = true; 
	mActive = true;

	mServer = RakNetworkFactory::GetRakServerInterface();

	// Starting the server is very simple.  4 players allowed (local + 3).
	// 0 means we don't care about a connectionValidationInteger, 
	bool b = mServer->Start(3, 0, CONI("RAKNET_THREAD_SLEEP_TIME"), CONR("PORT"));
	
	// error
	if (!b) Exception( Exception::ERR_INTERNAL_ERROR, "Error: Could Not Create Server.",
				"Multiplayer::serverStart" );

}

//----------------------------------------------------------------------------


void Multiplayer::serverAddCameraPlayer(Thing* cam)
{
	PlayerInfo server;
	server.id.binaryAddress = 0;
	server.name = mPlayerName + " (Serving)";

	server.wizardUID = cam->getUID();
	server.teamNum = cam->getTeamNum();

	mPlayers.clear();
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
		mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player, false);
	}
	else
	{
		mServer->Send(bitStream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, player, false);
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
		mServer->Send(bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true);	
	}
	else
	{
		mServer->Send(bitStream, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_PLAYER_ID, true);	
	}
}

//----------------------------------------------------------------------------

void Multiplayer::clientSend(String message, int type)
{
	BitStream stream;
	stringToBitStream(message,stream,type);

	clientSend(&stream);
}

//----------------------------------------------------------------------------

void Multiplayer::clientSend(int num, int type)
{
	BitStream stream;
	bs.Write(type);
	bs.Write(num);

	clientSend(&stream);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendHudText(String message, PlayerID player)
{
	serverSendText(message, ID_HUDMESSAGE, player);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendText(String message, int type, PlayerID player)
{
	BitStream bs;
	stringToBitStream(message,bs,type);

	serverSend(&bs,player);
}

//----------------------------------------------------------------------------

void Multiplayer::serverSendInt(int num, int type, PlayerID player)
{
	BitStream bs;
	bs.Write(type);
	bs.Write(num);

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

void Multiplayer::serverSendAllInt(int num, int type)
{
	BitStream bs;
	bs.Write(type);
	bs.Write(num);

	serverSendAll(&bs);
}

//----------------------------------------------------------------------------

void Multiplayer::frame(Real time)
{
	if (!mActive) return;

	if (mIsServer) serverRecieve();
	else clientRecieve();
}

//----------------------------------------------------------------------------

void Multiplayer::clientRecieve()
{
	if (!mActive) return;

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

	if (mClientReadyToDisconnect) clientDisconnect();
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
	mClient->Disconnect(10);
	RakNetworkFactory::DestroyRakClientInterface(mClient);

	// clear the player list
	PlayerList::getSingleton().clear();

	// reset the physics
	Physics::getSingleton().reset();
	
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
	mServer->Disconnect(10);
	RakNetworkFactory::DestroyRakServerInterface(mServer);

	// clear the player list
	PlayerList::getSingleton().clear();
	mPlayers.clear();

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

	/**
	 * Get the Packet Identifier
	 * Copied from Multiplayer.cpp (from RakNet)
	 * If the first byte is ID_TIMESTAMP, then we want the 5th byte
	 * Otherwise we want the 1st byte
	 * @param The packet to ID
	 * @return The ID of the Packet
	 **/
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

	std::ostringstream num("");
	num << pid.binaryAddress;
	LogManager::getSingleton().logMessage(String("Multiplayer::getWizardUID() failed, PlayerID not found: #") + num.str());
	return -1;
}

//----------------------------------------------------------------------------

PlayerID Multiplayer::getPlayerID(int wizardUID)
{
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		if (mPlayers[i].wizardUID == wizardUID) return mPlayers[i].id;
	}

	std::ostringstream num("");
	num << wizardUID;
	LogManager::getSingleton().logMessage(String("Multiplayer::getPlayerID() failed, WizardUID not found: #") + num.str() );
	PlayerID none;
	none.binaryAddress = 0;
	none.port = 0;
	return none;
}

//----------------------------------------------------------------------------

PlayerInfo* Multiplayer::getPlayerInfo(PlayerID pid)
{
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		if (mPlayers[i].id == pid) return &mPlayers[i];
	}

	std::ostringstream num("");
	num << pid.binaryAddress;
	LogManager::getSingleton().logMessage(String("Multiplayer::getPlayerInfo() failed, PlayerID not found: #") + num.str());
	return 0;
}

//----------------------------------------------------------------------------

void Multiplayer::updateScores()
{
	if (!isServer()) return;

	// clear all the scoreboards
	serverSendAllText(" ", ID_CLEAR_SCOREBOARD);
	PlayerList::getSingleton().clear();

	// re-add all the players + scores
	for (int i=0; i<(int)mPlayers.size(); i++)
	{
		int teamNum = mPlayers[i].teamNum;
		std::ostringstream num("");
		num << Physics::getSingleton().getTeam(teamNum)->getScore();

		String player = String("") + num.str() + "   " + mPlayers[i].name;
			
		serverSendAllText(player, ID_ADD_SCORE);
		PlayerList::getSingleton().addPlayer(player);
	}
}

//----------------------------------------------------------------------------

// note, mPlayers[0] is the server, not a client
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
			int pid, uid;

			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(uid);

			// set the camera UID
			Renderer::getSingleton().getCameraThing()->_setUID(uid);

			std::ostringstream num("");
			num << uid;
			LogManager::getSingleton().logMessage(String("Setting Wizard UID: ") + num.str());

			// send in the castle skin
			clientSend(Menu::getSingleton().getChosenCastleSkin(), ID_SET_CASTLE_SKIN);

			return true;
		}
		case ID_CONNECTION_REQUEST_ACCEPTED: //////////////////////////////////////////////////////
		{
			// oddly, nothing is done here
			Menu::getSingleton().setMessage("Connected, Loading .  .  .");
			return true;
		}
		case ID_MAP_NAME: //////////////////////////////////////////////////////
		{

			// get the name
			String map;
			packetToString(packet,map);

			String themap = (String)map.c_str();

			LogManager::getSingleton().logMessage(String("Got Map") + themap);

			// load the new map
			Renderer::getSingleton().loadMap(themap, false);
			
			// send our name and skin to the server
			clientSend(mPlayerName,ID_ADD_PLAYER);

			// hide the menu
			Menu::getSingleton().hide();
			Menu::getSingleton().clearLoadingScreen();

			// the client is now Ready
			mClientReady = true;
			return true;
		}
		case ID_KICK: //////////////////////////////////////////////////////
		{
			// disconnect
			mClientReadyToDisconnect = true;

			// get the message
			String msg;
			packetToString(packet, msg);
			Menu::getSingleton().setMessage(msg);
			Menu::getSingleton().show();
			Menu::getSingleton().button_disconnect();

			return true;
		}
		case ID_MESSAGE: //////////////////////////////////////////////////////
		{
			// get the message
			String msg;
			packetToString(packet,msg);

			// set it
			Hud::getSingleton().setMessage(msg);
			return true;
		}
		case ID_DIE: //////////////////////////////////////////////////////
		{
			int pid;

			Vector3 pos;

			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);

			Renderer::getSingleton().getCameraThing()->die();
			Renderer::getSingleton().getCameraThing()->setPosition(pos);
			return true;
		}
		case ID_GHOST: //////////////////////////////////////////////////////
		{
			Renderer::getSingleton().getCameraThing()->makeGhost();
			return true;
		}
		case ID_VICTORY: //////////////////////////////////////////////////////
		{
			Game::getSingleton().victory();
			return true;
		}
		case ID_TELEPORT: //////////////////////////////////////////////////////
		{
			int pid;

			Vector3 pos;

			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);

			Renderer::getSingleton().getCameraThing()->setPosition(pos);
			return true;
		}
		case ID_CLEAR_SCOREBOARD: //////////////////////////////////////////////////////
		{
			PlayerList::getSingleton().clear();
			return true;
		}
		case ID_ADD_SCORE: //////////////////////////////////////////////////////
		{
			// get the new scoreboard entry
			String player;
			packetToString(packet,player);

			// add it
			if (player.length() > 2)
				PlayerList::getSingleton().addPlayer(player);
			return true;
		}
		case ID_SETSPELLLEVEL: //////////////////////////////////////////////////////
		{
			// get the new level
			int pid, level;			
			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(level);

			// set it
			SpellManager::getSingleton().setLevel(level);
			return true;
		}
		case ID_HUDMESSAGE: //////////////////////////////////////////////////////
		{
			String msg;
			packetToString(packet,msg);

			Hud::getSingleton().setMessage(msg, true);
			return true;
		}
		case ID_PLAYSOUND: //////////////////////////////////////////////////////
		{
			// get the thing uid and sound index
			int pid, uid, sound;			
			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(uid);
			bs.Read(sound);

			// play the sound
			Thing* thing = Physics::getSingleton().getThing(uid);
			if (thing)
				thing->playSound(sound);
			return true;
		}
		case ID_THINGMESSAGE: //////////////////////////////////////////////////////
		{
			// get the thing uid and message
			int pid, uid, msg, val;			
			Vector3 pos;
			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(uid);
			bs.Read(msg);
			bs.Read(val);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);

			// handle the message
			Thing* thing = Physics::getSingleton().getThing(uid);
			if (thing)
				thing->handleMessage(msg, pos, val);
			return true;
		}
		default:
			;
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

			// get a new wizard
			WizardThing* wizard = new WizardThing(true);
			Physics::getSingleton().addThing(wizard);
			int wizardUID = wizard->getUID();

			// update the player list
			PlayerInfo player;
			player.id = packet->playerId;
			player.name = playerName;
			player.wizardUID = wizardUID;
			player.teamNum = wizard->getTeamNum();
			mPlayers.push_back(player);

			// send a message to the client telling it what its wizardUID is
			BitStream stream;
			stream.Write(ID_SET_WIZUID);
			stream.Write(wizardUID);
			serverSend(&stream, player.id);

			// set the health of the new wizard
			wizard->setHealth(CONI("WIZARD_HEALTH"));

			// update everyone's scoreboard
			updateScores();

			// sync the new client
			Physics::getSingleton().syncClient(player);

			// tell them where to start
			Vector3 spos = Game::getSingleton().getStartPos();
			BitStream clientStream;
			clientStream.Write(ID_TELEPORT);
			clientStream.Write(spos.x);
			clientStream.Write(spos.y);
			clientStream.Write(spos.z);
			serverSend(&clientStream, player.id);

			return true;
		}

		case ID_NEW_INCOMING_CONNECTION: //////////////////////////////////////////////////////
		{
			// send the name of the map
			LogManager::getSingleton().logMessage("Got New Incoming, sending map");
			serverSendText(Renderer::getSingleton().getMapName(),ID_MAP_NAME,packet->playerId);
			return true;
		}

		case ID_SET_CASTLE_SKIN: //////////////////////////////////////////////////////
		{			
			// get the skin number
			int pid, skin;			
			BitStream bs((const char*)packet->data, packet->length, false);
			bs.Read(pid);
			bs.Read(skin);

			// find the client
			for (int i=0; i<(int)mPlayers.size(); i++)
			{
				if (mPlayers[i].id == packet->playerId)
				{
					// set the skin
					mPlayers[i].castleSkin = skin;
					return true;
				}
			}
			return true;
		}

		case ID_KICKME://////////////////////////////////////////////////////
			serverSendText("Disconnected ",ID_KICK,packet->playerId);
		case ID_DISCONNECTION_NOTIFICATION: 
		case ID_CONNECTION_LOST: 
		{
			// find the client no longer connected
			for (int i=0; i<(int)mPlayers.size(); i++)
			{
				if (mPlayers[i].id == packet->playerId)
				{
					// remove their wizard
					Physics::getSingleton().getThing(mPlayers[i].wizardUID)->destroy();

					// remove their ip from the list
					mPlayers.erase(mPlayers.begin()+i);

					// update the scoreboards
					updateScores();
				}
			}

			return true;
		}
		default:
			;
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
		default:
			;
	}
	return false;
}

//----------------------------------------------------------------------------

void Multiplayer::clientRequestKick()
{
	// stop sending camera updates
	CameraThing* cam = Renderer::getSingleton().getCameraThing();
	
	if (cam!=0) cam->_setUID(0);

	clientSend(" ", ID_KICKME);
}

//----------------------------------------------------------------------------

void Multiplayer::killWizard(Thing* wizard, Vector3 pos)
{
	// find the wizard's player
	PlayerInfo* player = getPlayerInfo(getPlayerID(wizard->getUID()));

	// find the wizard's castle
	Thing* castle = Physics::getSingleton().getTeam(wizard->getTeamNum())->getCastle();
	
	if (castle)
	{
		pos = castle->getPosition();
	}
	else if (pos == Vector3(-1,-1,-1))
	{
        Vector3 offset;
		Real wdo = CONR("WIZARD_DEATH_OFFSET");
		offset.x = Math::RangeRandom(-wdo, wdo);
		offset.y = 0;
		offset.z = Math::RangeRandom(-wdo, wdo);
		pos = wizard->getPosition() + offset;
	}

	BitStream bs;
	bs.Write(ID_DIE);
	bs.Write(pos.x);
	bs.Write(pos.y);
	bs.Write(pos.z);
	serverSend(&bs, player->id);
}

//----------------------------------------------------------------------------

void Multiplayer::teleportWizard(Thing* wizard, Vector3 pos)
{
	// find the wizard's player
	PlayerInfo* player = getPlayerInfo(getPlayerID(wizard->getUID()));

	// find the wizard's castle
	Thing* castle = Physics::getSingleton().getTeam(wizard->getTeamNum())->getCastle();

	BitStream bs;
	bs.Write(ID_TELEPORT);
	bs.Write(pos.x);
	bs.Write(pos.y);
	bs.Write(pos.z);
	serverSend(&bs, player->id);
}
//----------------------------------------------------------------------------

void Multiplayer::ghostWizard(Thing* wizard)
{
	((WizardThing*)wizard)->makeGhost();

	// find the wizard's player
	PlayerInfo* player = getPlayerInfo(getPlayerID(wizard->getUID()));

	BitStream bs;
	bs.Write(ID_GHOST);
	serverSend(&bs, player->id);
}

//----------------------------------------------------------------------------

void Multiplayer::stringToBitStream(String& string, BitStream& bs, int type)
{
	//LogManager::getSingleton().logMessage(String("Server encoding string: ") + string);

	char cstr[STRING_MAX_LENGTH];
	strcpy(cstr, string.c_str());
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

	BitStream bs((const char*)packet->data,packet->length,false);
	bs.Read(UID);
	bs.Read(len);
	bs.Read(cstr,len);

	string += String(cstr);

	//LogManager::getSingleton().logMessage(String("Client decoded string: ") + string);
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
