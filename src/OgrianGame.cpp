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
 *------------------------------------*/

#include "OgrianGame.h"

template<> Ogrian::Game * Singleton< Ogrian::Game >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Game::Game()
{
	mPreGame = false;
	mLava = false;
	mHasWon = false;
	mSkirmishMode = false;

	mStartPos = Vector3(0,0,0);
	loadSounds();

	mMapMusic = CONS("THEME_MUSIC");
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

ConfigFile Game::getConfig()
{
	return mConfig;
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
	if (Multiplayer::getSingleton().isServer() && !mHasWon)
		serverVictoryCheck();
	else if (mSkirmishMode && !mHasWon)
		skirmishVictoryCheck();

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
		if (team)
		{
			Thing* wizard =  Physics::getSingleton().getThing(team->getWizardUID());
			if (wizard && !((WizardThing*)wizard)->isGhost())
			{
				numLivingWizards++;
				winner = wizard;
			}
		}
	}

	if (numLivingWizards == 1)
	{
		// game over
		if (winner == Renderer::getSingleton().getCameraThing())
		{
			// we are the winner
			victory();
		}
		else
		{
			// send the victory message to the right player
			PlayerID pid = Multiplayer::getSingleton().getPlayerID(winner->getUID());
			Multiplayer::getSingleton().serverSendInt(0, ID_VICTORY, pid);
		}
	}
}

//----------------------------------------------------------------------------

void Game::skirmishVictoryCheck()
{
	// look for enemies
	WizardThing* cam = Renderer::getSingleton().getCameraThing();
	Team* team = 0;
	
	if (cam) team = cam->getTeam();

	// note:  100000000000 > sizeof(signed long)  --jdpowell 20050612
	if (team && team->getNearestEnemyBuilding(cam, 2147483648) == 0)
	{
		// we are the winner
		victory();
	}
}

//----------------------------------------------------------------------------

void Game::victory()
{
	if (mHasWon) return;

	Hud::getSingleton().setMessage(CONS("HUD_VICTORY"));

	int volume = Menu::getSingleton().getChosenMusicVolume();
	Audio::getSingleton().playSong(CONS("THEME_MUSIC"), volume);

	mHasWon = true;
}

//----------------------------------------------------------------------------

void Game::loadSounds()
{
	SOUND_WHOOSH = Audio::getSingleton().loadSound("Media/sounds/whoosh2.mp3", true);
	SOUND_BANG = Audio::getSingleton().loadSound("Media/sounds/boom2.mp3", true);
	SOUND_HISS = Audio::getSingleton().loadSound("Media/sounds/hiss1.wav");
	SOUND_CHIRP = Audio::getSingleton().loadSound("Media/sounds/zap1.wav");
	SOUND_HUM = Audio::getSingleton().loadSound("Media/sounds/hum1.wav");
	SOUND_CRUNCH = Audio::getSingleton().loadSound("Media/sounds/crunch.ogg");
	SOUND_TELEPORT = Audio::getSingleton().loadSound("Media/sounds/teleport.ogg", true);
	SOUND_HEAL = Audio::getSingleton().loadSound("Media/sounds/heal.ogg");
}

//----------------------------------------------------------------------------

void Game::updateScores()
{
	// propogate the new scores if this is a server
	if (Multiplayer::getSingleton().isServer()) 
		Multiplayer::getSingleton().updateScores();
}

//----------------------------------------------------------------------------

Real Game::getManaDropMultiplier(int teamNum)
{
	Team* team = Physics::getSingleton().getTeam(teamNum);
	
	if (!team) return 1;

	if (team->getWizardUID() >= 0) return 1;

	return atof(mConfig.getSetting("MANA_DROP_MULTIPLIER").c_str());
}

//----------------------------------------------------------------------------

String Game::getMapMusic()
{
	return mMapMusic;
}

//----------------------------------------------------------------------------

void Game::reset()
{
	mSkirmishMode = false;

	Physics::getSingleton().reset();

	HealthBarManager::getSingleton().clear();
	DotManager::getSingleton().reset();

	Renderer::getSingleton().createCameraThing();
	CameraThing* cam = Renderer::getSingleton().getCameraThing();
	cam->reset();
	Physics::getSingleton().addThing(cam);
}

//----------------------------------------------------------------------------

