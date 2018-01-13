/*****************************************************************************
	Copyright 2007 Jeremiah Powell, 
	Based on OgrianMultiplayer Copyright 2004 Mike Prosser.


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
 * \file Server.h
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Storage for Server-side logic 
 **/
#ifndef SERVER_H_
#define SERVER_H_

#include "Multiplayer.h"

namespace Ogrian
{
	
/**
 * \class Server 
 * \brief Server class, which has several client connections to manage.
 */
class Server: public Multiplayer 
{
private:
	int mNumberClients; // usually 1 or more, never 0
	// TODO: map of client states (up/down/downed_risk/hung, unacked/acked, etc) 
protected:

    /**
     * update the internal State of the server
     **/	
	void updateState(const RakNetTime curTime);
	
public:
	/**
 	* Create a server
 	**/
	Server();
	
	/**
	 * stop the network interface so we can disconnect
	 **/
	~Server();

	/**
	 * connect to the network (convenience function)
	 **/
	void listen(void);	
	
	/**
	 * disconnect from the network
	 **/
	void disconnect(void);

	/**
	 * start the network interface so we can (dis)connect
 	**/
	void startup(void);

	/**
	 * set the number of Ports
	 * \parameter the number of ports > 1
	 **/
	void setNumberClients(const int);

	/**
	 * Get the number of ports
	 * \returns the number of ports
	 **/
	int getNumberClients(void) const;	
}; // end class Server

}; // end Ogrian namespace

#endif /*SERVER_H_*/
