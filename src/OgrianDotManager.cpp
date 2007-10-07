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
OgrianDotManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the 'dots' that are used in place of sprites
far from the camera

 *------------------------------------*/

#include "OgrianDotManager.h"


using namespace Ogre;

template<> Ogrian::DotManager * Singleton< Ogrian::DotManager >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

DotManager::DotManager()
{
	// create the billboardset
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	mDotSet = sceneMgr->createBillboardSet("DotSet", CONI("DOT_POOL"));	

	// attach the set to a node
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mDotSet);
	mNode->setPosition(Vector3(0,0,0));

	mDotSet->setRenderQueueGroup(RENDER_QUEUE_7);
	mDotSet->setMaterialName("Ogrian/Dot");

	// for the foliage //

	// create the billboardset
	mFoliageDotSet = sceneMgr->createBillboardSet("FoliageDotSet", CONI("DOT_POOL"));	

	// attach the set to a node
	mFoliageNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mFoliageNode->attachObject(mFoliageDotSet);
	mFoliageNode->setPosition(Vector3(0,0,0));

	mFoliageDotSet->setRenderQueueGroup(RENDER_QUEUE_6);
	mFoliageDotSet->setMaterialName("Ogrian/Dot");
			
	mFoliageDotSet->setBillboardType(BBT_ORIENTED_COMMON);
	mFoliageDotSet->setCommonDirection(Vector3::UNIT_Y);
}

//----------------------------------------------------------------------------

DotManager::~DotManager()
{
	mDotSet->clear();
	mFoliageDotSet->clear();

	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	delete mDotSet;
	mDotSet = 0;
	
	delete mFoliageDotSet;
	mFoliageDotSet = 0;

	mNode->detachAllObjects();
}

//----------------------------------------------------------------------------

Billboard* DotManager::newDot(Vector3 pos, ColourValue colour)
{
	Billboard* dot = mDotSet->createBillboard(pos, colour);
	return dot;
}

//----------------------------------------------------------------------------

void DotManager::remove(Billboard* dot)
{
	mDotSet->removeBillboard(dot);
}

//----------------------------------------------------------------------------

Billboard* DotManager::newFoliageDot(Vector3 pos)
{
	Billboard* dot = mFoliageDotSet->createBillboard(pos);
	return dot;
}

//----------------------------------------------------------------------------

void DotManager::removeFoliage(Billboard* dot)
{
	mFoliageDotSet->removeBillboard(dot);
}

//----------------------------------------------------------------------------

void DotManager::setFoliageMaterial(String material)
{
	mFoliageDotSet->setMaterialName(material);
}

//----------------------------------------------------------------------------

void DotManager::reset()
{
	mDotSet->clear();
	mFoliageDotSet->clear();
}

//----------------------------------------------------------------------------

DotManager& DotManager::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new DotManager();
	}
    return Singleton<DotManager>::getSingleton();
}

//----------------------------------------------------------------------------

}
