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
OgrianGame.h
Original Author: Mike Prosser
Additional Authors: 

Description: Game is a singleton that holds general game-related code, such as for
starting games and detecting victory. 
/*------------------------------------*/

#ifndef __OgrianGame_H__
#define __OgrianGame_H__

#include <Ogre.h>

using namespace Ogre;

namespace Ogrian
{

class Game : public Singleton< Game >
{
public:
	virtual ~Game();

	void startGame();

	static Game& getSingleton(void);
private:
	Game();

	void startSkirmishGame();
	void startServerGame();
	void startClientGame();
};

}

#endif