// note: this is called from Renderer::loadMap()
void Game::startGame(ConfigFile config)
{
	reset();

	mConfig = config;

	// read the start location
	mStartPos.x = atoi(mConfig.getSetting( "startX" ).c_str());
	mStartPos.z = atoi(mConfig.getSetting( "startZ" ).c_str());
	mStartPos.y = HeightMap::getSingleton().getHeightAt(mStartPos.x, mStartPos.z);
	Renderer::getSingleton().getCameraThing()->setPosition(mStartPos);

	// start the frame listener
	Renderer::getSingleton().getFrameListener()->setGameRunning(true);

	// reset our wizard
	Renderer::getSingleton().getCameraThing()->setHealth(CONR("WIZARD_HEALTH"));

	// start the audio
	Audio::getSingleton().start();

	// set the music
	mMapMusic = config.getSetting("MAP_MUSIC").c_str();

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
	SpellManager::getSingleton().setLevel(-2);
}

//----------------------------------------------------------------------------

void Game::startServerGame()
{
	Hud::getSingleton().reinit();
	SpellManager::getSingleton().setLevel(-2);

	// activate pregame mode
	mPreGame = true;
}

//----------------------------------------------------------------------------

void Game::serverEndPreGame()
{
	setPreGame(false);

	// build a list of start locations
	std::vector<Vector3> slocs;

	if (Menu::getSingleton().getChosenHostMode() == 0)
	{
		// set up an enemy team 
		ColourValue colour;
		colour.r = atoi(mConfig.getSetting( "MONSTERS_RED" ).c_str()) / 255.0;
		colour.g = atoi(mConfig.getSetting( "MONSTERS_GREEN" ).c_str()) / 255.0;
		colour.b = atoi(mConfig.getSetting( "MONSTERS_BLUE" ).c_str()) / 255.0;
		int teamNum = Physics::getSingleton().newTeam(colour);

		// read in the image file
		loadMapThingsFromImage("MultiplayerThingsTexture", mConfig, HeightMap::getSingleton().getWorldSize(), teamNum);

		// load the starting locations
		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);
			if (thing->getType() == PLAYERMARKERTHING)
			{
				slocs.push_back(thing->getPosition());
				thing->destroy();
			}
		}
	}
	else
	{
		loadMapThingsSparsely(mConfig, HeightMap::getSingleton().getWorldSize());

		// build a list of start locations
		Real x = atoi(mConfig.getSetting( "p1startX" ).c_str());
		Real z = atoi(mConfig.getSetting( "p1startZ" ).c_str());
		Vector3 spawn1(x,HeightMap::getSingleton().getHeightAt(x,z), z);

		x = atoi(mConfig.getSetting( "p2startX" ).c_str());
		z = atoi(mConfig.getSetting( "p2startZ" ).c_str());
		Vector3 spawn2(x,HeightMap::getSingleton().getHeightAt(x,z), z);

		x = atoi(mConfig.getSetting( "p3startX" ).c_str());
		z = atoi(mConfig.getSetting( "p3startZ" ).c_str());
		Vector3 spawn3(x,HeightMap::getSingleton().getHeightAt(x,z), z);

		x = atoi(mConfig.getSetting( "p4startX" ).c_str());
		z = atoi(mConfig.getSetting( "p4startZ" ).c_str());
		Vector3 spawn4(x,HeightMap::getSingleton().getHeightAt(x,z), z);

		if (spawn1.length() > 1) slocs.push_back(spawn1);
		if (spawn2.length() > 1) slocs.push_back(spawn2);
		if (spawn3.length() > 1) slocs.push_back(spawn3);
		if (spawn4.length() > 1) slocs.push_back(spawn4);
	}


	// chose an sloc for the camera
	Vector3 sloc = Vector3(500,0,500);
	if (slocs.size() > 1)
	{
		int index = Math::RangeRandom(0,slocs.size()-.1);
		sloc = slocs[index];
		slocs.erase(slocs.begin()+index);
	}
	else if (slocs.size() > 0)
		sloc = slocs[0];

	sloc.y = HeightMap::getSingleton().getHeightAt(sloc.x, sloc.z);

	// kill the camera
	Renderer::getSingleton().getCameraThing()->die();
	Renderer::getSingleton().getCameraThing()->setPosition(sloc);

	// enable the build spell
	SpellManager::getSingleton().setLevel(-1);

	// kill all wizards
	for (int i=0; i<Physics::getSingleton().numThings(); i++)
	{
		Thing* thing = Physics::getSingleton().getThingByIndex(i);

		if (thing && thing->getType() == WIZARDTHING)
		{
			// chose an sloc
			if (slocs.size() > 1)
			{
				int index = Math::RangeRandom(0,slocs.size()-.1);
				sloc = slocs[index];
				slocs.erase(slocs.begin()+index);
			}
			else if (slocs.size() > 0) 
				sloc = slocs[0];

			sloc.y = HeightMap::getSingleton().getHeightAt(sloc.x, sloc.z);

			Multiplayer::getSingleton().killWizard(thing, sloc);
		}
	}

	//  enable the build spell
	for (int i=0; i<Multiplayer::getSingleton().numClients(); i++)
	{
		PlayerInfo player = Multiplayer::getSingleton().getClient(i);
        Multiplayer::getSingleton().serverSendInt(-1,ID_SETSPELLLEVEL,player.id);
	}
}

