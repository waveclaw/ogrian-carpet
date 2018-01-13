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
 * \file Client.h
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Storage for Client objects 
 **/
 #ifndef CLIENT_H_
#define CLIENT_H_

#include "Multiplayer.h"

//----------------------------------------------------------------------------
// Ogrian 
namespace Ogrian
{
/**
 * \class Client 
 * \brief We are a client, implement good client behavior
 */
class Client: public Multiplayer 
{
     
public:	
	/**
	 * stop the network interface so we can disconnect
	 **/
	~Client();

	/**
	 * connect to the network
	 **/
	void connect(void);	

	/**
	 * disconnect from the network
	 **/
	void disconnect(void);

	/**
	 * start the network interface so we can (dis)connect
 	**/
	void startup(void);
}; // end class Client

} // end namespace orgian

#endif /*CLIENT_H_*/
