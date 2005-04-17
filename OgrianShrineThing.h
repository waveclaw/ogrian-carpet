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
OgrianShrineThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a shrine which can be claimed to increase mana regen for a wizard

/*------------------------------------*/

#ifndef __OgrianShrineThing_H__
#define __OgrianShrineThing_H__

#include <Ogre.h>
#include "OgrianMultiplayer.h"
#include "OgrianBuildingHeightMap.h"
#include "OgrianConst.h"
#include "OgrianModel.h"
#include "OgrianPhysics.h"
#include "OgrianHeightMap.h"
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class ShrineBallThing : public Thing
{
public:
	ShrineBallThing(Thing* shrine)
		: Thing("Ogrian/ShrineBall", SPRITE, "ShrineBall", false, CONR("SHRINE_BALL_SCALE"), Vector3(0,0,0), SPHERE)
	{
		mShrine = shrine;
		setColour(ColourValue::White);
	}

	virtual void setColour(ColourValue& colour)
	{
		if (colour == ColourValue::White)
		{
			setMaterial("Ogrian/Clear");
		}
		else
		{
			setMaterial("Ogrian/ShrineBall");
			playSound(Game::getSingleton().SOUND_HUM);
		}

		Thing::setColour(colour);
		setUpdateFlag();
	}

	virtual void claim(int teamNum)
	{
		if (mShrine)
			mShrine->claim(teamNum);
	}
	
	virtual ThingType getType()	{ return SHRINEBALLTHING; }

private:
	Thing* mShrine;
};

/////////////////////////////////////////////////////////////////////////////
class ShrineThing : public Thing
{
public:
	ShrineThing(Vector3 pos, String mesh, Real scale, Real ratio) 
		: Thing("Ogrian/Tower", MODEL, "Shrine", false, CONR("SHRINE_WIDTH"), pos, CUBE)
	{
		// set the mesh
		static_cast<Model*>(getVisRep())->setMesh(mesh,scale,ratio);

		setHeight(CONR("SHRINE_HEIGHT"));
		Thing::setTeamNum(-1);

		mBall = 0;

		if (!Multiplayer::getSingleton().isClient())
		{
			setColour(ColourValue::White);
			mBall = new ShrineBallThing(this);
			Physics::getSingleton().addThing(mBall);
		}
		
		setPosition(pos);
		BuildingHeightMap::getSingleton().moldLandscape(this);
	}

	virtual void destroy()
	{
		if (mBall)
			mBall->destroy();
	}

	virtual void claim(int teamNum)
	{
		playSound(Game::getSingleton().SOUND_HUM, true);

		setTeamNum(teamNum);
	}

	virtual void setTeamNum(int teamNum)
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

	virtual void setColour(ColourValue& colour)
	{
		if (mBall)
			mBall->setColour(colour);
	
		Thing::setColour(colour);
	}

	virtual void setPosition(Vector3 pos)
	{
		pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z);

		Thing::setPosition(pos);

		pos.y += getHeight()/2 + CONR("SHRINE_BALL_SCALE");

		if (mBall)
            mBall->setPosition(pos);
	}

	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return SHRINETHING; }
	
private:
	ShrineBallThing* mBall;
};

}
#endif