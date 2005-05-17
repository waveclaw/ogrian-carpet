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
OgrianShrineThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a shrine which can be claimed to increase mana regen for a wizard

/*------------------------------------*/

#include "OgrianMultiplayer.h"
#include "OgrianSkinManager.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianModel.h"
#include "OgrianPhysics.h"
#include "OgrianHeightMap.h"
#include "OgrianShrineThing.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

ShrineBallThing::ShrineBallThing(Thing* shrine)
	: Thing("Ogrian/ShrineGlow", SPRITE, "ShrineBall", true, CONR("SHRINE_BALL_SCALE"), Vector3(0,0,0), SPHERE)
{
	mShrine = shrine;
	setColour(ColourValue::White);
}

//----------------------------------------------------------------------------

void ShrineBallThing::setColour(ColourValue& colour)
{
	if (colour == ColourValue::White)
	{
		setMaterial("Ogrian/Clear");
	}
	else
	{
		setMaterial("Ogrian/ShrineGlow");
		playSound(Game::getSingleton().SOUND_HUM);
	}

	Thing::setColour(colour);
	setUpdateFlag();
}

//----------------------------------------------------------------------------

void ShrineBallThing::claim(int teamNum)
{
	if (mShrine)
		mShrine->claim(teamNum);
}

/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------

ShrineThing::ShrineThing(Vector3 pos, int skin) 
	: Thing("Ogrian/Tower", MODEL, "Shrine", false, CONR("SHRINE_WIDTH"), pos, CUBE)
{
	// set the mesh
	setSkin(skin);

	setHeight(CONR("SHRINE_HEIGHT"));
	Thing::setTeamNum(-1);

	mBall = 0;

	if (!Multiplayer::getSingleton().isClient())
	{
		setColour(ColourValue::White);
		mBall = new ShrineBallThing(this);
		Physics::getSingleton().addThing(mBall);
	}
}

//----------------------------------------------------------------------------

void ShrineThing::setSkin(int skin)
{
	String mesh = SkinManager::getSingleton().getShrineSkin(skin);
	static_cast<Model*>(getVisRep())->setMesh(mesh);
}

//----------------------------------------------------------------------------

void ShrineThing::destroy()
{
	if (mBall)
		mBall->destroy();
}

//----------------------------------------------------------------------------

void ShrineThing::claim(int teamNum)
{
	playSound(Game::getSingleton().SOUND_HUM, true);

	setTeamNum(teamNum);
}

//----------------------------------------------------------------------------

void ShrineThing::setTeamNum(int teamNum)
{
	// remove from the old team
	if (getTeamNum() >= 0)
	{
		Team* team = Physics::getSingleton().getTeam(getTeamNum());
		if (team)
		{
			WizardThing* wizard = (WizardThing*)Physics::getSingleton().getThing(team->getWizardUID());
			if (wizard)
				wizard->removeShrine();
		}
	}

	// add to the new team
	if (teamNum >= 0)
	{
		Team* team = Physics::getSingleton().getTeam(teamNum);
		if (team)
		{
			WizardThing* wizard = (WizardThing*)Physics::getSingleton().getThing(team->getWizardUID());
			if (wizard)
				wizard->addShrine();
		}

		setColour(team->getColour());
	}
	else setColour(ColourValue::White);

	Thing::setTeamNum(teamNum);

	if (mBall) mBall->setTeamNum(teamNum);
}

//----------------------------------------------------------------------------

void ShrineThing::setColour(ColourValue& colour)
{
	if (mBall)
		mBall->setColour(colour);

	Thing::setColour(colour);
}

//----------------------------------------------------------------------------

void ShrineThing::setPosition(Vector3 pos)
{
	pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z);

	Thing::setPosition(pos);

	pos.y += getHeight()/2 + CONR("SHRINE_BALL_SCALE");

	if (mBall)
        mBall->setPosition(pos);
}

//----------------------------------------------------------------------------

}