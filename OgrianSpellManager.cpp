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
OgrianSpellManager.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the spells for the HUD, etc

/*------------------------------------*/

#include "OgrianSpellManager.h"
#include "OgrianHud.h"
#include "OgrianFireballSpell.h"
#include "OgrianClaimSpell.h"
#include "OgrianBuildSpell.h"

using namespace Ogre;

template<> Ogrian::SpellManager * Singleton< Ogrian::SpellManager >::ms_Singleton = 0;
namespace Ogrian
{


//----------------------------------------------------------------------------

SpellManager::SpellManager()
{
	mSpells[SPELL_BUILD] = new BuildSpell();
	mSpells[SPELL_CLAIM] = new ClaimSpell();
	mSpells[SPELL_FIREBALL] = new FireballSpell();

	disableAllSpells();
	enableSpell(SPELL_BUILD);
	enableSpell(SPELL_CLAIM);
	enableSpell(SPELL_FIREBALL);

	readyDefaultSpell();
}

//----------------------------------------------------------------------------

SpellManager::~SpellManager()
{

}

//----------------------------------------------------------------------------

Real SpellManager::castSpell()
{
	Vector3 pos = Renderer::getSingleton().getCamera()->getPosition();
	Vector3 dir = Renderer::getSingleton().getCamera()->getDirection();

	mSpells[mCurrentSpell]->makeThing(pos, dir);

	return mSpells[mCurrentSpell]->getCastPeriod();
}

//----------------------------------------------------------------------------

void SpellManager::disableAllSpells()
{
	for (int i=0; i<NUM_SPELLS; i++)
		disableSpell(i);
}

//----------------------------------------------------------------------------

void SpellManager::disableSpell(int spell)
{
	if (spell >= NUM_SPELLS) return;

	mSpells[spell]->setEnabled(false);
}

//----------------------------------------------------------------------------

void SpellManager::enableSpell(int spell)
{
	if (spell >= NUM_SPELLS) return;

	mSpells[spell]->setEnabled(true);
}

//----------------------------------------------------------------------------

void SpellManager::readyDefaultSpell()
{
	mCurrentSpell = 0;
	readySpell(mCurrentSpell);
}

//----------------------------------------------------------------------------

void SpellManager::readyNextSpell()
{
	// increment the current spell
	mCurrentSpell++;

	// wrap around
	if (mCurrentSpell == NUM_SPELLS) mCurrentSpell = 0;

	// dont select a disabled spell
	while (!mSpells[mCurrentSpell]->getEnabled())
	{
		mCurrentSpell++;
		if (mCurrentSpell == NUM_SPELLS)
		{
			mCurrentSpell = 0;
			break;
		}
	}
	
	readySpell(mCurrentSpell);
}

//----------------------------------------------------------------------------

void SpellManager::readyPrevSpell()
{
	// increment the current spell
	mCurrentSpell--;

	// wrap around
	if (mCurrentSpell < 0 ) mCurrentSpell = NUM_SPELLS-1;

	// dont select a disabled spell
	while (!mSpells[mCurrentSpell]->getEnabled())
	{
		mCurrentSpell--;
		if (mCurrentSpell == -1)
		{
			mCurrentSpell = NUM_SPELLS;
			break;
		}
	}

	readySpell(mCurrentSpell);
}

//----------------------------------------------------------------------------

void SpellManager::readySpell(int spell)
{
	// ignore invalid spells
	if (spell >= NUM_SPELLS) return;
	if (spell < 0) return;

	// dont ready disabled spells
	if (!mSpells[spell]->getEnabled()) return;

	// show the right icons
	for (int i=0; i<NUM_SPELLS; i++)
	{
		// if its the ready spell
		if (i == mCurrentSpell) 
			Hud::getSingleton().setSpellIcon(i, mSpells[i]->getReadyMaterial());

		// if its enabled
		else if (mSpells[i]->getEnabled())
			Hud::getSingleton().setSpellIcon(i, mSpells[i]->getEnabledMaterial());

		// if its disabled
		else 
			Hud::getSingleton().setSpellIcon(i, mSpells[i]->getDisabledMaterial());
	}

	// show the name
	Hud::getSingleton().setSpellName(mSpells[spell]->getString());
}

//----------------------------------------------------------------------------

SpellManager& SpellManager::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new SpellManager();
	}
    return Singleton<SpellManager>::getSingleton();
}

//----------------------------------------------------------------------------

}