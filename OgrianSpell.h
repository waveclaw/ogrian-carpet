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
OgrianSpell.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a spell

/*------------------------------------*/


#ifndef __OgrianSpell_H__
#define __OgrianSpell_H__

#include <Ogre.h>
#include "OgrianConst.h"

using namespace Ogre;

namespace Ogrian
{

class Spell
{
public:
	Spell() { mEnabled = false; }
	virtual ~Spell() {}

	// cast this spell
	virtual void cast(Vector3 pos, Vector3 dir) = 0;

	virtual Real getCastPeriod() = 0;

	virtual int getManaCost() = 0;

	virtual String getString() = 0;

	virtual String getReadyMaterial() = 0; 

	virtual String getEnabledMaterial() = 0; 

	virtual String getDisabledMaterial() = 0; 

	virtual void setEnabled(bool enabled) { mEnabled = enabled; }

	virtual bool getEnabled() { return mEnabled; }

private:
	bool mEnabled;

};

}

#endif