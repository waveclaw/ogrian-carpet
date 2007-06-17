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

 *------------------------------------*/

#include "OgrianOrientedSprite.h"
#include "OgrianRenderer.h"
#include "OgrianDotManager.h"

using namespace Ogre;

namespace Ogrian
{

//----------------------------------------------------------------------------

OrientedSprite::OrientedSprite()
{
	mCurrentPose = 0;
	mCurrentSprite = 0;
	mDot = 0;
	mInRenderer = false;

	frame();
}

//----------------------------------------------------------------------------

OrientedSprite::~OrientedSprite()
{
	removeFromRenderer();

	// remove the dot
	if (mDot) 
	{
		DotManager::getSingleton().remove(mDot);
		mDot = 0;
	}

	while (mPoses.size() > 0)
	{
		delete mPoses[mPoses.size()-1];
		mPoses.pop_back();
	}
	mCurrentPose = 0;
	mCurrentSprite = 0;
}

//----------------------------------------------------------------------------

void OrientedSprite::setColour(ColourValue& colour)
{
	mColour = colour;

	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setColour(mColour);
	else if (mDot)
		mDot->setColour(colour);
}

//----------------------------------------------------------------------------

ColourValue OrientedSprite::getColour()
{
	return mColour;
}

//----------------------------------------------------------------------------

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
	else if (mDot)
		mDot->setPosition(pos);
}

//----------------------------------------------------------------------------

void OrientedSprite::setWidth(Real width)
{
	mWidth = width;

	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setWidth(width);
	else if (mDot)
		mDot->setDimensions(mWidth*CONR("DOT_SIZE"),mHeight*CONR("DOT_SIZE"));
}

//----------------------------------------------------------------------------

void OrientedSprite::setHeight(Real height)
{
	mHeight = height;
	
	if (mInRenderer && mCurrentSprite != 0)
		mCurrentSprite->setHeight(height);
	else if (mDot)
		mDot->setDimensions(mWidth*CONR("DOT_SIZE"),mHeight*CONR("DOT_SIZE"));
}

//----------------------------------------------------------------------------

void OrientedSprite::addToRenderer()
{
	// dont do this twice!
	if (mInRenderer) return;

	// remove the dot
	if (mDot) 
	{
		DotManager::getSingleton().remove(mDot);
		mDot = 0;
	}

	if (mCurrentSprite != 0)
	{
		mCurrentSprite->setPosition(mPos);
		mCurrentSprite->setWidth(mWidth);
		mCurrentSprite->setHeight(mHeight);
		mCurrentSprite->setColour(mColour);
		mCurrentSprite->addToRenderer();
	}
	mInRenderer = true;
}

//----------------------------------------------------------------------------

void OrientedSprite::removeFromRenderer(bool makeDot)
{
	// dont do this twice!
	if (!mInRenderer) return;

	if (mCurrentSprite != 0)
		mCurrentSprite->removeFromRenderer(false);

	// make a dot
	if (!mDot && makeDot)
	{
		mDot = DotManager::getSingleton().newDot(mPos, mColour);
		mDot->setDimensions(mWidth*CONR("DOT_SIZE"), mHeight*CONR("DOT_SIZE"));
	}

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
	if (index >= (int)mPoses.size())
		Exception( Exception::ERR_INTERNAL_ERROR, "Error: Pose not found.",
				"OrientedSprite::setPose" );

	mCurrentPose = mPoses[index];
	mPoseIndex = index;

	frame();
}

//----------------------------------------------------------------------------

int OrientedSprite::getPose()
{
	return mPoseIndex;
}

//----------------------------------------------------------------------------

void OrientedSprite::clearPoses()
{
	// delete all the poses
	for (int i=0; i<(int)mPoses.size(); i++)
		delete mPoses[i];

	mPoses.clear();

	mCurrentPose = 0;
	mCurrentSprite = 0;
}

//----------------------------------------------------------------------------

void OrientedSprite::setOrientation(Radian orientation)
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
			if (mCurrentSprite != 0) mCurrentSprite->removeFromRenderer(false); 

			newSprite->setPosition(mPos);
			newSprite->setWidth(mWidth);
			newSprite->setHeight(mHeight);
			newSprite->setColour(mColour);
			newSprite->addToRenderer();
		}

		mCurrentSprite = newSprite;
	}
}

//----------------------------------------------------------------------------

}
