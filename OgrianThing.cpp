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

Thing::Thing(String material, String prefix, bool fixed_y, Real scale, Vector3 pos)
{
	// initialize the mvars
	mAlive = true;
	mInRenderer = false;
	mBbset = 0;
	mBillboard = 0;
	mNode = 0;

	// name it
	mName = prefix << "_" << msNextGeneratedNameExt++;

	// set the settings
	mFixed_y = fixed_y;
	setMaterial(material);
	setVelocity(Vector3(0,0,0));
	setPosition(pos);
	setScale(scale);

	// add it to the renderer
	_addToRenderer();
}

Thing::~Thing()
{
	_removeFromRenderer();
}

// start rendering this thing
void Thing::_addToRenderer()
{
	// dont do this twice!
	if (mInRenderer) return;

	// create the billboardset
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	mBbset = sceneMgr->createBillboardSet(mName,1);
	mBillboard = mBbset->createBillboard(0, 0, 0);

	if (mFixed_y)
	{
		// it doesn't really matter if its common or self, since there's only one per set
		mBbset->setBillboardType(BBT_ORIENTED_SELF);
		mBillboard->mDirection = Vector3::UNIT_Y;
	}

	// attach the set
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mBbset);

	mInRenderer = true;

	// apply its properties to it
	setMaterial(mMaterial);
	Thing::setPosition(mPos);
	setScale(mRadius*2);

}

// stop rendering this thing
void Thing::_removeFromRenderer()
{
	// dont do this twice!
	if (!mInRenderer) return;

	// remove it from the scene
	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	// null the mvars
	mBbset = 0;
	mBillboard = 0;
	mNode = 0;

	mInRenderer = false;
}

bool Thing::isAlive()
{
	return mAlive;
}

void Thing::setVelocity(Vector3 vel)
{
	mVel = vel;
}
void Thing::setPosition(Vector3 pos)
{
	mPos = pos;

	if (mInRenderer)
		mNode->setPosition(pos);
}

void Thing::setScale(Real scale)
{
	mRadius = scale/2;

	if (mInRenderer)
		mBillboard->setDimensions(scale,scale);
}

Vector3 Thing::getPosition()
{
	return mPos;
}

Vector3 Thing::getVelocity()
{
	return mVel;
}

Real Thing::getScale()
{
	return mRadius*2;
}

void Thing::setMaterial(String material)
{
	mMaterial = material;

	if (mInRenderer && mMaterial != "")
		mBbset->setMaterialName(material);
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
	if (distance(cam) < CAMERA_THING_CULL_DIST)
	{
		// add it if its close enough
		if (!mInRenderer)
			_addToRenderer();
	}
	else
	{
		// remove it otherwise
        if (mInRenderer)
			_removeFromRenderer();
	}
}

// calculate the distance, neglecting the difference in altitude
Real Thing::distance(Thing* e)
{
	return sqrt((mPos.x - e->mPos.x)*(mPos.x - e->mPos.x) 
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

Real Thing::getRadius()
{
	return mRadius;
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


// they are ordered by x location
bool Thing::operator<(Thing* other)
{
	return (mPos.x < other->mPos.x);
}

}