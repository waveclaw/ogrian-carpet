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
#include "OgrianBuildingHeightMap.h"
#include "OgrianHUD.h"
#include "OgrianAIWizardThing.h"
#include "OgrianSkinManager.h"
#include "OgrianSpellManager.h"
#include "OgrianManaThing.h"
#include "OgrianGnomeThing.h"
#include "OgrianTickThing.h"
#include "OgrianSentinelThing.h"
#include "OgrianTowerThing.h"
#include "OgrianHutThing.h"
#include "OgrianFoliageThing.h"

template<> Ogrian::Game * Singleton< Ogrian::Game >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Game::Game()
{
	mPreGame = false;
	mLava = false;
	mStartPos = Vector3(0,0,0);
	loadSounds();
}

//----------------------------------------------------------------------------

Game::~Game()
{

}

//----------------------------------------------------------------------------

void Game::setLava(bool lava)
{
	mLava = lava;
}

//----------------------------------------------------------------------------

bool Game::getLava()
{
	return mLava;
}

//----------------------------------------------------------------------------

Vector3 Game::getStartPos()
{
	return mStartPos;
}

//----------------------------------------------------------------------------

void Game::frame(Real time)
{
	// tick the Hud
	Hud::getSingleton().frame();

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
			Hud::getSingleton().setMessage(CONS("HUD_VICTORY"));
		}
		else
		{
			// send the victory message to the right player
			PlayerID pid = Multiplayer::getSingleton().getPlayerID(winner->getUID());
			Multiplayer::getSingleton().serverSendText(CONS("HUD_VICTORY"), ID_MESSAGE, pid);
		}
	}

}

//----------------------------------------------------------------------------

void Game::loadSounds()
{
	SOUND_WHOOSH = Audio::getSingleton().loadSound("Media/sounds/whoosh2.mp3", true);
	SOUND_BANG = Audio::getSingleton().loadSound("Media/sounds/boom2.mp3", true);
	SOUND_HISS = Audio::getSingleton().loadSound("Media/sounds/hiss1.wav");
	SOUND_CHIRP = Audio::getSingleton().loadSound("Media/sounds/zap1.wav");
	SOUND_HUM = Audio::getSingleton().loadSound("Media/sounds/hum1.wav");
}

//----------------------------------------------------------------------------

void Game::updateScores()
{
	// propogate the new scores if this is a server
	if (Multiplayer::getSingleton().isServer()) 
		Multiplayer::getSingleton().updateScores();
}

//----------------------------------------------------------------------------

void Game::reset()
{
	Physics::getSingleton().reset();

	Renderer::getSingleton().createCameraThing();
	CameraThing* cam = Renderer::getSingleton().getCameraThing();
	cam->reset();
	Physics::getSingleton().addThing(cam);
}

//----------------------------------------------------------------------------

// note: this is called from Renderer::loadMap()
void Game::startGame(ConfigFile config)
{
	mConfig = config;

	// read the start locations
	mStartPos.x = atoi(mConfig.getSetting( "startX" ).c_str());
	mStartPos.z = atoi(mConfig.getSetting( "startZ" ).c_str());
	Renderer::getSingleton().getCameraThing()->setPosition(mStartPos);

	// start the frame listener
	Renderer::getSingleton().getFrameListener()->setGameRunning(true);

	// reset our wizard's health
	Renderer::getSingleton().getCameraThing()->setHealth(CONR("WIZARD_HEALTH"));

	// start the audio
	Audio::getSingleton().start();

	if (Multiplayer::getSingleton().isClient()) startClientGame();
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
	Hud::getSingleton().reinit();
	SpellManager::getSingleton().disableAllSpells();
	SpellManager::getSingleton().enableSpell(SPELL_CLAIM);
}

//----------------------------------------------------------------------------

void Game::startServerGame()
{
	Hud::getSingleton().reinit();
	SpellManager::getSingleton().disableAllSpells();
	SpellManager::getSingleton().enableSpell(SPELL_CLAIM);

	// activate pregame mode
	mPreGame = true;

	// make some foliage
	LogManager::getSingleton().logMessage("Making Foliage...");
	int foliageNum = atoi(mConfig.getSetting( "FoliageAmount" ).c_str());
	Real foliageLineMin = atoi(mConfig.getSetting( "FOLIAGE_LINE_MIN" ).c_str());
	Real foliageLineMax = atoi(mConfig.getSetting( "FOLIAGE_LINE_MAX" ).c_str());
	Real foliageScale = atoi(mConfig.getSetting( "FOLIAGE_SCALE" ).c_str());
	Real foliageScaleVar = atoi(mConfig.getSetting( "FOLIAGE_SCALE_VAR" ).c_str());

	// set up some foliage
	int i=0;
	Real size = HeightMap::getSingleton().getWorldSize();
	while (i<foliageNum)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > foliageLineMin && y < foliageLineMax)
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			Real scale = foliageScale + (Math::SymmetricRandom()-.5) * foliageScaleVar;

			Physics::getSingleton().addThing(new FoliageThing(scale,pos));
		}
	}
}

//----------------------------------------------------------------------------

