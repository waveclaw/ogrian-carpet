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
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

class Team
{
public:
	Team(int teamNum, ColourValue colour);

	// the team's number
	int getTeamNum();

	// the team's wizard
	int getWizardUID();
	void setWizardUID(int uid);
	bool hasWizard();

	// the team's score
	void setScore(int score);
	int getScore();
	void incrementScore();

	// the team's castle
	void setCastle(Castle* castle);
	Castle* getCastle();
	bool hasCastle();

	// the team's colour
	ColourValue getColour();
	void setColour(ColourValue& colour);

	// the team's enemies
	void addEnemy(DamageableThing* enemy);
	void removeEnemy(DamageableThing* enemy);

	DamageableThing* getNearestEnemy(Thing* thing, Real range);

private:
	int mTeamNum;
	int mScore;
	int mWizardUID;
	ColourValue mColour;

	Castle* mCastle;

	std::vector<DamageableThing*> mEnemies;
};

}

#endif