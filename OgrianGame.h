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


	void startGame(ConfigFile config);

	// while in pregame mode, claimthings cannot be made
	void setPreGame(bool active);
	bool isPreGame();
	void serverEndPreGame();

	void updateScores();

	void frame(Real time); // call this every frame

	// read in the game configuration
	void readConfig();

	// where all wizards should start
	Vector3 getStartPos();

	// wether or not the current map has lava
	void setLava(bool lava);
	bool getLava();

	static Game& getSingleton(void);

	// sound indexes
	int SOUND_WHOOSH;
	int SOUND_BANG;
	int SOUND_HISS;
	int SOUND_CHIRP;
	int SOUND_HUM;

private:
	bool mLava;
	Vector3 mStartPos;
	ConfigFile mConfig;

	Game();

	// reset the game
	void reset();

	void startSkirmishGame();
	void startServerGame();
	void startClientGame();


	void serverVictoryCheck();

	void loadSounds();

	bool mPreGame;
};

}

#endif