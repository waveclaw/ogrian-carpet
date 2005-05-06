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
OgrianSprite.h
Original Author: Mike Prosser
Additional Authors: 

Description: A Sprite is a billboard that is properly depth
sorted and has a unique material. 

/*------------------------------------*/

#ifndef __OgrianSprite_H__
#define __OgrianSprite_H__

#include <Ogre.h>

#include "OgrianVisRep.h"

using namespace Ogre;

namespace Ogrian
{

class Sprite : public VisRep
{
public:
	Sprite(String name, bool fixed_y=false);
	virtual ~Sprite();

	// set the position
	virtual void setPosition(Vector3 pos);

	// set the width
	virtual void setWidth(Real width);
	
	// set the rotation
	virtual void setRotation(Radian rotation);

	// set the height
	virtual void setHeight(Real height);

	// set the material used by this sprite
	virtual void setMaterial(String material);

	// set the color
	virtual void setColour(ColourValue& colour);

	// get the colour
	virtual ColourValue getColour();

	// get the material used by this sprite
	virtual String getMaterial();
	
	// unused
	virtual int addPose(String basename) { return 0; }
	virtual void setPose(int index) {}
	virtual int getPose() { return 0; }
	virtual void clearPoses() {} 
	virtual void frame() {}
	virtual void setOrientation(Radian orientation) {}

	// add this sprite to the renderer
	virtual void addToRenderer();

	// remove this sprite from the renderer
	virtual void removeFromRenderer(bool makeDot=true);

	// returns true if this sprite is in the renderer
	virtual inline bool inRenderer();

private:
	// graphical rendering stuff
	BillboardSet* mBbset;
	Billboard* mBillboard;
	Billboard* mDot;
	Billboard* mFoliageDot;

	bool mFixed_y;
	String mMaterial;
	SceneNode* mNode;
	String mName;

	bool mIsFoliage;
	bool mInRenderer;
	Real mWidth;
	Real mHeight; 
	Vector3 mPos;
	ColourValue mColour;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	void makeDot();
};
}
#endif