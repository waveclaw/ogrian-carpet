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
OgrianHud.h
Original Author: Mike Prosser
Additional Authors: 

Description: The heads up display

 *------------------------------------*/


#ifndef __OgrianHud_H__
#define __OgrianHud_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianConst.h"
#include "OgrianClock.h"
#include "OgrianSpellManager.h"



using namespace Ogre;

namespace Ogrian
{

class Hud : public Singleton< Hud >
{
public:
	virtual ~Hud();
    static Hud& getSingleton(void);

	// call this every frame
	void frame();

	// show the HUD
	void show();

	// hide the HUD
	void hide();

	// toggle the HUD on/off
	void toggle();

	// change the HUD to ghost mode
	void makeGhost();

	// change the HUD back to normal mode
	void reinit();

	// set the ManaStone
	void setManaStone(int manaStone);

	// set the Mana
	void setMana(int mana);

	// set the health 
	void setHealth(int health);

	// set the hud counters
	void setNumManaBalls(int num);
	void setNumShrines(int num);
	void setNumTowers(int num);
	void setNumSentinels(int num);
	void setNumGnomes(int num);
	void setNumTicks(int num);
	void setNumAlbatrosses(int num);

	// set the score
	void setScore(int score);

	// set the message, and if temp, clear it after a time
	void setMessage(String msg, bool temp=false);

	// set the spell name
	void setSpellName(String name);

	// set spell icons
	void setSpellIcon(int num, String material);

private:
	Hud();
	void setManaBar(); // acording to mMana and mManaStone

	int mManaAmount;
	int mManaStoneAmount;

	Time mMessageClearTime;

	bool mGhost;

	Overlay* mHudOverlay;
	Overlay* mBackgroundOverlay;
	Overlay* mGhostOverlay;

	GuiElement* mMana;
	GuiElement* mManaStone;
	GuiElement* mHealth;
	GuiElement* mMessage;
	GuiElement* mSpellName;

	GuiElement* mManas;
	GuiElement* mShrines;
	GuiElement* mTowers;
	GuiElement* mSentinels;
	GuiElement* mGnomes;
	GuiElement* mTicks;
	GuiElement* mAlbatrosses;

	GuiElement* mManasIcon;
	GuiElement* mShrinesIcon;
	GuiElement* mTowersIcon;
	GuiElement* mSentinelsIcon;
	GuiElement* mGnomesIcon;
	GuiElement* mTicksIcon;
	GuiElement* mAlbatrossesIcon;

	GuiElement* mScore;

	GuiElement* mHealthBar;
	GuiElement* mManaBar;

	GuiElement* mSpellIcons[NUM_SPELLS];
};

}

#endif
