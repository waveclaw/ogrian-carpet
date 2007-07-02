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
 * \file OgrianPacketEnum.h
 * \author RakkarSoft,  Mike Prosser
 * \brief  Enumerates the packets used in Ogrian Carpet
 */
#ifndef __OgrianPacketEnum_H__
#define __OgrianPacketEnum_H__

#include <MessageIdentifiers.h>
#define ID_RESERVED 9


/** 
 * \enum PacktID The unique ID of packet types.
 */
enum PacketType
{
	/**
	 * \note Ogrian Types
	*/
	ID_ADD_PLAYER = ID_USER_PACKET_ENUM + ID_RESERVED, /**< Player ADDed */
	ID_SET_CASTLE_SKIN, /**< My Castle's Skin */
	ID_KICK, /**< I kick you from the server */
	ID_KICKME, /**< I kick me from the server */
	ID_MAP_NAME,/**< Where am I? */
	ID_SET_WIZUID,/**< Even Wizards need ID */

	ID_UPDATE_THING, /**< The thing, uh, updated */
	ID_REMOVE_THING, /**< Easy come, easy go */

	ID_CLEAR_SCOREBOARD,/**< Even the winners start from zeo */
	ID_ADD_SCORE,/**< And a point goes to.. */

	ID_DIE, /**< KILL IT! KILL IT, BECAUSE IT'S DIFFERENT! */
	ID_TELEPORT, /**< It puts the wizards in the basket or it gets hurt. */
	ID_MESSAGE, /**< Like a little oceanic bottle, I tell you this. */
	ID_SETSPELLLEVEL, /**< Do your spells PWN! or just pwn!, now? */
	
	ID_PLAYSOUND,
	ID_THINGMESSAGE, /**< And the Thing goes Moooooo */

	ID_HUDMESSAGE, /**< Paging Doctor Butts, Doctor Seymore Butts in the living room? */

	ID_GHOST, /**< I see dead players!!! */
	ID_VICTORY /**< Teh winnnar is you! */
};

/** 
 * \enum PerrType is this a client, server, p2p network?
 */
enum PeerType {
	CLIENT, /**< clients, who only have 1 connection and don't do conflict rez */
	SERVER, /**< servers, who do confliict resolution and have many connections */
	PEER2PEER /**< TODO - is P2P even useful? Perhaps use for lobby-less setup */
};

#endif
