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
OgrianTeam.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a team, it has a wizard and a score

/*------------------------------------*/


#include "OgrianTeam.h"
#include "OgrianPhysics.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

Team::Team(int teamNum, ColourValue colour)
{
	mColour = colour;
	mWizardUID = -1;
	mCastle = 0;
	mScore = 0;
	mTeamNum = teamNum;

	// build the enemy list
	if (!Multiplayer::getSingleton().isClient())
	{
		for (int i=0; i<Physics::getSingleton().numThings(); i++)
		{
			Thing* thing = Physics::getSingleton().getThingByIndex(i);

			if (thing->isDamageable())
				addEnemy((DamageableThing*)thing);
		}
	}
}

//----------------------------------------------------------------------------

int Team::getWizardUID()
{
	return mWizardUID;
}

//----------------------------------------------------------------------------

void Team::setWizardUID(int uid)
{
	mWizardUID = uid;
}

//----------------------------------------------------------------------------

bool Team::hasWizard()
{
	return (mWizardUID != -1);
}

//----------------------------------------------------------------------------

void Team::setScore(int score)
{
	if (mScore == score) return;

	mScore = score;
		
	Game::getSingleton().updateScores();
}

//----------------------------------------------------------------------------

int Team::getScore()
{
	return mScore;
}

//----------------------------------------------------------------------------

void Team::incrementScore()
{
	mScore++;

	Game::getSingleton().updateScores();
}

//----------------------------------------------------------------------------

void Team::setCastle(Castle* castle)
{
	mCastle = castle;
}

//----------------------------------------------------------------------------

Castle* Team::getCastle()
{
	return mCastle;
}

//----------------------------------------------------------------------------

bool Team::hasCastle()
{
	return (mCastle != 0);
}

//----------------------------------------------------------------------------

ColourValue Team::getColour()
{
	return mColour;
}

//----------------------------------------------------------------------------

void Team::setColour(ColourValue& colour)
{
	mColour = colour;
}

//----------------------------------------------------------------------------

void Team::addEnemy(DamageableThing* enemy)
{
	// check the candidates
	if (enemy->getType() == FOLIAGETHING) return;
	if (enemy->getTeamNum() == mTeamNum) return;
	if (enemy->getTeamNum() < 0) return;
	if (!enemy->isAlive()) return;

	// add it
	mEnemies.push_back(enemy);
}

//----------------------------------------------------------------------------

void Team::removeEnemy(DamageableThing* enemy)
{
	for (int i=0; i<(int)mEnemies.size(); i++)
	{
		if (mEnemies[i] == enemy)
		{
			mEnemies[i] = 0;
			mEnemies.erase(mEnemies.begin() + i);
		}
	}
}

//----------------------------------------------------------------------------

DamageableThing* Team::getNearestEnemy(Thing* thing, Real range)
{
	if (mEnemies.size() == 0) return 0;

	DamageableThing* target = 0;
	Real bestDist = range;

	for (int i=0; i<(int)mEnemies.size(); i++)
	{
		DamageableThing* candidate = mEnemies[i];
		if (candidate && candidate->isAlive())
		{
			Real dist = candidate->axisDistance(thing);
			if (dist < bestDist)
			{
				bestDist = dist;
				target = mEnemies[i];
			}
		}
	}

	if (target && target->isAlive())
		return target;
	else
		return 0;
}

//----------------------------------------------------------------------------

}
