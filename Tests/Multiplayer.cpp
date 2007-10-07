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
 * \file OgrianMultiplayer.cpp
 * \autho Mike Prosser <mikeprosser@gmail.com>, Jeremiah Powell <waveclaw@hot_nospam_mail.com>
 * \brief Abstract the networking layer. 
 **/
#include "../Tests/Multiplayer.h"


using namespace Ogre;

// Dear Lord, abusing Singleton to make global variables.... :-(
//template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;

namespace Ogrian
{

/**
 * Initialize an empty multiplayer configuration
 * \exception Warns if the Interface fails to create
 **/
Multiplayer::Multiplayer()
{
	return Multiplayer(CONS("SERVER"), CONI("PORT"), CONI("SLEEPTIME"));
} // end contructor

/**
 * Initialize a multiplayer configuration based on details
 * \exception Warns if the Interface fails to create
 **/
Multiplayer::Multiplayer(const String Server, int port, int sleepTime)
{
	// get one or more peers
	mIsConnected = false;
	mAddress = new Server; // force raknet to use INADDR_ANY with ""
	mPort = port;
	mMaxConnections = CONI("MAX_CONNECTIONS");
	mSleepTime = sleepTime; // milliseconds
	mPeer = RakNetworkFactory::GetRakPeerInterface(); 
	if (0 == mPeer) 
	{
	Exception( Exception::ERR_INTERNAL_ERROR, 
  	           String("Error pre-initalizing network system. RakNet had an error: RakNetworkFactory::GetRakPeerInterface"),
                   String("Multiplayer::Multiplayer"));
	};	
} // end hidden constructor

/**
 * We're done with the network
 **/
Multiplayer::~Multiplayer()
{
	delete mInputBuffer;
	delete mOutputBuffer;
} // end destructor


/**
 * Is this a good connection?
 * \return True if is running and connected, false otherwise
 **/
bool Multiplayer::isConnected() const
{
		return(mIsConnected); 
		// return mPeer->IsConnected(mAddress); <-- not what you think it is
} // end isconnected

/**
 * get the current port for networking
 **/
unsigned short Multiplayer::getPort(void) const {
	return mPort;
} // end getPort

/**
 * get the current address for networking
 **/
const char* Multiplayer::getAddress(void) const {
	return mAddress->c_str(); 
} // end getAddress

/**
 * get the current maximum simultanious clients for networking
 **/
int Multiplayer::getMaxConnections(void) const {
	return mMaxConnections;
} // end getMaxConnections

/**
 * get the current simultanious clients for networking
 * \returns the number of addresses accepting connections
 **/
int Multiplayer::getConnectionCount(void) const
{
	int count = 0;
	for (int i = 0; i < getMaxConnections();i++)
		if (mPeer->GetSystemAddressFromIndex(i) != UNASSIGNED_SYSTEM_ADDRESS)
			count++;
	return count;
} // end getConnectCount

/**
 * set the port for the connection dynamically
 * \parameter the port (0 through 4,294,967,295). Uses 26000, the quake port, by default)
 **/
void Multiplayer::setPort(const unsigned short port) 
{
		mPort = port;
} // end setPort

/**
 * set the remote address for the connection dynamically
 * \parameter the address to listen on or connect to.  Use "" to listen on all addresses.
 **/
void Multiplayer::setAddress(const char *address) 
{
	mAddress = new String(address);
} // end setAddress

/**
 * set the current maximum simultanious clients for networking (for map support)
 * \parameter the number of connections to use.
 **/
void Multiplayer::setMaxConnections(const int connections) 
{
	mMaxConnections = connections;
} // end setMaxConnections

/**
 * set the connection status
 * \parameter the state of the connection
 **/
void Multiplayer::setConnected(const bool status)
{
	mIsConnected = status;
} // end setConnected

/**
 * set Time to Sleep in the network code
 * \parameter time to sleep before returning
 **/
void Multiplayer::setSleepTime(const int time)
{
	if(time > 0) 
	{
		mSleepTime = time;
	} else {
		mSleepTime = OGRIAN_DEFAULT_SLEEP_TIME; // no cheating!
	}
} // end setSleepTime

/**
 * get the Time to Sleep in the network code
 * \returns time to sleep in millisecons before returning
 **/
int Multiplayer::getSleepTime(void) const
{
	return mSleepTime;
} // end getSleepTime

/**
 * Put this packet into the buffer to send at the next opportunity
 * the return value is meaningful when we don't let the buffers get beyond 1 packet
 * per cycle. This is also the best performance point for a spin-buffer based packet buffer.
 * \parameter the packet to send
 * \returns true if the buffer has "something" in it at least
 **/	
bool Multiplayer::send(Packet *p) const {
	mOutputBuffer->put(p);
	return !mOutputBuffer->empty();
} // end send

/**
 * Get the latest packet from the buffer
 * \returns the packet requested or null if no packet found
 **/
Packet* Multiplayer::receive() const
{
	return mInputBuffer->get();
};	// end receive


/**
 * update the internal State of the server
**/	
void Multiplayer::updateState(const RakNetTime curTime)
{
	Packet *p = mPeer->Receive();
	while (p)
	{
		switch (p->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			isConnected=true;
			break;
			// print out errors
		case ID_CONNECTION_ATTEMPT_FAILED:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: Connection Attempt Failed: ", 
  	    		"Multiplayer");		
			isConnected=false;
			break;
		case ID_ALREADY_CONNECTED:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: Already Connected: ", 
  	    		"Multiplayer");		
  	    	break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: NO Free Incomming Connections: ", 
  	    		"Multiplayer");		
			isConnected=false;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: Disconnected: ", 
  	    		"Multiplayer");		
			isConnected=false;
			break;
		case ID_CONNECTION_LOST:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: Connection Lost: ", 
  	    		"Multiplayer");		
			isConnected=false;
			break;
		case ID_MODIFIED_PACKET:
			Exception( Exception::ERR_NOT_IMPLEMENTED, 
  	    		"Error: Packed Modified In-flight: ", 
  	    		"Multiplayer");		
			break;
		}
		peer->DeallocatePacket(p);
		p = peer->Receive();
	
	}

	if ( curTime > flipConnectionTime )
	{
		if (isConnected)
		{
			Disconnect();			
		}
		else
		{
			Connect();
		};
		flipConnectionTime = 
			curTime + OGRIAN_BASETIME + ( randomMT() % OGRIAN_RANDOM_MULTIPLIER );		
	}

	if (curTime>nextSendTime && isConnected)
	{
		peer->Send((const char*)&randomData,RANDOM_DATA_SIZE,HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
		nextSendTime = curTime + OGRIAN_BASETIME + ( randomMT() % OGRIAN_RANDOM_MULTIPLIER );
	}
}; // end update

}; // end namespace ogrian