//----------------------------------------------------------------------------

void Game::startSkirmishGame()
{
	// reset the player
	Thing* cam = Renderer::getSingleton().getCameraThing();
	cam->die();
	cam->setPosition(mStartPos);
	Hud::getSingleton().reinit();

	// set up an enemy team 
	ColourValue colour;
	colour.r = atoi(mConfig.getSetting( "MONSTERS_RED" ).c_str()) / 255.0;
	colour.g = atoi(mConfig.getSetting( "MONSTERS_GREEN" ).c_str()) / 255.0;
	colour.b = atoi(mConfig.getSetting( "MONSTERS_BLUE" ).c_str()) / 255.0;
	int teamNum = Physics::getSingleton().newTeam(colour);

	int mode = Menu::getSingleton().getChosenSkirmishMode();
	if (mode == 0)
	{
		// read in the image file
		loadMapThingsFromImage("SkirmishThingsTexture", mConfig, HeightMap::getSingleton().getWorldSize(), teamNum);
		
		// set the camera position
		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);
			if (thing->getType() == PLAYERMARKERTHING)
			{
				cam->setPosition(thing->getPosition());
				thing->destroy();
				break;
			}
		}
	}
	else if (mode == 1)
	{
		// set up the things randomly
		loadMapThingsRandomly(mConfig, HeightMap::getSingleton().getWorldSize(), teamNum);
	}
	else if (mode == 2)
	{
		// read in the image file
		loadMapThingsFromImage("MultiplayerThingsTexture", mConfig, HeightMap::getSingleton().getWorldSize(), teamNum);

		// build a list of start locations
		std::vector<Vector3> slocs;

		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);
			if (thing->getType() == PLAYERMARKERTHING)
			{
				slocs.push_back(thing->getPosition());
				thing->destroy();
			}
		}
			
		// chose start for the player
		if (slocs.size() > 0)
		{
			int index = Math::RangeRandom(0,slocs.size()-.1);
			Vector3 playerStart = slocs[index];
			slocs.erase(slocs.begin()+index);

			playerStart.y = HeightMap::getSingleton().getHeightAt(playerStart.x, playerStart.z);
			cam->setPosition(playerStart);
		}

		// fill the rest of the slots with bots
		while (slocs.size() > 0)
		{
			// chose a start for the bot
			Vector3 botStart;

			int index =	Math::RangeRandom(0,slocs.size()-.1);
			botStart = slocs[index];
			slocs.erase(slocs.begin()+index);

			botStart.y = HeightMap::getSingleton().getHeightAt(botStart.x, botStart.z);
			
			// choose a random colour
			ColourValue botColour;
			botColour.r = Math::RangeRandom(0,1);
			botColour.g = Math::RangeRandom(0,1);
			botColour.b = Math::RangeRandom(0,1);

			// add a bot
			String brain = mConfig.getSetting( "BOT_BRAIN" ).c_str();

			AIWizardThing* bot = new AIWizardThing(botStart, botColour, brain);
			Physics::getSingleton().addThing(bot);

			Team* team = bot->getTeam();
			team->setColour(botColour);
			team->setWizardUID(bot->getUID());

			// give him a castle
			int skin = atoi(mConfig.getSetting( "BOT_CASTLE_SKIN" ).c_str());
			Vector3 caspos = BuildingHeightMap::getSingleton().alignPosition(botStart);
			Castle* castle = new Castle(team->getTeamNum(), caspos, skin);

			team->setCastleUID(castle->	getUID());
		}
	}


	// reset the score
	Renderer::getSingleton().getCameraThing()->getTeam()->setScore(0);
	Renderer::getSingleton().getCameraThing()->setManaStone(0);
	Renderer::getSingleton().getCameraThing()->setMana(0);

	// enable the claim and build spells
	Hud::getSingleton().reinit();
	SpellManager::getSingleton().setLevel(-1);

	mSkirmishMode = true;
}

