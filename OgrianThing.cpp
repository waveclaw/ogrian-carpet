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
#include "OgrianTime.h"
#include "OgrianOrientedSprite.h"

namespace Ogrian
{

unsigned long Thing::msNextGeneratedNameExt = 1;

//----------------------------------------------------------------------------

Thing::Thing(String material, ThingVisRep visrep, String prefix, bool fixed_y, Real scale, Vector3 pos, ThingShape shape)
{
	// initialize the mvars
	mAlive = true;
	mInPhysics = false;
	mPlayingSound = false;
	mInEarshot = false;
	mCurrentSound = 0;
	mUID = -1;
	mLastUpdateTime = 0;

	// name it
	mName = prefix << "_" << msNextGeneratedNameExt++;

	if (visrep == SPRITE)
	{
		// make the sprite
		Sprite* sprite = new Sprite(mName, fixed_y);
		mVisRep = sprite;
	}
	else if (visrep == ORIENTEDSPRITE)
	{
		OrientedSprite* osprite = new OrientedSprite();
		mVisRep = osprite;
	}

	// set the settings
	setMaterial(material);
	setVelocity(Vector3(0,0,0));
	setPosition(pos);
	setScale(scale);
	setShape(shape);
	setOrientation(0);

	// add it to the renderer
	mVisRep->addToRenderer();
}

//----------------------------------------------------------------------------

Thing::~Thing()
{

	stopSound();

	if (mVisRep)
	{
		mVisRep->removeFromRenderer();

		delete mVisRep;
	}
}

//----------------------------------------------------------------------------

VisRep* Thing::getVisRep()
{
	return mVisRep;
}

//----------------------------------------------------------------------------

int Thing::getUID()
{
	return mUID;
}

//----------------------------------------------------------------------------

void Thing::_setUID(int uid)
{
	mUID = uid;
}

//----------------------------------------------------------------------------

void Thing::placedInPhysics(int uid)
{
	mUID = uid;
	mInPhysics = true;
}

//----------------------------------------------------------------------------

bool Thing::isAlive()
{
	return mAlive;
}

//----------------------------------------------------------------------------

void Thing::setShape(ThingShape shape)
{
	mShape = shape;
}

//----------------------------------------------------------------------------

ThingShape Thing::getShape()
{
	return mShape;
}

//----------------------------------------------------------------------------

void Thing::setVelocity(Vector3 vel)
{
	mVel = vel;

	if (vel == Vector3(0,0,0))
	{
		mStopTime = Time::getSingleton().getTime();
		mStopped = true;
	}
	else mStopped = false;
}

//----------------------------------------------------------------------------

void Thing::setPosition(Vector3 pos)
{
	// update the sprite
	mVisRep->setPosition(pos);

	// update physics
	if (mInPhysics && pos != mPos && mUID != EFFECT_UID)
		Physics::getSingleton().updateThing(this, mPos, pos);

	// update the sound
	if (mPlayingSound && mInEarshot && pos != mPos)
		Audio::getSingleton().setSoundPosition(mCurrentSound, pos);

	// update mPos
	mPos = pos;
}

//----------------------------------------------------------------------------

void Thing::setScale(Real scale)
{
	setHeight(scale);
	setWidth(scale);
}

//----------------------------------------------------------------------------

void Thing::setHeight(Real height)
{
	mHeight = height;
	
	mVisRep->setHeight(height);
}

//----------------------------------------------------------------------------

void Thing::setWidth(Real width)
{
	mWidth = width;

	mVisRep->setWidth(width);
}

//----------------------------------------------------------------------------

Vector3 Thing::getPosition()
{
	return mPos;
}

//----------------------------------------------------------------------------

Vector3 Thing::getVelocity()
{
	return mVel;
}

//----------------------------------------------------------------------------

Real Thing::getWidth()
{
	return mWidth;
}

//----------------------------------------------------------------------------

Real Thing::getHeight()
{
	return mHeight;
}

//----------------------------------------------------------------------------

void Thing::setMaterial(String material)
{
	mVisRep->setMaterial(material);
}

//----------------------------------------------------------------------------

// increment the position by the velocity times time
void Thing::move(Real time)
{
	if (mVisRep->inRenderer()) mVisRep->frame();

	setPosition(mPos + mVel * time);

	_updateVisibility();
	_updateAudibility();
}

//----------------------------------------------------------------------------

String Thing::getString()
{
	return String(mName) << "(" << getType() << ") #" << mUID;
}

//----------------------------------------------------------------------------

// only render things that are close to the camera
void Thing::_updateVisibility()
{
	// check the distance from the camera
	Thing* cam = Renderer::getSingleton().getCameraThing();
	if (cylinderDistance(cam) < THING_CULL_DIST)
	{
		// add it if its close enough
		if (!mVisRep->inRenderer())
			mVisRep->addToRenderer();
	}
	else
	{
		// remove it otherwise
        if (mVisRep->inRenderer())
			mVisRep->removeFromRenderer();
	}
}

//----------------------------------------------------------------------------

// calculate the distance, neglecting the difference in altitude: sqrt(x^2+y^2)
Real Thing::cylinderDistance(Thing* e)
{
	return sqrt((mPos.x - e->mPos.x)*(mPos.x - e->mPos.x) 
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

//----------------------------------------------------------------------------

// calculate the distance: sqrt(x^2+y^2+z^2)
Real Thing::sphereDistance(Thing* e)
{
	return sqrt(  (mPos.x - e->mPos.x)*(mPos.x - e->mPos.x)
				+ (mPos.y - e->mPos.y)*(mPos.y - e->mPos.y)
			    + (mPos.z - e->mPos.z)*(mPos.z - e->mPos.z));
}

//----------------------------------------------------------------------------

// calculate axis distance
Real Thing::axisDistance(Thing* e)
{
	// find the distance along each 
	Real xdist = Math::Abs(mPos.x - e->mPos.x);
	Real zdist = Math::Abs(mPos.z - e->mPos.z);

	// return the greater of the two
	return (xdist > zdist) ? xdist : zdist;
}

//----------------------------------------------------------------------------

ThingType Thing::getType()
{
	return THING;
}

//----------------------------------------------------------------------------

void Thing::collided(Thing* e)
{
	// override this for interesting behaviors
	
}

//----------------------------------------------------------------------------

void Thing::destroy()
{
	// mark it to be deleted
	mAlive = false;
}

//----------------------------------------------------------------------------

void Thing::setPosY(Real y)
{
	mPos.y = y;
	setPosition(mPos);
}

//----------------------------------------------------------------------------

Real Thing::getPosY()
{
	return mPos.y;
}

//----------------------------------------------------------------------------

void Thing::setVelY(Real y)
{
	mVel.y = y;
}

//----------------------------------------------------------------------------

Real Thing::getVelY()
{
	return mVel.y;
}

//----------------------------------------------------------------------------

Real Thing::getGroundY()
{
	return getGroundY(mPos);
}

//----------------------------------------------------------------------------

Real Thing::getGroundY(Vector3 pos)
{
	return HeightMap::getSingleton().getHeightAt(pos.x,pos.z);
}

//----------------------------------------------------------------------------

void Thing::playSound(String filename, bool loop)
{
	mSoundFilename = filename;
	mSoundLoop = loop;
	mPlayingSound = true;

	if (mInEarshot)
		mCurrentSound = Audio::getSingleton().playSound(mSoundFilename, getPosition(), mSoundLoop);
}

//----------------------------------------------------------------------------

void Thing::stopSound()
{
	if (!mPlayingSound) return;

	mPlayingSound = false;

	if (mInEarshot)
		Audio::getSingleton().stopSound(mCurrentSound);
}

//----------------------------------------------------------------------------

void Thing::_updateAudibility()
{
	bool wasInEarshot = mInEarshot;
	Thing* cam = Renderer::getSingleton().getCameraThing();

	// can it be heard?
	mInEarshot = cylinderDistance(cam) < THING_EARSHOT;

	if (mPlayingSound)
	{
		// silence it if it has gone out of range
		if (wasInEarshot && !mInEarshot)
			Audio::getSingleton().stopSound(mCurrentSound);

		// start the sound again when it comes in range
		if (!wasInEarshot && mInEarshot)
			mCurrentSound = Audio::getSingleton().playSound(mSoundFilename, getPosition(), mSoundLoop);
	}
}

//----------------------------------------------------------------------------

void Thing::setOrientation(Real direction)
{
	mOrientation = direction;
	mVisRep->setOrientation(direction);
}

//----------------------------------------------------------------------------

Real Thing::getOrientation()
{
	return mOrientation;
}

//----------------------------------------------------------------------------

bool Thing::isMoving()
{
	if (mStopped)
		if (mStopTime > Time::getSingleton().getTime() + THING_STOP_DELAY) 
			return false;

	return true;
}

//----------------------------------------------------------------------------

void Thing::generateBitStream(BitStream& bitstream)
{
	mLastUpdateTime = Time::getSingleton().getTime();

	bitstream.Write(ID_UPDATE_THING);
	bitstream.Write(mUID);
	bitstream.Write(getType());

	bitstream.Write(mPos.x);
	bitstream.Write(mPos.y);
	bitstream.Write(mPos.z);
	bitstream.Write(mVel.x);
	bitstream.Write(mVel.y);
	bitstream.Write(mVel.z);
	bitstream.Write(mOrientation);
}

//----------------------------------------------------------------------------

void Thing::interpretBitStream(BitStream& bitstream)
{
	Vector3 pos,vel;
	int pid,uid,type;
	Real orientation;

	bitstream.Read(pid);
	bitstream.Read(uid);
	bitstream.Read(type);

	bitstream.Read(pos.x);
	bitstream.Read(pos.y);
	bitstream.Read(pos.z);
	bitstream.Read(vel.x);
	bitstream.Read(vel.y);
	bitstream.Read(vel.z);
	bitstream.Read(orientation);

	assert(type == getType());

	setPosition(pos);
	setVelocity(vel);
	setOrientation(orientation);
}
//----------------------------------------------------------------------------

unsigned long Thing::lastUpdateTime()
{
	return mLastUpdateTime;
}
//----------------------------------------------------------------------------

}