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

using namespace Ogre;

namespace Ogrian
{

// extend floatingThing for floating behavior
class Team
{
public:
	Team(int wizardUID)
	{
		mWizardUID = wizardUID;
		mScore = 0;
	}

	int getWizardUID()
	{
		return mWizardUID;
	}

	void setScore(int score)
	{
		mScore = score;
	}

	int getScore()
	{
		return mScore;
	}

	void incrementScore()
	{
		mScore++;
	}

private:
	int mScore;
	int mWizardUID;
};

}

#endif