//----------------------------------------------------------------------------

void Game::loadMapThingsSparsely(ConfigFile config, Real worldSize)
{
	// make some foliage
	int foliageNum = atoi(config.getSetting( "FoliageAmount" ).c_str());
	Real foliageLineMin = atof(config.getSetting( "FOLIAGE_LINE_MIN" ).c_str());
	Real foliageLineMax = atof(config.getSetting( "FOLIAGE_LINE_MAX" ).c_str());
	Real foliageScale = atof(config.getSetting( "FOLIAGE_SCALE" ).c_str());
	Real foliageScaleVar = atof(config.getSetting( "FOLIAGE_SCALE_VAR" ).c_str());

	// set up some foliage
	int i=0;
	while (i<foliageNum)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * worldSize;
        Real z = Math::SymmetricRandom() * worldSize;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		if (y > foliageLineMin && y < foliageLineMax)
		{
			i++;
			Vector3 pos = Vector3(x,0,z);
			Real scale = foliageScale + (Math::SymmetricRandom()-.5) * foliageScaleVar;

			Physics::getSingleton().addThing(new FoliageThing(scale,pos));
		}
	}

	// set up some wild mana and shrines
	int numMana = atoi(config.getSetting( "MANA_START_NUM" ).c_str());
	int manaAmount = atoi(config.getSetting( "MANA_START_AMOUNT" ).c_str());

	int numShrines = atoi(config.getSetting( "NUM_SHRINES" ).c_str());
	int shrineSkin = atoi(config.getSetting( "SHRINE_SKIN" ).c_str());

	i=0;
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
			
			if (i < numShrines)
			{
				pos = BuildingHeightMap::getSingleton().alignPosition(pos);

				ShrineThing* shrine = new ShrineThing(pos, shrineSkin);
				Physics::getSingleton().addThing(shrine);
			}
		}
	}
}

//----------------------------------------------------------------------------

