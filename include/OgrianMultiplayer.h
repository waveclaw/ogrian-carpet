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

/**
 * \file OgrianMultiplayer.h
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief Handles all of the multiplayer networking code.
 **/
#ifndef __OgrianMultiplayer_H__
#define __OgrianMultiplayer_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include <RakNetworkFactory.h>
#include <BitStream.h>
#include <GetTime.h>
#include <RakPeerInterface.h>

#include "OgreConfigFile.h"
#include "OgrianPacketEnum.h"

#include "../Tests/stub.hpp"
//include "OgrianThing.h"
//include "OgrianMenu.h"
//include "OgrianHud.h"
//include "OgrianRenderer.h"
//include "OgrianPlayerList.h"
//include "OgrianPhysics.h"

#define UNASSIGNED_PLAYER_ID -1
#define STRING_MAX_LENGTH 256

using namespace Ogre;
using namespace RakNet;

namespace Ogrian
{

/** \struct PlayerInfo
 * \brief Collection of player-related data for networking
 * Not that this shouldn't be somewhere fundamental?
 */
struct PlayerInfo
{
	PlayerID id;
	String name;
	int teamNum;
	int wizardUID;
	int castleSkin;
};

/* \class Multiplayer OgrianMultiplayer.h "include/OgrianMultiplayer.h"
 * \brief The Multiplayer class handles all of the multiplayer networking code.
 */
class Multiplayer : public Singleton< Multiplayer >
{
public:
	/**
	 * Clean up after the multiplayer code, including kicking everyone, closing ports and thread shutdown. 
	 */
	virtual ~Multiplayer();

	/**
	 * Implement singleton, since the constructor is private.
	 * @return If no object exists, return a new object, else reference the existing 'single' object
	 */
    static Multiplayer& getSingleton(void);

	/**
	 * Start a server
	 */
	void serverStart();
		 
	/**
	 * Helper function to add the server-player's Camera to the world.
	 */  
	void serverAddCameraPlayer(Thing* cam);

	/**
	 * Start a client
	 */
	void clientStart();

	/**
	 * Send a message to the server 
	 * @param bitStream The RAW (native-binary-encoded) message
	 * @param reliable Does this message need reliable transport? 
	 */
	void clientSend(BitStream* bitStream, bool reliable=true);
	/**
	 * Send Text to the server 
	 * @param message The string to send
	 * @param type Of the enumerated types, which is this message? 
	 */
	void clientSend(String message, int type);
	/**
	 * Send a message to the server 
	 * @param bitStream The RAW (native-binary-encoded) message
	 * @param reliable Does this message need reliable transport? 
	 */
	void clientSend(int num, int type);

	// send a message to a client
	void serverSend(BitStream* bitStream, PlayerID player, bool reliable=true);
	void serverSendText(String message, int type, PlayerID player);
	void serverSendInt(int num, int type, PlayerID player);
	void serverSendAll(BitStream* bitStream, bool reliable=true);
	void serverSendAllText(String message, int type);
	void serverSendAllInt(int num, int type);

	// send a message to the client's Hud
	void serverSendHudText(String message, PlayerID player);

	// request a kick (to disconnect cleanly)
	void clientRequestKick();

	// force disconnect from the server
	void clientDisconnect();

	// disconnect all clients from this server
	void serverDisconnect();

	// true if this is an active server
	bool isServer();

	// true if this is an active client
	bool isClient();

	// true if this ever was an active client
	bool wasClient();

	// call this every frame
	void frame(Real time);

	// the number of clients connected
	int serverNumPlayers();

	// too see if we are connected
	bool isConnected();

	// turn an player ID into a Thing UID for the associated WizardThing
	int getWizardUID(PlayerID pid);

	// turn a wizard into a playerID
	PlayerID getPlayerID(int wizardUID);

	// turn an player ID into the assiciated PlayerInfo
	PlayerInfo* getPlayerInfo(PlayerID pid);

	// the number of clients connected
	int numClients();

	// send out updated scores
	void updateScores();

	PlayerInfo getClient(int i);

	// teleport a wizard
	void teleportWizard(Thing* wizard, Vector3 pos);

	// kill a player (default pos means random)
	void killWizard(Thing* wizard, Vector3 pos=Vector3(-1,-1,-1));
	
	// ghost a player
	void ghostWizard(Thing* wizard);

private:
	Multiplayer();
	bool mIsServer;
	bool mActive;
	bool mWasClient;
	bool mClientReady;
	bool mClientReadyToDisconnect;
	String mPlayerName;
	String mServerName;

	// Pointers to the interfaces of our server and client.
	RakPeerInterface* mClient;
	RakPeerInterface* mServer;

	// a list of connected players
	std::vector<PlayerInfo> mPlayers;

	// load the config file
	void loadConfig();

	// recieve packets on the client
	void clientRecieve();

	// receive packets on the server
	void serverRecieve();

	/**
	 * Get the Packet Identifier
	 * @param The packet to ID
	 * @return The ID of the Packet
	 **/
	PacketID getPacketIdentifier(Packet* p);

	// handle misc RakNet packets, returns true if the packet was handled
	bool handleRakPacket(Packet* p, PacketID pid);

	// handle misc Ogrian Server-recieved packets, returns true if the packet was handled
	bool serverHandlePacket(Packet* p, PacketID pid);
	
	// handle misc Ogrian Client-recieved packets, returns true if the packet was handled
	bool clientHandlePacket(Packet* p, PacketID pid);

	// get a string out of a packet
	void packetToString(Packet* packet, String& string);

	// turn a string into a bitstream
	void stringToBitStream(String& string, BitStream& bs, int type);
};

}
#endif
