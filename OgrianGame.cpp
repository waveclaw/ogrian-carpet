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
#include "OgrianHUD.h"
#include "OgrianAIWizardThing.h"

template<> Ogrian::Game * Singleton< Ogrian::Game >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Game::Game()
{
	loadSounds();
}

//----------------------------------------------------------------------------

Game::~Game()
{

}

//----------------------------------------------------------------------------

void Game::frame(Real time)
{
	// tick the multiplayer
	Multiplayer::getSingleton().frame(time);

	// tick the physics
	Physics::getSingleton().frame(time);

	// tick the audio
	Audio::getSingleton().frame(time);

	// tick the input
	Menu::getSingleton().frame(time);
	Input::getSingleton().frame(time);
}

//----------------------------------------------------------------------------

void Game::loadSounds()
{
	SOUND_WHOOSH = Audio::getSingleton().loadSound("Media/sounds/whoosh2.mp3");
	SOUND_BANG = Audio::getSingleton().loadSound("Media/sounds/boom2.mp3");
}

//----------------------------------------------------------------------------

void Game::updateScores()
{
	if (Multiplayer::getSingleton().isClient()) return;

	// update the scores
	Hud::getSingleton().setScore(Physics::getSingleton().getTeam(0)->getScore());

	// propogate the new scores if this is a server
	if (Multiplayer::getSingleton().isServer()) 
		Multiplayer::getSingleton().updateScores();
}

//----------------------------------------------------------------------------

void Game::startGame()
{
	Renderer::getSingleton().getFrameListener()->setGameRunning(true);
	Renderer::getSingleton().getCameraThing()->setHealth(CONR("WIZARD_HEALTH"));

	// start the audio
	Audio::getSingleton().start();

	if (Multiplayer::getSingleton().wasClient()) startClientGame();
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
	// load AI Wizards
	for (int i=0; i<CONI("NUM_BOTS"); i++)
	{
		AIWizardThing* ai = new AIWizardThing( 
			Renderer::getSingleton().getCameraThing()->getPosition());

		Physics::getSingleton().addThing(ai);
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