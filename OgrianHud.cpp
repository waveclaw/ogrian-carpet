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
OgrianHud.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The heads up display

/*------------------------------------*/

#include "OgrianHud.h"

using namespace Ogre;

template<> Ogrian::Hud * Singleton< Ogrian::Hud >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

Hud::Hud()
{
	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/HUD/Overlay");
	mScore = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Score");
	mHealth = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Health");
}

//----------------------------------------------------------------------------

Hud::~Hud()
{

}

//----------------------------------------------------------------------------

void Hud::show()
{
	mOverlay->show();
}

//----------------------------------------------------------------------------

void Hud::hide()
{
	mOverlay->hide();
}

//----------------------------------------------------------------------------

void Hud::setScore(int score)
{
	std::ostringstream num("");
	num << score;
	setScore(String("Mana: ") + num.str());
}

//----------------------------------------------------------------------------

void Hud::setScore(String score)
{
	mScore->setCaption(score);
}

//----------------------------------------------------------------------------

void Hud::setHealth(int health)
{
	std::ostringstream num("");
	num << health;
	setHealth(String("Health: ") + num.str());
}

//----------------------------------------------------------------------------

void Hud::setHealth(String health)
{
	mHealth->setCaption(health);
}

//----------------------------------------------------------------------------

Hud& Hud::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Hud();
	}
    return Singleton<Hud>::getSingleton();
}

//----------------------------------------------------------------------------

}