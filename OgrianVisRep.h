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
OgrianVisRep.h
Original Author: Mike Prosser
Additional Authors: 

Description: The VisRep class serves as a superclass for Sprites, OrientedSprites, and Meshes. 

/*------------------------------------*/

#ifndef __OgrianVisRep_H__
#define __OgrianVisRep_H__

#include <Ogre.h>

using namespace Ogre;

namespace Ogrian
{

class VisRep
{
public:
	// set the position
	virtual void setPosition(Vector3 pos) = 0;

	// set the width
	virtual void setWidth(Real width) = 0;

	// set the height
	virtual void setHeight(Real height) = 0;

	// add a pose (only used by Oriented Sprites)
	virtual int addPose(String basename) = 0;
	
	// add a pose (only used by Oriented Sprites)
	virtual void setPose(int index) = 0;

	// set the orientation from -pi to pi (only used by Oriented Sprites)
	virtual void setOrientation(Real orientation) = 0;

	// set the material
	virtual void setMaterial(String material) = 0;

	// add this visrep to the renderer
	virtual void addToRenderer() = 0;

	// remove this visrep from the renderer
	virtual void removeFromRenderer() = 0;

	// returns true if its in the renderer
	virtual inline bool inRenderer() = 0;

	// call this every frame
	virtual void frame() = 0;
};
}
#endif