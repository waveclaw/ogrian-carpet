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
OgrianSprite.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: A Sprite is a billboard that is properly depth
sorted and has a unique material. 

/*------------------------------------*/

#include "OgrianSprite.h"
#include "OgrianRenderer.h"
#include "OgrianDotManager.h"

using namespace Ogre;

namespace Ogrian
{

unsigned long Sprite::msNextGeneratedNameExt = 1;

//----------------------------------------------------------------------------

Sprite::Sprite(String name, bool fixed_y)
{
	mBbset = 0;
	mBillboard = 0;
	mDot = 0;
	mNode = 0;
	mName = name;
	mFixed_y = fixed_y;

	if (StringUtil::startsWith(mName,"auto"))
	{
			std::ostringstream num("");
			num << msNextGeneratedNameExt++;
			mName += num.str();
	}

	mWidth = -1;
	mHeight = -1;
	mColour = ColourValue::White;

	mInRenderer = false;

}

//----------------------------------------------------------------------------

Sprite::~Sprite()
{
	removeFromRenderer();

	// remove the dot
	if (mDot) 
	{
		DotManager::getSingleton().remove(mDot);
		mDot = 0;
	}
}

//----------------------------------------------------------------------------

void Sprite::setColour(ColourValue& colour)
{
	mColour = colour;

	if (mInRenderer) 
		mBillboard->setColour(colour);
	else if (mDot)
		mDot->setColour(colour);
}

//----------------------------------------------------------------------------

ColourValue Sprite::getColour()
{
	return mColour;
}

//----------------------------------------------------------------------------

// returns true if it's in the renderer
bool Sprite::inRenderer()
{
	return mInRenderer;
}

//----------------------------------------------------------------------------

// set the position
void Sprite::setPosition(Vector3 pos)
{
	mPos = pos;

	if (mInRenderer)
		mNode->setPosition(pos);
	else if (mDot)
		mDot->setPosition(pos);
}

//----------------------------------------------------------------------------

void Sprite::setWidth(Real width)
{
	mWidth = width;

	if (mInRenderer)
		mBillboard->setDimensions(mWidth,mHeight);
	else if (mDot)
		addToRenderer(); // not sure why this is necessary, but it is
}

//----------------------------------------------------------------------------

void Sprite::setHeight(Real height)
{
	mHeight = height;
	
	if (mInRenderer)
		mBillboard->setDimensions(mWidth,mHeight);
	else if (mDot) 
		addToRenderer(); // not sure why this is necessary, but it is
}

//----------------------------------------------------------------------------

void Sprite::setRotation(Radian rotation)
{
	if (mInRenderer)
		mBillboard->setRotation(rotation);
}

//----------------------------------------------------------------------------

void Sprite::setMaterial(String material)
{
	mMaterial = material;

	if (mInRenderer)
		mBbset->setMaterialName(material);
}

//----------------------------------------------------------------------------

String Sprite::getMaterial()
{
	return mMaterial;
}

//----------------------------------------------------------------------------

void Sprite::addToRenderer()
{
	// dont do this twice!
	if (mInRenderer) return;

	// remove the dot
	if (mDot) 
	{
		DotManager::getSingleton().remove(mDot);
		mDot = 0;
	}

	// create the billboardset
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();

	std::ostringstream num("");
	num << msNextGeneratedNameExt++;
	mName += "_" + num.str();

	mBbset = sceneMgr->createBillboardSet(mName,1);
	mBillboard = mBbset->createBillboard(0, 0, 0);
	

	mBbset->setRenderQueueGroup(RENDER_QUEUE_7);

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
	setPosition(mPos);
	setWidth(mWidth);
	setColour(mColour);
}

//----------------------------------------------------------------------------

void Sprite::removeFromRenderer(bool makeDot)
{
	// dont do this twice!
	if (!mInRenderer) return;

	// remove it from the scene
	mBbset->clear();
	mNode->detachAllObjects();
	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	delete mBbset;

	// null the mvars
	mBbset = 0;
	mBillboard = 0;
	mNode = 0;

	// make a dot (unless it's pure white)
	if (!mDot && makeDot && mColour != ColourValue::White)
	{
		mDot = DotManager::getSingleton().newDot(mPos, mColour);
		if (mDot)
			mDot->setDimensions(mWidth*CONR("DOT_SIZE"), mHeight*CONR("DOT_SIZE"));
	}

	mInRenderer = false;
}

//----------------------------------------------------------------------------

}