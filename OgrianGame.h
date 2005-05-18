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
#include "OgrianThing.h"

using namespace Ogre;

namespace Ogrian
{

class PlayerMarkerThing : public Thing
{
public:
	PlayerMarkerThing::PlayerMarkerThing(Vector3 pos)
		: Thing("Ogrian/Clear", SPRITE, "Marker", 0, 1, pos) {}
		
	virtual ThingType getType() { return PLAYERMARKERTHING; }
};

class Game : public Singleton< Game >
{
public:
	virtual ~Game();

	void startGame(ConfigFile config);

	// while in pregame mode, claimthings cannot be made
	void setPreGame(bool active);
	bool isPreGame();
	void serverEndPreGame();

	void updateScores();

	// call this when this player wins
	void victory();

	// call this every frame
	void frame(Real time); 

	// get the map config file
	ConfigFile getConfig();

	// where all wizards should start
	Vector3 getStartPos();

	// in skirmish mode, less mana is dropped by native monsters
	Real getManaDropMultiplier(int neamNum);

	// wether or not the current map has lava
	void setLava(bool lava);
	bool getLava();

	// get the music for this map
	String getMapMusic();

	static Game& getSingleton(void);

	// sound indexes
	int SOUND_WHOOSH;
	int SOUND_BANG;
	int SOUND_HISS;
	int SOUND_CHIRP;
	int SOUND_HUM;
	int SOUND_CRUNCH;
	int SOUND_TELEPORT;
	int SOUND_HEAL;

private:
	bool mLava;
	bool mHasWon;

	Vector3 mStartPos;
	String mMapMusic;
	ConfigFile mConfig;

	Game();

	// reset the game
	void reset();

	void startSkirmishGame();
	void startServerGame();
	void startClientGame();

	// load a set of things from an image
	void loadMapThingsFromImage(String textureKey, ConfigFile config, Real worldSize, int enemyTeamNum);

	// load a set of things randomly
	void loadMapThingsRandomly(ConfigFile config, Real worldSize, int enemyTeamNum);

	// load random mana and foliage only
	void loadMapThingsSparsely(ConfigFile config, Real worldSize);

	void serverVictoryCheck();

	void skirmishVictoryCheck();

	void loadSounds();

	bool mPreGame;
	bool mSkirmishMode;
};

}

#endif