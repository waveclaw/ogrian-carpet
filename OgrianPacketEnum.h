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
	ID_PING_OPEN_CONNECTIONS, // Only reply to the unconnected ping if we have open connections
	ID_REQUEST_STATIC_DATA, // Someone asked for our static data (internal use only)
	ID_CONNECTION_REQUEST, // Asking for a new connection (internal use only)
	ID_SECURED_CONNECTION_RESPONSE, // Connecting to a secured server/peer
	ID_SECURED_CONNECTION_CONFIRMATION, // Connecting to a secured server/peer
	ID_RPC, // Remote procedure call (internal use only)
	ID_BROADCAST_PINGS, // Server / Client only - The server is broadcasting the pings of all players in the game (internal use only)
	ID_SET_RANDOM_NUMBER_SEED, // Server / Client only - The server is broadcasting a random number seed (internal use only)
	ID_RPC_WITH_TIMESTAMP, // Same as RPC, but treat the first 4 bytes as a timestamp
	// Handle these below.  Possible recipients in [...]
	ID_PONG, // [CLIENT|PEER] Pong.  Returned if we ping a system we are not connected so.  First byte is ID_PONG, second 4 bytes is the ping, following bytes is system specific enumeration data.
	ID_RSA_PUBLIC_KEY_MISMATCH, // [CLIENT|PEER] We preset an RSA public key which does not match what the system we connected to is using.
	ID_REMOTE_DISCONNECTION_NOTIFICATION, // [CLIENT] In a client/server enviroment, a client other than ourselves has disconnected gracefully.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_CONNECTION_LOST, // [CLIENT] In a client/server enviroment, a client other than ourselves has been forcefully dropped.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_NEW_INCOMING_CONNECTION, // [CLIENT] In a client/server enviroment, a client other than ourselves has connected.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_EXISTING_CONNECTION, // [CLIENT] On our initial connection to the server, we are told of every other client in the game.  Packet::playerID is modified to reflect the playerID of this client.
	ID_REMOTE_STATIC_DATA, // [CLIENT] - Got the data for another client
	ID_CONNECTION_BANNED, // [PEER|CLIENT] We are banned from the system we attempted to connect to.
	ID_CONNECTION_REQUEST_ACCEPTED, // [CLIENT] In a client/server enviroment, our connection request to the server has been accepted.
	ID_NEW_INCOMING_CONNECTION, // [PEER|SERVER] A remote system has successfully connected.
	ID_NO_FREE_INCOMING_CONNECTIONS, // [PEER|CLIENT] The system we attempted to connect to is not accepting new connections.
	ID_DISCONNECTION_NOTIFICATION, // [PEER|SERVER|CLIENT] The system specified in Packet::playerID has disconnected from us.  For the client, this would mean the server has shutdown.
	ID_CONNECTION_LOST, // [PEER|SERVER|CLIENT] Reliable packets cannot be delivered to the system specifed in Packet::playerID.  The connection to that system has been closed.
	ID_TIMESTAMP, // [PEER|SERVER|CLIENT] The four bytes following this byte represent an unsigned long which is automatically modified by the difference in system times between the sender and the recipient. Requires that you call StartOccasionalPing.
	ID_RECEIVED_STATIC_DATA, // [PEER|SERVER|CLIENT] We got a bitstream containing static data.  You can now read this data.  This packet is transmitted automatically on connections, and can also be manually sent.
	ID_INVALID_PASSWORD, // [PEER|CLIENT] The remote system is using a password and has refused our connection because we did not set the correct password.
	ID_MODIFIED_PACKET, // [PEER|SERVER|CLIENT] A packet has been tampered with in transit.  The sender is contained in Packet::playerID.
	ID_REMOTE_PORT_REFUSED, // [PEER|SERVER|CLIENT] The remote host is not accepting data on this port.  This only comes up when connecting to yourself on the same computer and there is no bound socket on that port.
	ID_VOICE_PACKET, // [PEER] This packet contains voice data.  You should pass it to the RakVoice system.
	ID_UPDATE_DISTRIBUTED_NETWORK_OBJECT, // [CLIENT|SERVER] Indicates creation or update of a distributed network object.  Pass to DistributedNetworkObjectManager::Instance()->HandleDistributedNetworkObjectPacket
	ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_ACCEPTED, // [CLIENT] Client creation of a distributed network object was accepted.  Pass to DistributedNetworkObjectManager::Instance()->HandleDistributedNetworkObjectPacketCreationAccepted
	ID_DISTRIBUTED_NETWORK_OBJECT_CREATION_REJECTED, // [CLIENT] Client creation of a distributed network object was rejected.  Pass to DistributedNetworkObjectManager::Instance()->HandleDistributedNetworkObjectPacketCreationRejected
	ID_AUTOPATCHER_REQUEST_FILE_LIST, // [PEER|SERVER|CLIENT] Request for a list of downloadable files. Pass to Autopatcher::SendDownloadableFileList
	ID_AUTOPATCHER_FILE_LIST, // [PEER|SERVER|CLIENT] Got a list of downloadable files. Pass to Autopatcher::OnAutopatcherFileList
	ID_AUTOPATCHER_REQUEST_FILES, // [PEER|SERVER|CLIENT] Request for a particular set of downloadable files. Pass to Autopatcher::OnAutopatcherRequestFiles
	ID_AUTOPATCHER_SET_DOWNLOAD_LIST, // [PEER|SERVER|CLIENT] Set the list of files that were approved for download and are incoming. Pass to Autopatcher::OnAutopatcherSetDownloadList
	ID_AUTOPATCHER_WRITE_FILE, // [PEER|SERVER|CLIENT] Got a file that we requested for download.  Pass to Autopatcher::OnAutopatcherWriteFile
	ID_QUERY_MASTER_SERVER, // [MASTERSERVER] Request to the master server for the list of servers that contain at least one of the specified keys
	ID_MASTER_SERVER_DELIST_SERVER, // [MASTERSERVER] Remove a game server from the master server.
	ID_MASTER_SERVER_UPDATE_SERVER, // [MASTERSERVER|MASTERCLIENT] Add or update the information for a server.
	ID_MASTER_SERVER_SET_SERVER, // [MASTERSERVER|MASTERCLIENT] Add or set the information for a server.
	ID_RELAYED_CONNECTION_NOTIFICATION, // [MASTERSERVER|MASTERCLIENT] This message indicates a game client is connecting to a game server, and is relayed through the master server.
	ID_ADVERTISE_SYSTEM, // [PEER|SERVER|CLIENT] Inform a remote system of our IP/Port.
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
	ID_KICKME,
	ID_MAP_NAME,
	ID_SET_WIZUID,

	ID_UPDATE_THING, 
	ID_REMOVE_THING,

	ID_SETSCORE,
	ID_CLEAR_SCOREBOARD,
	ID_ADD_SCORE,

	ID_SETHEALTH,
	ID_DIE,
	ID_GHOST,
	ID_MESSAGE,
};

#endif