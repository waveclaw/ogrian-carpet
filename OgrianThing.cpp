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
#include "OgrianModel.h"

#include <iostream>
#include <sstream>

namespace Ogrian
{

unsigned long Thing::msNextGeneratedNameExt = 0;

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
	mFlickerPeriod = 0;
	mLastRotTime = 0;
	mLastRotDir = false;
	mUpdateType = PERIODIC;
	mUpdateRequested = false;
	mTeamNum = -1;
	
	// name it
	std::ostringstream num("");
	num << msNextGeneratedNameExt++;
	mName = prefix + "_" + num.str();
	//LogManager::getSingleton().logMessage(String("Making Thing: ") + mName);

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
	else if (visrep == MODEL)
	{
		Model* model = new Model();
		mVisRep = model;
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

	// request an update
	requestUpdate();
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

void Thing::setColour(ColourValue& colour)
{
	mVisRep->setColour(colour);
}

//----------------------------------------------------------------------------

ColourValue Thing::getColour()
{
	return mVisRep->getColour();
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

void Thing::setFlickerPeriod(Real time)
{
	mFlickerPeriod = time*1000;
}

//----------------------------------------------------------------------------

void Thing::setVelocity(Vector3 vel)
{
	mVel = vel;
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
	
	if (mFlickerPeriod > 0)
	{
		// periodically rotate 180 degrees
		if (mLastRotTime + mFlickerPeriod < Time::getSingleton().getTime())
		{
			getVisRep()->setRotation(Degree(mLastRotDir?0:180));
			mLastRotTime = Time::getSingleton().getTime();
			mLastRotDir = !mLastRotDir;
		}
	}

	// call collidedground when it hits the ground
	if (!Multiplayer::getSingleton().isClient())
		if (getGroundY() > getPosition().y && isAlive()) 
			collidedGround();
}

//----------------------------------------------------------------------------

String Thing::getString()
{
	std::ostringstream num("");
	num << mUID;
	return String(mName) + "(" + /*getType() +*/ ") #" + num.str();
}

//----------------------------------------------------------------------------

// only render things that are close to the camera
void Thing::_updateVisibility()
{
	// check the distance from the camera
	Thing* cam = Renderer::getSingleton().getCameraThing();
	if (cylinderDistance(cam) < CONR("THING_CULL_DIST"))
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
	return HeightMap::getSingleton().getHeightAt(pos.x, pos.z);
}

//----------------------------------------------------------------------------

void Thing::playSound(int id)
{
	mSoundId = id;
	mPlayingSound = true;

	if (mInEarshot)
		mCurrentSound = Audio::getSingleton().playSound(mSoundId, getPosition());
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
	mInEarshot = cylinderDistance(cam) < CONR("THING_EARSHOT");

	if (mPlayingSound)
	{
		// silence it if it has gone out of range
		if (wasInEarshot && !mInEarshot)
			Audio::getSingleton().stopSound(mCurrentSound);

		// start the sound again when it comes in range
		if (!wasInEarshot && mInEarshot)
			mCurrentSound = Audio::getSingleton().playSound(mSoundId, getPosition());
	}
}

//----------------------------------------------------------------------------

void Thing::setOrientation(Real direction)
{
	mOrientation = direction;
	mVisRep->setOrientation(Radian(direction));
}

//----------------------------------------------------------------------------

Real Thing::getOrientation()
{
	return mOrientation;
}

//----------------------------------------------------------------------------

void Thing::setTeamNum(int teamNum)
{
	mTeamNum = teamNum;
}

//----------------------------------------------------------------------------

int Thing::getTeamNum()
{
	return mTeamNum;
}

//----------------------------------------------------------------------------

void Thing::generateBitStream(BitStream& bitstream, int pid)
{
	mLastUpdateTime = Time::getSingleton().getTime();
	mUpdateRequested = false;

	bitstream.Write(pid);
	bitstream.Write(mUID);
	bitstream.Write(getType());

	bitstream.Write(mPos.x);
	bitstream.Write(mPos.y);
	bitstream.Write(mPos.z);
	bitstream.Write(mVel.x);
	bitstream.Write(mVel.y);
	bitstream.Write(mVel.z);

	bitstream.Write(mOrientation);
	bitstream.Write(mHeight);
	bitstream.Write(mWidth);

	ColourValue colour = getColour();
	bitstream.Write(colour.r);
	bitstream.Write(colour.b);
	bitstream.Write(colour.g);
}

//----------------------------------------------------------------------------

void Thing::interpretBitStream(BitStream& bitstream)
{
	Vector3 pos,vel;
	int pid,uid,type;
	Real orientation, height, width;

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
	bitstream.Read(height);
	bitstream.Read(width);

	ColourValue colour;
	bitstream.Read(colour.r);
	bitstream.Read(colour.b);
	bitstream.Read(colour.g);

	assert(type == getType());

	setPosition(pos);
	setVelocity(vel);
	setOrientation(orientation);
	setHeight(height);
	setWidth(width);
	setColour(colour);
}

//----------------------------------------------------------------------------

unsigned long Thing::lastUpdateTime()
{
	return mLastUpdateTime;
}

//----------------------------------------------------------------------------

void Thing::setUpdateType(ThingUpdateType type)
{
	mUpdateType = type;
}

//----------------------------------------------------------------------------

ThingUpdateType Thing::getUpdateType()
{
	return mUpdateType;
}

//----------------------------------------------------------------------------

void Thing::requestUpdate()
{
	mUpdateRequested = true;
}

//----------------------------------------------------------------------------

bool Thing::updateRequested()
{
	return mUpdateRequested;
}

//----------------------------------------------------------------------------

}