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
	mMana = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Mana");
	mHealth = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Health");
	mMessage = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Message");
	mSpellName = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SpellName");

	mSpellIcons[0] = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SpellIcon/0");
	mSpellIcons[1] = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SpellIcon/1");
	mSpellIcons[2] = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SpellIcon/2");
	setMessage("");

	mBaseMana = 0;
	mActiveMana = 0;
	setMana();
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

void Hud::makeGhost()
{
	mMana->hide();
	mHealth->hide();
	mSpellName->hide();

	for (int i=0; i<NUM_SPELLS; i++)
		mSpellIcons[i]->hide();

	setMessage("you   are   dead   : (");
}

//----------------------------------------------------------------------------

void Hud::setBaseMana(int baseMana)
{
	mBaseMana = baseMana;
	setMana();
}

//----------------------------------------------------------------------------

void Hud::setActiveMana(int activeMana)
{
	mActiveMana = activeMana;
	setMana();
}

//----------------------------------------------------------------------------

void Hud::setMana()
{
	std::ostringstream active("");
	active << mActiveMana;

	std::ostringstream base("");
	base << mBaseMana;

	mMana->setCaption(String("Mana: ") +active.str() + "/" + base.str());
}

//----------------------------------------------------------------------------

void Hud::setMessage(String msg)
{
	mMessage->setCaption(msg);
}

//----------------------------------------------------------------------------

void Hud::setSpellName(String name)
{
	mSpellName->setCaption(name);
}

//----------------------------------------------------------------------------

void Hud::setSpellIcon(int num, String material)
{
	if (num >= NUM_SPELLS) return;

	mSpellIcons[num]->setMaterialName(material);
}

//----------------------------------------------------------------------------

void Hud::setHealth(int health)
{
	if (health <= 0)
		setHealth("Game Over - You are Dead!");
	else
	{
        std::ostringstream num("");
		num << health;
		setHealth(String("Health: ") + num.str());
	}
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