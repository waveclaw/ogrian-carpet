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

Description: This is a special Thing that is tied to the camera. 

Description: A Sprite is a billboard that is properly depth
sorted and has a unique material. 

/*------------------------------------*/

#ifndef __OgrianSprite_H__
#define __OgrianSprite_H__

#include <Ogre.h>

using namespace Ogre;

namespace Ogrian
{

class Sprite
{
public:
	Sprite(String name, bool fixed_y=false);
	virtual ~Sprite();

	// set the position
	virtual void setPosition(Vector3 pos);

	virtual void setWidth(Real width);

	virtual void setHeight(Real height);

	virtual void setMaterial(String material);
	
	virtual void addToRenderer();

	virtual void removeFromRenderer();

	virtual inline bool inRenderer();

private:
	// graphical rendering stuff
	BillboardSet* mBbset;
	Billboard* mBillboard;
	bool mFixed_y;
	String mMaterial;
	SceneNode* mNode;
	String mName;

	bool mInRenderer;
	Real mWidth;
	Real mHeight; 
	Vector3 mPos;

};
}
#endif