void Game::serverEndPreGame()
{
	setPreGame(false);

	// set up some wild mana and huts
	Real numHuts = atoi(mConfig.getSetting( "NUM_HUTS" ).c_str());
	Real numMana = atoi(mConfig.getSetting( "MANA_START_NUM" ).c_str());
	Real manaAmount = atoi(mConfig.getSetting( "MANA_START_AMOUNT" ).c_str());
	int i=0;
	while(i<numMana)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * 1000.0;
        Real z = Math::SymmetricRandom() * 1000.0;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			ManaThing* mana = new ManaThing(manaAmount,pos);
			Physics::getSingleton().addThing(mana);
			
			if (i < numHuts)
			{
				pos = BuildingHeightMap::getSingleton().alignPosition(pos);
				HutThing* hut = new HutThing(pos);
				Physics::getSingleton().addThing(hut);
			}
		}
	}
	
	// build a list of start locations
	std::vector<Vector3> slocs;
	slocs.push_back(Vector3(
		atoi(mConfig.getSetting( "p1startX" ).c_str()) ,0,
		atoi(mConfig.getSetting( "p1startZ" ).c_str()) ));

	slocs.push_back(Vector3(
		atoi(mConfig.getSetting( "p2startX" ).c_str()) ,0,
		atoi(mConfig.getSetting( "p2startZ" ).c_str()) ));

	slocs.push_back(Vector3(
		atoi(mConfig.getSetting( "p3startX" ).c_str()) ,0,
		atoi(mConfig.getSetting( "p3startZ" ).c_str()) ));

	slocs.push_back(Vector3(
		atoi(mConfig.getSetting( "p4startX" ).c_str()) ,0,
		atoi(mConfig.getSetting( "p4startZ" ).c_str()) ));

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
	// make some foliage
	LogManager::getSingleton().logMessage("Making Foliage...");
	int foliageNum = atoi(mConfig.getSetting( "FoliageAmount" ).c_str());
	Real foliageLineMin = atoi(mConfig.getSetting( "FOLIAGE_LINE_MIN" ).c_str());
	Real foliageLineMax = atoi(mConfig.getSetting( "FOLIAGE_LINE_MAX" ).c_str());
	Real foliageScale = atoi(mConfig.getSetting( "FOLIAGE_SCALE" ).c_str());
	Real foliageScaleVar = atoi(mConfig.getSetting( "FOLIAGE_SCALE_VAR" ).c_str());

	// set up some foliage
	int i=0;
	Real size = HeightMap::getSingleton().getWorldSize();
	while (i<foliageNum)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > foliageLineMin && y < foliageLineMax)
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			Real scale = foliageScale + (Math::SymmetricRandom()-.5) * foliageScaleVar;

			Physics::getSingleton().addThing(new FoliageThing(scale,pos));
		}
	}

	// add some huts
	Real numHuts = atoi(mConfig.getSetting( "NUM_HUTS" ).c_str());
	i=0;
	while(i<numHuts)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			pos = BuildingHeightMap::getSingleton().alignPosition(pos);

			HutThing* hut = new HutThing(pos);
			Physics::getSingleton().addThing(hut);
		}
	}

	int numSkins = SkinManager::getSingleton().numSkins();

	// set up a team 
	ColourValue colour;
	colour.r = atoi(mConfig.getSetting( "MONSTERS_RED" ).c_str()) / 255.0;
	colour.g = atoi(mConfig.getSetting( "MONSTERS_GREEN" ).c_str()) / 255.0;
	colour.b = atoi(mConfig.getSetting( "MONSTERS_BLUE" ).c_str()) / 255.0;
	int teamNum = Physics::getSingleton().newTeam(colour);

	// set up some enemy towers
	Real numTowers = atoi(mConfig.getSetting( "NUM_TOWERS" ).c_str());
	i=0;
	while(i<numTowers)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			pos = BuildingHeightMap::getSingleton().alignPosition(pos);

			TowerThing* tower = new TowerThing(teamNum,pos);
			Physics::getSingleton().addThing(tower);
		}
	}

	// set up some ticks
	Real numTicks = atoi(mConfig.getSetting( "NUM_TICKS" ).c_str());
	i=0;
	while(i<numTicks)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			TickThing* tick = new TickThing(teamNum,pos);
			Physics::getSingleton().addThing(tick);
		}
	}

	// set up some sentinels
	Real numSentinels = atoi(mConfig.getSetting( "NUM_SENTINELS" ).c_str());
	i=0;
	while(i<numSentinels)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > CONR("BUILDING_MIN_GROUNDY"))
		{
			i++;
			Vector3 pos = Vector3(x,0,z);

			SentinelThing* sentinel = new SentinelThing(teamNum,pos);
			Physics::getSingleton().addThing(sentinel);
		}
	}

	// reset the score
	Renderer::getSingleton().getCameraThing()->getTeam()->setScore(0);
	Renderer::getSingleton().getCameraThing()->setBaseMana(0);
	Renderer::getSingleton().getCameraThing()->setActiveMana(0);

	// enable the claim and build spells
	Hud::getSingleton().reinit();
	SpellManager::getSingleton().disableAllSpells();
	SpellManager::getSingleton().enableSpell(SPELL_CLAIM);
	SpellManager::getSingleton().enableSpell(SPELL_BUILD);

	// reset the player
	Renderer::getSingleton().getCameraThing()->die();
	Renderer::getSingleton().getCameraThing()->setPosition(mStartPos);
	Hud::getSingleton().reinit();
}

//----------------------------------------------------------------------------

void Game::readConfig()
{
	// invert the mouse according to the config file
	if (CONB("MOUSE_Y_INVERT"));
	else Menu::getSingleton().button_invertMouseToggle();

	// set the music preference
	if (!CONB("MUSIC"))
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