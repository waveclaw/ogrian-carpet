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
OgrianManaThing.h
Original Author: Mike Prosser
Additional Authors: 


Description: the ManaThing is a ball of mana
They drift downhill and combine when they touch. 
They have some amount of mana.
When they combine, one transfers all of its amount to the other.
When they are claimed, they move to the castle

 *------------------------------------*/


#ifndef __OgrianManaThing_H__
#define __OgrianManaThing_H__

#include <Ogre.h>

using namespace Ogre;

namespace Ogrian
{

class ManaThing : public Thing
{
public:
	ManaThing(int amount=1, Vector3 pos = Vector3(0,0,0));

	// change the colour to reflect team ownership
	virtual void setTeamNum(int teamNum);

	virtual void claim(int teamNum);

	// setting the amount automatically sets the scale
	virtual void setAmount(int amount);

	// get the amount of mana this represents
	virtual inline int getAmount();

	// When two manas collide, they combine - one takes on all of the amount of the other
	// and the other disapears
	virtual void collided(Thing* e);

	// float
	virtual void move(Real time);
	virtual void setPosition(Vector3 pos);

	// move downhill at a constant slow velocity if unclaimed
	virtual void think();

	virtual ThingType getType()	{	return MANATHING; }

	virtual void destroy();

private:
	int mAmount;
	bool mStopped;
};

}

#endif
