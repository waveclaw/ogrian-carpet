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
OgrianModel.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a 3D mesh VisRep

/*------------------------------------*/

#include "OgrianModel.h"
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

unsigned long Model::msNextGeneratedNameExt = 1;

//----------------------------------------------------------------------------

Model::Model()
{
	std::ostringstream num("");
	num << msNextGeneratedNameExt++;
	mName = String("OgrianModel") + num.str();

	mWidth = 1;
	mHeight = 1;

	mInRenderer = false;
	mMeshName = "cube.mesh";
}

//----------------------------------------------------------------------------

Model::~Model()
{
	removeFromRenderer();
}

//----------------------------------------------------------------------------

void Model::setMesh(String mesh, Real scale, Real ratio)
{
	mMeshName = mesh;
	mScale = scale;
	mRatio = ratio;
	
	if (mInRenderer)
	{
		removeFromRenderer();
		addToRenderer();
	}
}

//----------------------------------------------------------------------------

ColourValue Model::getColour()
{
	return ColourValue::White;
}

//----------------------------------------------------------------------------

// returns true if it's in the renderer
bool Model::inRenderer()
{
	return mInRenderer;
}

//----------------------------------------------------------------------------

// set the position
void Model::setPosition(Vector3 pos)
{
	mPos = pos;

	if (mInRenderer)
		mNode->setPosition(pos);
}

//----------------------------------------------------------------------------

void Model::setWidth(Real width)
{
	mWidth = width;
	_updateScale();
}

//----------------------------------------------------------------------------

void Model::setHeight(Real height)
{
	mHeight = height;
	_updateScale();
}

//----------------------------------------------------------------------------

void Model::_updateScale()
{
	if (mInRenderer)
	{
		mNode->setScale(
			(mWidth/2)/mScale,
			(mHeight/2)/(mScale*mRatio),
			(mWidth/2)/mScale
		);
	}
}

//----------------------------------------------------------------------------

void Model::setMaterial(String material)
{
	mMaterial = material;

	if (mInRenderer)
		mEntity->setMaterialName(mMaterial);
}

//----------------------------------------------------------------------------

String Model::getMaterial()
{
	return mMaterial;
}

//----------------------------------------------------------------------------

void Model::addToRenderer()
{
	if (mInRenderer) return;
	mInRenderer = true;
		
	// create the entity
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	mEntity = sceneMgr->createEntity(mName,mMeshName);

	// attach the set
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mEntity);

	// apply its properteis
	setMaterial(mMaterial);
	setPosition(mPos);
	_updateScale();
}

//----------------------------------------------------------------------------

void Model::removeFromRenderer(bool makeDot)
{
	if (!mInRenderer) return;
	if (mNode == 0) return;

	mNode->detachObject(mName);
	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	// remove it from the scene
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	sceneMgr->removeEntity(mName);

	// null the mvars
	mEntity = 0;
	mNode = 0;

	mInRenderer = false;
}

//----------------------------------------------------------------------------

}