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
OgrianMultiplayer.h
Original Author: Mike Prosser
Additional Authors: 

Description: This handles all of the multiplayer networking code.

/*------------------------------------*/

#ifndef __OgrianMultiplayer_H__
#define __OgrianMultiplayer_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianPacketEnum.h"
#include "OgrianThing.h"

#include "RakNetworkFactory.h"
#include "RakClientInterface.h"
#include "RakServerInterface.h"
#include "NetworkTypes.h"
#include "BitStream.h"

using namespace Ogre;
using namespace RakNet;

namespace Ogrian
{

struct PlayerInfo
{
	PlayerID id;
	String name;
	int teamNum;
	int wizardUID;
};

// The Pysics class handles collision detection and notification among all Thing objects.
class Multiplayer : public Singleton< Multiplayer >
{
public:
	virtual ~Multiplayer();
    static Multiplayer& getSingleton(void);

	// start a server
	void serverStart();

	// start a client
	void clientStart();

	// send a message to the server
	void clientSend(BitStream* bitStream, bool reliable=true);
	void clientSendText(String message, int type);

	// send a message to a client
	void serverSend(BitStream* bitStream, PlayerID player, bool reliable=true);
	void serverSendText(String message, int type, PlayerID player);
	void serverSendAll(BitStream* bitStream, bool reliable=true);
	void serverSendAllText(String message, int type);

	// request a kick (to disconnect cleanly)
	void clientRequestKick();

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

	// kill a player
	void killWizard(Thing* wizard);

private:
	Multiplayer();
	bool mIsServer;
	bool mActive;
	bool mWasClient;
	bool mClientReady;
	bool mClientReadyToDisconnect;
	String mPlayerName;
	String mServerName;
	String mSkin;

	// Pointers to the interfaces of our server and client.
	RakClientInterface* mClient;
	RakServerInterface* mServer;

	// a list of connected players
	std::vector<PlayerInfo> mPlayers;

	// load the config file
	void loadConfig();

	// recieve packets on the client
	void clientRecieve();

	// receive packets on the server
	void serverRecieve();

	// disconnect from the server
	void clientDisconnect();

	// get the packet identifier
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