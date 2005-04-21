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
	mBackground = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/HUD/Background");

	mOverlay = (Overlay*)OverlayManager::getSingleton().getByName("Ogrian/HUD/Overlay");
	mMana = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Mana");
	mHealth = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Health");
	mMessage = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Message");
	mSpellName = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SpellName");

	mManas = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Manas");
	mShrines = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Shrines");
	mTowers = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Towers");
	mSentinels = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Sentinels");
	mGnomes = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Gnomes");
	mTicks = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Ticks");
	mAlbatrosses = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Albatrosses");
	
	mManasIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/ManasIcon");
	mShrinesIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/ShrinesIcon");
	mTowersIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/TowersIcon");
	mSentinelsIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/SentinelsIcon");
	mGnomesIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/GnomesIcon");
	mTicksIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/TicksIcon");
	mAlbatrossesIcon = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/AlbatrossesIcon");
	
	mScore = GuiManager::getSingleton().getGuiElement("Ogrian/HUD/Score");

	for (int i=0; i<NUM_SPELLS; i++)
	{
		std::ostringstream num("");
		num << i;
		mSpellIcons[i] = GuiManager::getSingleton().getGuiElement(String("Ogrian/HUD/SpellIcon/") + num.str());
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
	mBackground->show();
	mOverlay->show();
}

//----------------------------------------------------------------------------

void Hud::hide()
{
	mBackground->hide();
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
	setMessage(CONS("HUD_DEAD"));

	mMana->hide();
	mHealth->hide();
	mSpellName->hide();

	for (int i=0; i<NUM_SPELLS; i++)
		mSpellIcons[i]->hide();
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

void Hud::setNumManaBalls(int num)
{
	if (num > 0)
	{
		mManas->show();
		mManasIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mManas->setCaption(numstr.str());
	}
	else
	{
		mManas->hide();
		mManasIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumShrines(int num)
{
	if (num > 0)
	{
		mShrines->show();
		mShrinesIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mShrines->setCaption(numstr.str());
	}
	else
	{
		mShrines->hide();
		mShrinesIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumTowers(int num)
{
	if (num > 0)
	{
		mTowers->show();
		mTowersIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mTowers->setCaption(numstr.str());
	}
	else
	{
		mTowers->hide();
		mTowersIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumSentinels(int num)
{
	if (num > 0)
	{
		mSentinels->show();
		mSentinelsIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mSentinels->setCaption(numstr.str());
	}
	else
	{
		mSentinels->hide();
		mSentinelsIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumGnomes(int num)
{
	if (num > 0)
	{
		mGnomes->show();
		mGnomesIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mGnomes->setCaption(numstr.str());
	}
	else
	{
		mGnomes->hide();
		mGnomesIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumTicks(int num)
{
	if (num > 0)
	{
		mTicks->show();
		mTicksIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mTicks->setCaption(numstr.str());
	}
	else
	{
		mTicks->hide();
		mTicksIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setNumAlbatrosses(int num)
{
	if (num > 0)
	{
		mAlbatrosses->show();
		mAlbatrossesIcon->show();

		std::ostringstream numstr("");
		numstr << num;

		mAlbatrosses->setCaption(numstr.str());
	}
	else
	{
		mAlbatrosses->hide();
		mAlbatrossesIcon->hide();
	}
}

//----------------------------------------------------------------------------

void Hud::setScore(int score)
{
	std::ostringstream numstr("");
	numstr << score;

	mScore->setCaption(String(CONS("HUD_SCORE")) + numstr.str());
}

//----------------------------------------------------------------------------

void Hud::frame()
{
	if (mMessageClearTime && mMessageClearTime < Clock::getSingleton().getTime())
	{
		if (!mMana->isVisible())
			setMessage(CONS("HUD_DEAD"));
		else
			setMessage("");
	}
}

//----------------------------------------------------------------------------

void Hud::setMessage(String msg, bool temp)
{
	// ignore messages when in ghost mode
	if (!mMana->isVisible()) return;

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