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
OgrianThing.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: the Thing class is the basic moveable object. 
It interacts with the renderer and the physics engine. 
It is rendered as a Billboard.

/*------------------------------------*/

#include "OgrianThing.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"

namespace Ogrian
{

unsigned long Thing::msNextGeneratedNameExt = 1;

Thing::Thing(String material, String prefix, bool fixed_y, Real scale, Vector3 pos, ThingShape shape)
{
	// initialize the mvars
	mAlive = true;
	mInPhysics = false;

	// name it
	mName = prefix << "_" << msNextGeneratedNameExt++;

	// make the sprite
	mSprite = new Sprite(mName, fixed_y);

	// set the settings
	setMaterial(material);
	setVelocity(Vector3(0,0,0));
	setPosition(pos);
	setScale(scale);
	setShape(shape);

	// add it to the renderer
	_addToRenderer();
}

Thing::~Thing()
{
	_removeFromRenderer();

	if (mSprite)
		delete mSprite;
}

void Thing::placedInPhysics()
{
	mInPhysics = true;
}

// start rendering this thing
void Thing::_addToRenderer()
{
	mSprite->addToRenderer();
}

// stop rendering this thing
void Thing::_removeFromRenderer()
{
	mSprite->removeFromRenderer();
}

bool Thing::isAlive()
{
	return mAlive;
}

void Thing::setShape(ThingShape shape)
{
	mShape = shape;
}

ThingShape Thing::getShape()
{
	return mShape;
}

void Thing::setVelocity(Vector3 vel)
{
	mVel = vel;
}
void Thing::setPosition(Vector3 pos)
{
	// update the sprite
	mSprite->setPosition(pos);

	// update physics
	if (mInPhysics && pos != mPos)
		Physics::getSingleton().updateThing(this, mPos, pos);

	// update mPos
	mPos = pos;
}

void Thing::setScale(Real scale)
{
	setHeight(scale);
	setWidth(scale);
}

void Thing::setHeight(Real height)
{
	mHeight = height;
	
	mSprite->setHeight(height);
}

void Thing::setWidth(Real width)
{
	mWidth = width;

	mSprite->setWidth(width);
}

Vector3 Thing::getPosition()
{
	return mPos;
}

Vector3 Thing::getVelocity()
{
	return mVel;
}

Real Thing::getWidth()
{
	return mWidth;
}

Real Thing::getHeight()
{
	return mHeight;
}

void Thing::setMaterial(String material)
{
	mSprite->setMaterial(material);
}

// increment the position by the velocity times time
void Thing::move(Real time)
{
	setPosition(mPos + mVel * time);

	_updateVisibility();
}

// only render things that are close to the camera
void Thing::_updateVisibility()
{
	// check the distance from the camera
	Thing* cam = Renderer::getSingleton().getCameraThing();
	if (cylinderDistance(cam) < THING_CULL_DIST)
	{
		// add it if its close enough
		if (!mSprite->inRenderer())
			_addToRenderer();
	}
	else
	{
		// remove it otherwise
        if (mSprite->inRenderer())
			_removeFromRenderer();
	}
}

// calculate the distance, neglecting the difference in altitude: sqrt(x^2+y^2)
Real Thing::cylinderDistance(Thing* e)
{
	return sqrt((mPos.x - e->mPos.x)*(mPos.x - e->mPos.x) 
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

// calculate the distance: sqrt(x^2+y^2+z^2)
Real Thing::sphereDistance(Thing* e)
{
	return sqrt(  (mPos.x - e->mPos.x)*(mPos.x - e->mPos.x)
				+ (mPos.y - e->mPos.y)*(mPos.z - e->mPos.y)
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

// calculate axis distance
Real Thing::axisDistance(Thing* e)
{
	// find the distance along each 
	Real xdist = Math::Abs(mPos.x - e->mPos.x);
	Real zdist = Math::Abs(mPos.z - e->mPos.z);

	// return the greater of the two
	return (xdist > zdist) ? xdist : zdist;
}

ThingType Thing::getType()
{
	return THING;
}

void Thing::collided(Thing* e)
{
	// override this for interesting behaviors
	
}

void Thing::destroy()
{
	// mark it to be deleted
	mAlive = false;
}

void Thing::setPosY(Real y)
{
	mPos.y = y;
	setPosition(mPos);
}

Real Thing::getPosY()
{
	return mPos.y;
}

void Thing::setVelY(Real y)
{
	mVel.y = y;
}

Real Thing::getVelY()
{
	return mVel.y;
}

Real Thing::getGroundY()
{
	return getGroundY(mPos);
}

Real Thing::getGroundY(Vector3 pos)
{
	return HeightMap::getSingleton().getHeightAt(pos.x,pos.z);
}

void Thing::playSound(String filename)
{
	Audio::getSingleton().playSound(filename, getPosition());
}

}