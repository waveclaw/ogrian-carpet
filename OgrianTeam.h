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
OgrianTeam.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a team, it has a wizard and a score

/*------------------------------------*/


#ifndef __OgrianTeam_H__
#define __OgrianTeam_H__

#include <Ogre.h>
#include "OgrianGame.h"
#include "OgrianCastle.h"

using namespace Ogre;

namespace Ogrian
{

class Team
{
public:
	Team(int wizardUID, ColourValue colour)
	{
		mColour = colour;
		mWizardUID = wizardUID;
		mCastle = 0;
		mScore = 0;
	}

	virtual ~Team()
	{

	}

	int getWizardUID()
	{
		return mWizardUID;
	}

	void setWizardUID(int uid)
	{
		mWizardUID = uid;
	}

	void setScore(int score)
	{
		if (mScore == score) return;

		mScore = score;
			
		Game::getSingleton().updateScores();
	}

	int getScore()
	{
		return mScore;
	}

	void incrementScore()
	{
		mScore++;

		Game::getSingleton().updateScores();
	}

	void setCastle(Castle* castle)
	{
		mCastle = castle;
	}

	Castle* getCastle()
	{
		return mCastle;
	}

	bool hasCastle()
	{
		return (mCastle != 0);
	}

	ColourValue getColour()
	{
		return mColour;
	}

	void setColour(ColourValue& colour)
	{
		mColour = colour;
	}

private:
	int mScore;
	int mWizardUID;
	ColourValue mColour;

	Castle* mCastle;
};

}

#endif