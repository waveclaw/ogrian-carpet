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
#include "OgrianConstants.h"
#include "OgrianPacketEnum.h"

#include "RakNetworkFactory.h"
#include "RakClientInterface.h"
#include "RakServerInterface.h"
#include "NetworkTypes.h"
#include "BitStream.h"

using namespace Ogre;

namespace Ogrian
{

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
	void clientSend(BitStream* bitStream);
	void clientSend(char* message);

	// send a message to a client
	void serverSend(BitStream* bitStream, PlayerID player);
	void serverSend(char* message, PlayerID player);
	void serverSendAll(BitStream* bitStream);
	void serverSendAll(char* message);

	// disconnect from the server
	void clientDisconnect();

	// disconnect all clients from this server
	void serverDisconnect();

	// true if this is an active server
	bool isServer();

	// true if this is an active client
	bool isClient();

	// call this every frame
	void frame();

	// the number of clients connected
	int serverNumPlayers();

	// propagate a map load message
	void serverLoadMap(String filename);

	// too see if we are connected
	bool isConnected();

private:
	Multiplayer();
	bool mIsServer;
	bool mActive;
	String mPlayerName;
	String mServerName;

	// Pointers to the interfaces of our server and client.
	RakClientInterface* mClient;
	RakServerInterface* mServer;

	// a list of connected players
	std::vector<PlayerID> mPlayers;
	std::vector<String> mPlayerNames;

	// load the config file
	void loadConfig();

	// recieve packets on the client
	void clientRecieve();

	// receive packets on the server
	void serverRecieve();

	// get the packet identifier
	PacketID getPacketIdentifier(Packet* p);

	// handle misc RakNet packets, returns true if the packet was handled
	bool handleRakPacket(Packet* p, PacketID pid);

	// handle misc Ogrian Server-recieved packets, returns true if the packet was handled
	bool serverHandlePacket(Packet* p, PacketID pid);
	
	// handle misc Ogrian Client-recieved packets, returns true if the packet was handled
	bool clientHandlePacket(Packet* p, PacketID pid);
};

}
#endif