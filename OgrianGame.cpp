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
#include "OgrianSkinManager.h"
#include "OgrianSpellManager.h"
#include "OgrianManaThing.h"
#include "OgrianTowerThing.h"
#include "OgrianHutThing.h"

template<> Ogrian::Game * Singleton< Ogrian::Game >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Game::Game()
{
	mPreGame = false;
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

	// victory check
	if (Multiplayer::getSingleton().isServer())
		serverVictoryCheck();

	// tick the clock
	Clock::getSingleton().frame();
}

//----------------------------------------------------------------------------

void Game::serverVictoryCheck()
{
	if (isPreGame()) return;

	int numLivingWizards = 0;
	Thing* winner = 0;
	for (int i=0; i<Physics::getSingleton().numTeams(); i++)
	{
		Team* team = Physics::getSingleton().getTeam(i);
		Thing* wizard =  Physics::getSingleton().getThing(team->getWizardUID());
		if (wizard && !((WizardThing*)wizard)->isGhost())
		{
			numLivingWizards++;
			winner = wizard;
		}
	}

	if (numLivingWizards == 1)
	{
		// game over
		if (winner == Renderer::getSingleton().getCameraThing())
		{
			// we are the winner
			Hud::getSingleton().setMessage("! ! !   a   winner   is   you   ! ! !");
		}
		else
		{
			// send the victory message to the right player
			PlayerID pid = Multiplayer::getSingleton().getPlayerID(winner->getUID());
			Multiplayer::getSingleton().serverSendText("! ! !  a winner is you  ! ! !", ID_MESSAGE, pid);
		}
	}

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

void Game::setPreGame(bool active)
{
	mPreGame = active;
}

//----------------------------------------------------------------------------

bool Game::isPreGame()
{
	return mPreGame;
}

//----------------------------------------------------------------------------

void Game::startClientGame()
{

}

//----------------------------------------------------------------------------

void Game::startServerGame()
{
	// activate pregame mode
	mPreGame = true;


}

//----------------------------------------------------------------------------

void Game::serverEndPreGame()
{
	setPreGame(false);

	// set up some wild mana
	int i=0;
	while(i<CONI("MANA_START_NUM"))
	{
        // Random translate
        Real x = Math::SymmetricRandom() * 1000.0;
        Real z = Math::SymmetricRandom() * 1000.0;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("FOLIAGE_LINE_MIN") && y < CONR("FOLIAGE_LINE_MAX"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			ManaThing* mana = new ManaThing(CONI("MANA_START_AMOUNT"),pos);
			Physics::getSingleton().addThing(mana);
			
			if (i < CONI("NUM_HUTS"))
			{
				HutThing* hut = new HutThing(pos);
				Physics::getSingleton().addThing(hut);
			}
		}
	}
	
	// build a list of start locations
	std::vector<Vector3> slocs;
	slocs.push_back(Vector3(100,0,100));
	slocs.push_back(Vector3(900,0,100));
	slocs.push_back(Vector3(100,0,900));
	slocs.push_back(Vector3(900,0,900));

	// chose an sloc for the camera
	Vector3 sloc = Vector3(500,0,500);
	if (slocs.size() > 0)
	{
		int index = Math::RangeRandom(0,slocs.size()-.1);
		sloc = slocs[index];
		slocs.erase(slocs.begin()+index);
	}

	sloc.y = HeightMap::getSingleton().getHeightAt(sloc.x, sloc.z);

	// kill the camera
	Renderer::getSingleton().getCameraThing()->die();
	Renderer::getSingleton().getCameraThing()->setPosition(sloc);

	// enable the build spell
	SpellManager::getSingleton().enableSpell(SPELL_BUILD);

	// kill all wizards
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* thing = Physics::getSingleton().getThingByIndex(i);

		if (thing && thing->getType() == WIZARDTHING)
		{
			// chose an sloc
			if (slocs.size() > 0)
			{
				int index = Math::RangeRandom(0,slocs.size()-.1);
				sloc = slocs[index];
				slocs.erase(slocs.begin()+index);
			}
			else sloc = Vector3(500,0,500);

			sloc.y = HeightMap::getSingleton().getHeightAt(sloc.x, sloc.z);

			Multiplayer::getSingleton().killWizard(thing, sloc);
		}
	}

	//  enable the build spell
	for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
	{
		PlayerInfo player = Multiplayer::getSingleton().getClient(i);
        Multiplayer::getSingleton().serverSendInt(SPELL_BUILD,ID_ENABLESPELL,player.id);
	}
}

//----------------------------------------------------------------------------

void Game::startSkirmishGame()
{
	// set up some wild mana
	Real size = HeightMap::getSingleton().getWorldSize();
	int i=0;
	while(i<CONI("MANA_START_NUM"))
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			ManaThing* mana = new ManaThing(CONI("MANA_START_AMOUNT"),pos);
			Physics::getSingleton().addThing(mana);

			if (i < CONI("NUM_HUTS"))
			{
				HutThing* hut = new HutThing(pos);
				Physics::getSingleton().addThing(hut);
			}
		}
	}

	int numSkins = SkinManager::getSingleton().numSkins();

	// set up some enemy towers
	i=0;
	while(i<CONI("NUM_BOTS"))
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			AIWizardThing* ai = new AIWizardThing(pos, int(Math::RangeRandom(0.8,numSkins+0.5)));
			Physics::getSingleton().addThing(ai);

			TowerThing* tower = new TowerThing(ai->getTeamNum(),pos);
			Physics::getSingleton().addThing(tower);
		}
	}

	// reset the score
	Renderer::getSingleton().getCameraThing()->getTeam()->setScore(0);

	// enable the build spell
	SpellManager::getSingleton().enableSpell(SPELL_BUILD);
}

//----------------------------------------------------------------------------

void Game::readConfig()
{
	// invert the mouse according to the config file
	ConfigFile config;
	config.load( "config.txt" );
	if (config.getSetting( "mouse_y_inv" ) == "true");
	else Menu::getSingleton().button_invertMouseToggle();

	// set the music preference
	if (config.getSetting( "music" ) == "false")
		Menu::getSingleton().button_musicToggle();
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