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
OgrianFoliageThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: FoliageThing has the Foliage material. It is for scenery. 

/*------------------------------------*/

#ifndef __OgrianFoliageThing_H__
#define __OgrianFoliageThing_H__

#include <Ogre.h>
#include "OgrianDamageableThing.h"

using namespace Ogre;

namespace Ogrian
{

/////////////////////////////////////////////////////////////////////////////

class FoliageCorpse : public Thing
{
public:
	FoliageCorpse(Vector3 pos, Real scale, String material) 
		: Thing(material, SPRITE, "Foliagecorpse", true, scale, pos, CYLINDER)
	{
		setHeight(scale*1.5);
		setColour(ColourValue(1,1,1));
	}

	virtual ThingType getType()	{ return EFFECT; }
};

/////////////////////////////////////////////////////////////////////////////

class FoliageThing : public DamageableThing
{
public:
	FoliageThing(Real scale=1, Vector3 pos=Vector3(0,0,0)) 
		: DamageableThing(Renderer::getSingleton().getFoliageMaterial(),
			SPRITE, "Foliage", true, scale, pos, CYLINDER)
	{
		// place it slightly underground
		setHeight(scale*CONR("FOLIAGE_SKEW"));
		setPosY(getGroundY() + scale*CONR("FOLIAGE_HEIGHT_OFF"));
		setHealth(CONI("FOLIAGE_HEALTH"));
		setColour(ColourValue(1,1,1));
	}

	virtual ThingType getType()
	{
		return FOLIAGETHING;
	}	

	virtual void die()
	{
		destroy();
	}

	virtual void destroy()
	{
		Physics::getSingleton().addEffect(new FoliageCorpse(getPosition(), getWidth(),
			Renderer::getSingleton().getFoliageMaterial() + "Dead"));

		DamageableThing::destroy();
	}

	virtual void clearCut()
	{
		DamageableThing::destroy();
	}

	virtual void collided(Thing* e)
	{
		if (e->getType() == CASTLEWALL || e->getType() == CASTLETOWER)
			clearCut();
	}
};

}

#endif