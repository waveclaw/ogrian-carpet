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
OgrianSpellManager.h
Original Author: Mike Prosser
Additional Authors: 

Description: This manages the spells for the HUD, etc

/*------------------------------------*/


#ifndef __OgrianSpellManager_H__
#define __OgrianSpellManager_H__

#include <Ogre.h>
#include <OgreSingleton.h>
#include "OgrianSpell.h"

using namespace Ogre;

namespace Ogrian
{


#define SPELL_CLAIM				0
#define SPELL_BUILD				1
#define SPELL_FIREBALL			2
#define SPELL_AKIMBO_FIREBALL	3
#define SPELL_FIRESTORM			4
#define NUM_SPELLS				5

class SpellManager : public Singleton< SpellManager >
{
public:
	virtual ~SpellManager();
    static SpellManager& getSingleton(void);

	virtual void enableSpell(int spell);
	virtual void disableSpell(int spell);

	virtual void disableAllSpells();

	virtual void readyNextSpell();
	virtual void readyPrevSpell();
	virtual void readyDefaultSpell();

	virtual int getManaCost();

	virtual Real castSpell();

private:
	SpellManager();
	virtual void readyCurrentSpell();

	int mCurrentSpell;
	Spell* mSpells[NUM_SPELLS];

};

}

#endif