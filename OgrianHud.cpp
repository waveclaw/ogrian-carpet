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
#include "OgrianConst.h"

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

	ColourValue colour;
	colour.r = CONR("red")/255.0;
	colour.g = CONR("green")/255.0;
	colour.b = CONR("blue")/255.0;

	for (int i=0; i<NUM_SPELLS; i++)
	{
		std::ostringstream num("");
		num << i;
		mSpellIcons[i] = GuiManager::getSingleton().getGuiElement(String("Ogrian/HUD/SpellIcon/") + num.str());
		mSpellIcons[i]->setColour(colour);
	}

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

void Hud::toggle()
{
	if (mOverlay->isVisible())
		hide();
	else
		show();
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

void Hud::reinit()
{
	mMana->show();
	mHealth->show();
	mSpellName->show();

	for (int i=0; i<NUM_SPELLS; i++)
		mSpellIcons[i]->show();

	setMessage("");
}

//----------------------------------------------------------------------------

void Hud::makeGhost()
{
	mMana->hide();
	mHealth->hide();
	mSpellName->hide();

	for (int i=0; i<NUM_SPELLS; i++)
		mSpellIcons[i]->hide();

	setMessage(CONS("HUD_DEAD"));
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

	mMana->setCaption(String(CONS("HUD_MANA")) +active.str() + "/" + base.str());
}

//----------------------------------------------------------------------------

void Hud::frame()
{
	if (mMessageClearTime && mMessageClearTime < Clock::getSingleton().getTime())
		setMessage("");
}

//----------------------------------------------------------------------------

void Hud::setMessage(String msg, bool temp)
{
	mMessage->setCaption(msg);

	if (temp)
	{
		mMessageClearTime = CONT("HUD_MESSAGE_TIME") + Clock::getSingleton().getTime();
	}
	else
	{
		mMessageClearTime = 0;
	}
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
		setHealth(String(CONS("HUD_HEALTH")) + num.str());
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