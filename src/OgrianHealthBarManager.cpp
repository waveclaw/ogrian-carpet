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
OgrianHealthBarManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the health bars that are shown above nearby damageable things

 *------------------------------------*/

#include "OgrianHealthBarManager.h"

using namespace Ogre;

template<> Ogrian::HealthBarManager * Singleton< Ogrian::HealthBarManager >::ms_Singleton = 0;
namespace Ogrian
{

//----------------------------------------------------------------------------

HealthBarManager::HealthBarManager()
{
	// create the billboardset
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	mBarSet = sceneMgr->createBillboardSet("HealthBarSet", CONI("HEALTH_BAR_POOL"));	

	// attach the set to a node
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mBarSet);
	mNode->setPosition(Vector3(0,0,0));

	mBarSet->setRenderQueueGroup(RENDER_QUEUE_9);
	mBarSet->setMaterialName("Ogrian/HealthBar");
}

//----------------------------------------------------------------------------

HealthBarManager::~HealthBarManager()
{
	mBarSet->clear();

	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	delete mBarSet;
	mBarSet = 0;
	
	mNode->detachAllObjects();
}

//----------------------------------------------------------------------------

Billboard* HealthBarManager::newBar(Vector3 pos, ColourValue colour)
{
	Billboard* bar = mBarSet->createBillboard(pos, colour);
	return bar;
}

//----------------------------------------------------------------------------

void HealthBarManager::remove(Billboard* bar)
{
	mBarSet->removeBillboard(bar);
}

//----------------------------------------------------------------------------

void HealthBarManager::clear()
{
	mBarSet->clear();
}

//----------------------------------------------------------------------------

HealthBarManager& HealthBarManager::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new HealthBarManager();
	}
    return Singleton<HealthBarManager>::getSingleton();
}

//----------------------------------------------------------------------------

}
