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
OgrianMultiplayer.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This handles all of the multiplayer networking code.

/*------------------------------------*/

#include "OgrianMultiplayer.h"


using namespace Ogre;

template<> Ogrian::Multiplayer * Singleton< Ogrian::Multiplayer >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

Multiplayer::Multiplayer()
{

}

//----------------------------------------------------------------------------

Multiplayer::~Multiplayer()
{

}

//----------------------------------------------------------------------------

void Multiplayer::startClient()
{
	mIsServer = false;
}

//----------------------------------------------------------------------------

void Multiplayer::startServer()
{
	mIsServer = true; 
}

//----------------------------------------------------------------------------

void Multiplayer::clientSendMessage(char* message)
{
	assert(!mIsServer);

}

//----------------------------------------------------------------------------

void Multiplayer::serverSendMessage(char* message, int player)
{
	assert(mIsServer);

}

//----------------------------------------------------------------------------

void Multiplayer::clientRecieve()
{
	assert(!mIsServer);

}

//----------------------------------------------------------------------------

void Multiplayer::serverRecieve()
{
	assert(mIsServer);

}

//----------------------------------------------------------------------------

void Multiplayer::clientDisconnect()
{
	assert(!mIsServer);

}

//----------------------------------------------------------------------------

void Multiplayer::serverDisconnect()
{
	assert(mIsServer);

}

//----------------------------------------------------------------------------

Multiplayer& Multiplayer::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Multiplayer();
	}
    return Singleton<Multiplayer>::getSingleton();
}

//----------------------------------------------------------------------------

}