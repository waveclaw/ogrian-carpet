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
OgrianOrientedSprite.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The OrientedSprite is a visual representation that can represent an object from each of 
the eight cardinal directions and also from any of an arbitrary number of poses. 

/*------------------------------------*/

#include "OgrianOrientedSprite.h"
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

OrientedSprite::OrientedSprite()
{
	mCurrentPose = 0;
	mCurrentSprite = 0;
	mInRenderer = false;

	frame();
}

//----------------------------------------------------------------------------

OrientedSprite::~OrientedSprite()
{
	removeFromRenderer();

	while (mPoses.size() > 0)
	{
		delete mPoses[mPoses.size()-1];
		mPoses.erase(mPoses.end());
	}
}

// returns true if it's in the renderer
bool OrientedSprite::inRenderer()
{
	return mInRenderer;
}

//----------------------------------------------------------------------------

// set the position
void OrientedSprite::setPosition(Vector3 pos)
{
	mPos = pos;

	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setPosition(pos);
}

//----------------------------------------------------------------------------

void OrientedSprite::setWidth(Real width)
{
	mWidth = width;

	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setWidth(width);
}

//----------------------------------------------------------------------------

void OrientedSprite::setHeight(Real height)
{
	mHeight = height;
	
	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setHeight(height);
}

//----------------------------------------------------------------------------

void OrientedSprite::addToRenderer()
{
	// dont do this twice!
	if (mInRenderer) return;

	if (mCurrentSprite != 0)
	{
		mCurrentSprite->setPosition(mPos);
		mCurrentSprite->setWidth(mWidth);
		mCurrentSprite->setHeight(mHeight);
		mCurrentSprite->addToRenderer();
	}
	mInRenderer = true;
}

//----------------------------------------------------------------------------

void OrientedSprite::removeFromRenderer()
{
	// dont do this twice!
	if (!mInRenderer) return;

	if (mCurrentSprite != 0)
		mCurrentSprite->removeFromRenderer();

	mInRenderer = false;
}

//----------------------------------------------------------------------------

int OrientedSprite::addPose(String basename)
{
	Pose* pose = new Pose(basename);
	mPoses.push_back(pose);
	return (int)mPoses.size()-1;
}

//----------------------------------------------------------------------------

void OrientedSprite::setPose(int index)
{
	mCurrentPose = mPoses[index];

	frame();
}

//----------------------------------------------------------------------------

void OrientedSprite::setOrientation(Real orientation)
{
	mOrientation = orientation;
}

//----------------------------------------------------------------------------

void OrientedSprite::frame()
{
	if (mCurrentPose == 0) return;

	Sprite* newSprite = mCurrentPose->getSprite(
		Renderer::getSingleton().getCamera()->getPosition(), mPos, mOrientation);

	if (mCurrentSprite != newSprite)
	{
		if (mInRenderer)
		{
			if (mCurrentSprite != 0) mCurrentSprite->removeFromRenderer(); 

			newSprite->setPosition(mPos);
			newSprite->setWidth(mWidth);
			newSprite->setHeight(mHeight);
			newSprite->addToRenderer();
		}

		mCurrentSprite = newSprite;
	}
}

//----------------------------------------------------------------------------

}