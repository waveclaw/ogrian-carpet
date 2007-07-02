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
 * \note Usage
 * Run two versions, one client and one server:
 * ./NetworkTest 26000 s &
 * ./NetworkTest 26000 c
 * Yes, 26000 is the Quake port.
 **/

/**
 * \file NetworkTest.cpp
 * \author Jeremiah Powell<waveclaw@hot_nospam_mail.com
 * \brief Unit test for the Ogrian Network system.
 * Unit testing the Network system can use either the existing system or
 * the stub system.  To use the stub system, define USE_STUB_H when compling. 
 */
#include "../Tests/Multiplayer.h"
#include <iostream>

using namespace std;
using namespace Ogrian;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include <stdio.h>
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT unused)
{
	char str[256];
#else // #ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[], char **envp)
{
	if (argc < 2) {
		cout << "What to port on localhost to use?" << endl;
	};
	if (argc < 3) {
		cout << "Server (s) or client (c)?" << endl;
		return 1;
	};
#endif // #ifdef WIN32

	//Ogre::LogManager& logmanager = Ogre::LogManager::getSingleton();
	//RakPeerInterface *peer = RakNetworkFactory::GetRakPeerInterface();	
	char* address; 
	unsigned short port;
	bool isServer;	
	
	#ifdef WIN32
		cout << "Port number?" << endl;
		gets(str);
		port = (unsigned short) atoi(str[0]);
		cout << "Using port: " << port << endl;
				cout << "(C) or (S)erver?" << endl;
		gets(str);
		if ((str[0]=='s')||(str[0]=='S'))
		{
			isServer = true;
		} else 
		{ 
			isServer = false;
		}
	#else
		port = (unsigned short) atoi(argv[1]);
		cout << "Using port: " << port << endl;
		 //pid_t parent = fork();
		 if ((argv[2][0] == 's')||(argv[2][0] == 'S')) {
		 	cout << "Choosing server" << endl;
		 	isServer = true;
		 } else {
		 	cout << "Choosing client" << endl;		 	
		 	isServer = false;
		 };
	#endif // WIN32

	Multiplayer &peer = Multiplayer::getSingleton();
	peer.setPort(port);
		 
	if (isServer)
	{
		cout << "Starting server...";cout.flush();
		address = "";// force raknet to use INADDR_ANY
		peer.setAddress(address);
		peer.startup(SERVER); //peer->Startup(MAX_CLIENTS, 30, &SocketDescriptor(SERVER_PORT,0), 1);
		cout << "started server." << endl;
		
		cout << "Listening." <<  endl;
		peer.listen();
		while (true) // !peer.isConnected()) 
		{
			cout << ".";cout.flush();
			sleep(1);
		};
		cout << "got a connection to listen!" << endl;
		
		
	// TODO - Add code body here
	
/* batch processing of queue'd messages!
 	get port off command line
 	creat a multiplayer::settings object
 	fork()
if parent (server) then
 	Create a multiplayer::server object
 		set server
 		set fake port
		listen for connection, echo connect
	echo strings to STDOUT in response to client object
	wait
	Create an object that implements multiplayer::streamble
		send this to the client
		update this on the client
loop
	send and listen for multiplayer::message exchanges
		first time, create a multiplayer::message
		first time, send to all with multiplayer::server
		check for a multiplayer::message
		generate a response multiplayer::message
end loop after 10th round
*/
 	cout << "Quitting server...";cout.flush();
	peer.disconnect();
	cout << "quit server." << endl;

	} else	{
		cout << "Starting client" <<  endl;
		address = "127.0.0.1";// force raknet to use localhost
		peer.setAddress(address);		
		peer.startup(CLIENT); // 	peer->Startup(1,30,&SocketDescriptor(), 1);
		cout << "Started client." << endl;
		
		cout << "Connecting...";cout.flush();
		peer.connect();
		if (peer.isConnected()) 
		{
			cout << "connected okay." << endl;
		} else {
			cout << "problem with connecting." << endl;
		}; // end if connecting
				
		// TODO - Add code body here	
/*if child (client) then
 	Create a multiplayer::client object
 		set client
 		set fake port
 		set IP = localhost
 		connect to server
 	Create an object that implements multiplayer::streamable
 	send this to the server
 	update this to the server
 	wait
 	echo strings to STDOUT in response to server updates
 loop
 	Send and listen for multiplayer::message exchanges
 		check for a multiplayer::message
 		generate a response multiplayer::message
 		send to server with multiplayer::client 
 end loop when kicked
 */ 
 	cout << "Quitting client." << endl;
	peer.disconnect();
	cout << "Quit client." << endl;
	}; // end if server or client

	// cannot do this with a reference	
	//delete peer; // force destroy to run.

    return 0;
}
