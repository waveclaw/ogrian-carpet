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

#include "PacketEnumerations.h"
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
	void startServer();

	// start a client
	void startClient();

	// send a message to the server
	void clientSendMessage(char* message);

	// send a message to a client
	void serverSendMessage(char* message, int player);

	// recieve packets on the client
	void clientRecieve();

	// receive packets on the server
	void serverRecieve();

	// disconnect from the server
	void clientDisconnect();

	// disconnect all clients from this server
	void serverDisconnect();

private:
	Multiplayer();
	bool mIsServer;

};

}
#endif