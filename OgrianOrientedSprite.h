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
OgrianOrientedSprite.h
Original Author: Mike Prosser
Additional Authors: 

Description: The OrientedSprite is a visual representation that can represent an object from each of 
the eight cardinal directions and also from any of an arbitrary number of poses. 

/*------------------------------------*/

#ifndef __OgrianOrientedSprite_H__
#define __OgrianOrientedSprite_H__

#include <Ogre.h>
#include "Math.h"

#include "OgrianSprite.h"

using namespace Ogre;

namespace Ogrian
{

class Pose
{
public:
	Pose(String basename)
	{
		f  = new Sprite("auto", true); f ->setMaterial(String(basename) << "f");
		fl = new Sprite("auto", true); fl->setMaterial(String(basename) << "fl");
		l  = new Sprite("auto", true); l ->setMaterial(String(basename) << "l");
		bl = new Sprite("auto", true); bl->setMaterial(String(basename) << "bl");
		b  = new Sprite("auto", true); b ->setMaterial(String(basename) << "b");
		br = new Sprite("auto", true); br->setMaterial(String(basename) << "br");
		r  = new Sprite("auto", true); r ->setMaterial(String(basename) << "r");
		fr = new Sprite("auto", true); fr->setMaterial(String(basename) << "fr");
	}
	virtual ~Pose()
	{
		delete f;
		delete fl;
		delete l;
		delete bl;
		delete b;
		delete br;
		delete r;
		delete fr;
	}

	Sprite* getSprite(Vector3 campos, Vector3 pos, float offset)
	{
		// determine the direction
		float dir = -atan2(campos.x - pos.x, campos.z - pos.z) + offset;

		// constrain the offset
		while (dir > Math::PI) dir -= 2*Math::PI;
		while (dir < -Math::PI) dir += 2*Math::PI;

		// return the propper sprite
		if (dir <= (-7/8.0)*Math::PI) return b;
		if (dir <= (-5/8.0)*Math::PI && dir >= (-7/8.0)*Math::PI) return bl;
		if (dir <= (-3/8.0)*Math::PI && dir >= (-5/8.0)*Math::PI) return l;
		if (dir <= (-1/8.0)*Math::PI && dir >= (-3/8.0)*Math::PI) return fl;
		if (dir <= (1/8.0)*Math::PI && dir >= (-1/8.0)*Math::PI) return f;
		if (dir <= (3/8.0)*Math::PI && dir >= (1/8.0)*Math::PI) return fr;
		if (dir <= (5/8.0)*Math::PI && dir >= (3/8.0)*Math::PI) return r;
		if (dir <= (7/8.0)*Math::PI && dir >= (5/8.0)*Math::PI) return br;
		if (dir >= (7/8.0)*Math::PI) return b;

		// error
		LogManager::getSingleton().logMessage(String("ERROR! dir: ") << dir*8/Math::PI << "/8*Math::PI");

		return 0;
	}

private:
	Sprite* f;
	Sprite* fl;
	Sprite* l;
	Sprite* bl;
	Sprite* b;
	Sprite* br;
	Sprite* r;
	Sprite* fr;
};

class OrientedSprite : public VisRep
{
public:
	OrientedSprite();

	virtual ~OrientedSprite();

	// set the position
	virtual void setPosition(Vector3 pos);

	// set the width
	virtual void setWidth(Real width);

	// set the height
	virtual void setHeight(Real height);

	// add a pose
	virtual int addPose(String basename);
	
	// set which pose to use 
	virtual void setPose(int index);

	// set the orientation from -Math::PI to Math::PI 
	// note, call this through Thing::setOrientation for best behavior
	virtual void setOrientation(Real orientation);
	
	// set the color
	virtual void setColour(ColourValue& colour);

	// get the colour
	virtual ColourValue getColour();

	// unused
	virtual void setMaterial(String material) {}
	virtual void setRotation(Real rotation) {};

	// add this visrep to the renderer
	virtual void addToRenderer();

	// remove this visrep from the renderer
	virtual void removeFromRenderer();

	// returns true if its in the renderer
	virtual inline bool inRenderer();

	// call this every frame to set the right sprite
	virtual void frame();

private:
	Pose* mCurrentPose;
	Sprite* mCurrentSprite;
	std::vector<Pose*> mPoses;

	Real mWidth;
	Real mHeight; 
	Vector3 mPos;
	ColourValue mColour;
	bool mInRenderer;
	Real mOrientation;
};
}
#endif