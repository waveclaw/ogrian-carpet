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
#include "OgrianGame.h"
#include "OgrianPhysics.h"
#include "OgrianMultiplayer.h"
#include "OgrianAIWizardThing.h"

template<> Ogrian::Game * Singleton< Ogrian::Game >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Game::Game()
{

}

//----------------------------------------------------------------------------

Game::~Game()
{

}

//----------------------------------------------------------------------------

void Game::startGame()
{
	Audio::getSingleton().start();
	Renderer::getSingleton().getFrameListener()->setGameRunning(true);
	Renderer::getSingleton().getCameraThing()->setHealth(CONR("WIZARD_HEALTH"));

	if (Multiplayer::getSingleton().isClient()) startClientGame();
	else if (Multiplayer::getSingleton().isServer()) startServerGame();
	else startSkirmishGame();
}

//----------------------------------------------------------------------------

void Game::startClientGame()
{

}

//----------------------------------------------------------------------------

void Game::startServerGame()
{

}

//----------------------------------------------------------------------------

void Game::startSkirmishGame()
{
	// make a team for the player
	Physics::getSingleton().newTeam(Renderer::getSingleton().getCameraThing()->getUID()); 

	// load AI Wizards
	for (int i=0; i<CONI("NUM_BOTS"); i++)
	{
		AIWizardThing* ai = new AIWizardThing( 
			Renderer::getSingleton().getCameraThing()->getPosition());

		Physics::getSingleton().addThing(ai);

		Physics::getSingleton().newTeam(ai->getUID());
	}
}

//----------------------------------------------------------------------------

Game& Game::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Game();
	}
	return Singleton<Game>::getSingleton();
}

//----------------------------------------------------------------------------

}