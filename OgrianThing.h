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
OgrianThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the Thing class is the basic moveable object. 
It interacts with the renderer and the physics engine. 
It is rendered as a Billboard.

/*------------------------------------*/

#ifndef __OgrianThing_H__
#define __OgrianThing_H__

#include <Ogre.h>
#include "OgrianHeightMap.h"

using namespace Ogre;

namespace Ogrian
{

enum ThingType
{
	THING,
	ROLLINGTHING,
	FLOATINGTHING,
	MANATHING,
	FOLIAGETHING,
	CAMERATHING,
};


// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class Thing
{
public:
	Vector3 mPos;

	Thing(String material, String prefix="thing", bool fixed_y=false, Real scale=1, Real x=0, Real y=0, Real z=0);
	virtual ~Thing();

	virtual void setVelocity(const Vector3 vel);
	virtual void setPosition(const Vector3 pos);

	virtual void setPosition(Real x, Real y, Real z);

	virtual void setVelocity(Real x, Real y, Real z);

	virtual void setScale(Real scale);

	virtual void setMaterial(String material);

	virtual void move(Real time);

	virtual Real distance(Thing* e);

	virtual Real getRadius();

	virtual Vector3 getPosition();
	virtual Vector3 getVelocity();
	virtual Real getScale();

	virtual ThingType getType(); 

	virtual void collided(Thing* e);

	virtual void destroy();

	virtual bool isAlive();

	// they are ordered by x location
	bool operator<(Thing* other);

	
protected:
	Vector3 mVel;

	BillboardSet* mBbset;
	Billboard* mBillboard;
	bool mFixed_y;
	String mMaterial;

	SceneNode* mNode;
	String mName;

	Real mRadius;
	Real mHeight;

	bool mAlive;

	bool mInRenderer;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	virtual void _addToRenderer();
	virtual void _removeFromRenderer();
	virtual void _updateVisibility();
};

}
#endif