void Game::loadMapThingsRandomly(ConfigFile config, Real worldSize, int enemyTeamNum)
{
	Thing* cam = Renderer::getSingleton().getCameraThing();

	// make some foliage
	LogManager::getSingleton().logMessage("Making Foliage...");
	int foliageNum = atoi(mConfig.getSetting( "FoliageAmount" ).c_str());
	Real foliageLineMin = atof(mConfig.getSetting( "FOLIAGE_LINE_MIN" ).c_str());
	Real foliageLineMax = atof(mConfig.getSetting( "FOLIAGE_LINE_MAX" ).c_str());
	Real foliageScale = atof(mConfig.getSetting( "FOLIAGE_SCALE" ).c_str());
	Real foliageScaleVar = atof(mConfig.getSetting( "FOLIAGE_SCALE_VAR" ).c_str());

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

	// add some shrines
	int numShrines = atoi(mConfig.getSetting( "NUM_SHRINES" ).c_str());
	int shrineSkin = atoi(mConfig.getSetting( "SHRINE_SKIN" ).c_str());
	i=0;
	while(i<numShrines)
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

			ShrineThing* shrine = new ShrineThing(pos, shrineSkin);
			Physics::getSingleton().addThing(shrine);
		}
	}

	Real minDist = atoi(mConfig.getSetting( "MONSTER_MIN_DIST" ).c_str());

	// set up some enemy towers
	Real numTowers = atoi(mConfig.getSetting( "NUM_TOWERS" ).c_str());
	int towerSkin = atoi(mConfig.getSetting( "TOWER_SKIN" ).c_str());
	i=0;
	while(i<numTowers)
	{
        // Random translate
        Real x = Math::SymmetricRandom() * size;
        Real z = Math::SymmetricRandom() * size;
		Real y = HeightMap::getSingleton().getHeightAt(x, z);

		Vector3 pos = Vector3(x,0,z);

		if (y > CONR("BUILDING_MIN_GROUNDY") && cam->cylinderDistance(pos) > minDist)
		{
			i++;
			pos = BuildingHeightMap::getSingleton().alignPosition(pos);

			TowerThing* tower = new TowerThing(enemyTeamNum, pos, towerSkin);
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

		Vector3 pos = Vector3(x,0,z);

		if (y > CONR("BUILDING_MIN_GROUNDY") && cam->cylinderDistance(pos) > minDist)
		{
			i++;

			TickThing* tick = new TickThing(enemyTeamNum,pos);
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

		Vector3 pos = Vector3(x,0,z);

		if (y > CONR("BUILDING_MIN_GROUNDY") && cam->cylinderDistance(pos) > minDist)
		{
			i++;

			SentinelThing* sentinel = new SentinelThing(enemyTeamNum,pos);
			Physics::getSingleton().addThing(sentinel);
		}
	}
}

//----------------------------------------------------------------------------

void Game::loadMapThingsFromImage(String textureKey, ConfigFile config, Real worldSize, int enemyTeamNum)
{
	LogManager::getSingleton().logMessage("Loading Map Things");

	String filename = config.getSetting(textureKey);
	
	if (filename == "") return;

	Image image;
	image.load(filename);

	if ( image.getFormat() != PF_A8R8G8B8 )
	{		
		std::ostringstream num("");
		num << image.getFormat();
		Exception( Exception::ERR_INVALIDPARAMS, String("Error: Image is not an RGBA image. ") + num.str(),
				"Game::loadMapThings" );
	}

	// uchar?  ushort?  M$ crap datatypes 
	// see OgrianGame.h for defines to fix this. --jdpowell 20050612
	unsigned char* data = image.getData();

	unsigned short width = image.getWidth();
	unsigned short height = image.getHeight();

	// loop through each pixel
	for (int row=0; row<height; row++)
	{
		for (int col=0; col<width; col++)
		{
			uchar red	= data[ ( ( row * width*4 ) + col*4 ) ];
			uchar green	= data[ ( ( row * width*4 ) + col*4 + 1) ];
			uchar blue	= data[ ( ( row * width*4 ) + col*4 + 2) ];
			uchar alpha	= data[ ( ( row * width*4 ) + col*4 + 3) ];

			int colour = red * 0x10000 + green * 0x100 + blue;

			Vector3 pos;

			if (alpha == 0xff)
			{
				pos.x = ((float)col / float(width)) * worldSize;
				pos.z = ((float)row / float(width)) * worldSize;
				pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z);
			
			switch (colour)
			{
				case HEX_RED: // tick
				{
					TickThing* tick = new TickThing(enemyTeamNum,pos);
					Physics::getSingleton().addThing(tick);
					break;
				}
				case HEX_PURPLE: // sentinel
				{
					SentinelThing* sentinel = new SentinelThing(enemyTeamNum,pos);
					Physics::getSingleton().addThing(sentinel);
					break;
				}
				case HEX_ORANGE: // tower
				{
					int towerSkin = atoi(mConfig.getSetting( "TOWER_SKIN" ).c_str());

					TowerThing* tower = new TowerThing(enemyTeamNum, pos, towerSkin);
					Physics::getSingleton().addThing(tower);
					break;
				}
				case HEX_YELLOW: // shrine
				{
					int shrineSkin = atoi(mConfig.getSetting( "SHRINE_SKIN" ).c_str());

					ShrineThing* shrine = new ShrineThing(pos, shrineSkin);
					Physics::getSingleton().addThing(shrine);
					break;
				}
				case HEX_CYAN: // mana
				{
					int manaAmount = atoi(config.getSetting( "MANA_START_AMOUNT" ).c_str());

					ManaThing* mana = new ManaThing(manaAmount,pos);
					Physics::getSingleton().addThing(mana);
					break;
				}
				case HEX_GREEN: // foliage
				{
					Real foliageScale = atoi(mConfig.getSetting( "FOLIAGE_SCALE" ).c_str());
					Real foliageScaleVar = atoi(mConfig.getSetting( "FOLIAGE_SCALE_VAR" ).c_str());
					Real scale = foliageScale + (Math::SymmetricRandom()-.5) * foliageScaleVar;

					Physics::getSingleton().addThing(new FoliageThing(scale,pos));
					break;
				}
				case HEX_WHITE: // player start
				{
					Physics::getSingleton().addThing(new PlayerMarkerThing(pos));
					break;
				}
			}}
		}
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
