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
OgrianPacketEnum.h
Original Author: RakkarSoft
Additional Authors: Mike Prosser

Description: This enumerates the packets used in Ogrian Carpet

/*------------------------------------*/

#ifndef __OgrianPacketEnum_H__
#define __OgrianPacketEnum_H__

enum PacketID
{
	//
	// RESERVED TYPES - DO NOT CHANGE THESE
	//
	// Ignore these:
	ID_PING, // Ping (internal use only)
	ID_PONG, // Pong.  Returned if we ping a system we are not connected so
	ID_REQUEST_STATIC_DATA, // Someone asked for our static data (internal use only)
	ID_CONNECTION_REQUEST, // Asking for a new connection (internal use only)
	ID_SYNCHRONIZE_MEMORY, // Synchronize memory (internal use only)
	ID_RPC, // Remote procedure call (internal use only)
	ID_RECEIVED_RELAYED_STATIC_DATA, // Server / Client only - Sending the static data for another system (internal use only)
	ID_BROADCAST_PINGS, // Server / Client only - The server is broadcasting the pings of all players in the game (internal use only)
	ID_SET_RANDOM_NUMBER_SEED, // Server / Client only - The server is broadcasting a random number seed (internal use only)
	// Handle these below
	ID_REMOTE_DISCONNECTION_NOTIFICATION, // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
	ID_REMOTE_CONNECTION_LOST, // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
	ID_REMOTE_NEW_INCOMING_CONNECTION, // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
	ID_TIMESTAMP, // Use this to indicate there is a timestamp.  Follow immediately with the timestamp
	ID_CONNECTION_REQUEST_ACCEPTED, // New connection is ok
	ID_CONNECTION_RESUMPTION, // Someone who was already connected connected again (For example they crashed and reconnected before we noticed they dropped).
	ID_NEW_INCOMING_CONNECTION, // Someone just connected to us
	ID_NO_FREE_INCOMING_CONNECTIONS,  // The system we tried to connect to cannot accept further connections
	ID_DISCONNECTION_NOTIFICATION, // We were gracefully disconnected from the peer (or server) specified in in packet->playerID
	ID_CONNECTION_LOST, // The network terminated the connection with a remote peer due to an inability to communicate. Peer specified in packet->playerId specifies who
	ID_RECEIVED_STATIC_DATA, // We got someone's static data
	ID_INVALID_PASSWORD, // Invalid password
	ID_MODIFIED_PACKET,  // This is sent to the game to indicate an incoming packet was modified.  The player it is for is specified in the packet
	ID_REMOTE_PORT_REFUSED, // The remote host is not accepting data on this port
	ID_VOICE_PACKET, // A voice packet
	ID_REMOTE_EXISTING_CONNECTION, // Client only - we just connected and were informed of an existing connection
	ID_UPDATE_DISTRIBUTED_NETWORK_OBJECT, // Indicates creation of a distributed network object. (for internal use)
	ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_ACCEPTED, // Client creation of a distributed network object was accepted (for internal use)
	ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_REJECTED, // Client creation of a distributed network object was rejected (for internal use)
	ID_RESERVED5, // For future versions
	ID_RESERVED6, // For future versions
	ID_RESERVED7, // For future versions
	ID_RESERVED8, // For future versions
	ID_RESERVED9, // For future versions
 	//-------------------------------------------------------------------------------------------------------------
 	

	//
	// Ogrian Types
	//
	ID_ADD_PLAYER,
	ID_KICK,
	ID_GET_MAP_NAME,
	ID_MAP_NAME,
	ID_SET_WIZUID,

	ID_UPDATE_THING, 
	ID_REMOVE_THING,

	ID_SETSCORE,
	ID_CLEAR_SCOREBOARD,
	ID_ADD_SCORE,
};

#endif