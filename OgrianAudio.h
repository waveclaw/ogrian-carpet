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
OgrianAudio.h
Original Author: Mike Prosser
Additional Authors: 

Description: The audio handler
/*------------------------------------*/

#ifndef __OgrianAudio_H__
#define __OgrianAudio_H__

#include <Ogre.h>
#include <fmod.h>

using namespace Ogre;

namespace Ogrian
{

class Audio : public Singleton< Audio >
{
public:
	virtual void playSound(String filename, Vector3 pos);

	virtual void frame(Real time);

	static Audio& getSingleton(void);

private:
	Audio();
	virtual ~Audio();
    float listenerpos[3]; 
};

}

#endif