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

#include "OgrianSprite.h"

using namespace Ogre;

namespace Ogrian
{

class Pose
{
public:
	Pose(String basename)
	{
		f  = new Sprite("auto", true);
		fl = new Sprite("auto", true);
		l  = new Sprite("auto", true);
		bl = new Sprite("auto", true);
		b  = new Sprite("auto", true);
		br = new Sprite("auto", true);
		r  = new Sprite("auto", true);
		fr = new Sprite("auto", true);

		// TODO: set the materials
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

	Sprite* getSprite(Vector3 campos)
	{

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

class OrientedSprite
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
	
	// unused
	virtual void setMaterial(String material) {}

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
	bool mInRenderer;
};
}
#endif