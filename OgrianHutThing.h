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
OgrianHut.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a hut which can be claimed to increase mana regen for a wizard

/*------------------------------------*/

#ifndef __OgrianHut_H__
#define __OgrianHut_H__

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

class HutBallThing : public Thing
{
public:
	HutBallThing(Thing* hut)
		: Thing("Ogrian/HutBall", SPRITE, "HutBall", false, CONR("HUT_BALL_WIDTH"), Vector3(0,0,0), SPHERE)
	{
		mHut = hut;
	}

	virtual void setColour(ColourValue& colour)
	{
		if (colour == ColourValue::White)
		{
			setMaterial("Ogrian/Clear");
		}
		else
		{
			setMaterial("Ogrian/HutBall");
			playSound(Game::getSingleton().SOUND_HUM);
		}

		Thing::setColour(colour);
	}

	virtual void claim(int teamNum)
	{
		if (mHut)
			mHut->claim(teamNum);
	}
	
	virtual ThingType getType()	{ return HUTBALLTHING; }

private:
	Thing* mHut;
};

/////////////////////////////////////////////////////////////////////////////
class HutThing : public Thing
{
public:
	HutThing(Vector3 pos=Vector3(0,0,0)) 
		: Thing("Ogrian/Tower", MODEL, "Hut", false, CONR("HUT_WIDTH"), pos, CUBE)
	{
		// set the mesh
		static_cast<Model*>(getVisRep())->setMesh("tower.mesh",
			CONR("HUT_MESH_SCALE"), CONR("HUT_MESH_RATIO"));

		setHeight(CONR("HUT_HEIGHT"));
		Thing::setTeamNum(-1);
		mBall = new HutBallThing(this);
		Physics::getSingleton().addThing(mBall);

		if (!Multiplayer::getSingleton().isClient())
			setColour(ColourValue::White);
		
		setPosition(pos);
		BuildingHeightMap::getSingleton().moldLandscape(this);
	}

	virtual void destroy()
	{
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
					wizard->removeHut();
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
					wizard->addHut();
			}

			setColour(team->getColour());
		}
		else setColour(ColourValue::White);

		Thing::setTeamNum(teamNum);
		setUpdateFlag();
	}

	virtual void setColour(ColourValue& colour)
	{
		mBall->setColour(colour);
		Thing::setColour(colour);
	}

	virtual void setPosition(Vector3 pos)
	{
		pos.y = HeightMap::getSingleton().getHeightAt(pos.x, pos.z);

		Thing::setPosition(pos);

		pos.y += getHeight();
		mBall->setPosition(pos);
	}

	virtual bool isBuilding() { return true; }
	
	virtual ThingType getType()	{ return HUTTHING; }
	
private:
	HutBallThing* mBall;
};

}
#endif