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

#include "OgrianAudio.h"
#include "OgrianRenderer.h"
#include "OgrianCameraThing.h"

template<> Ogrian::Audio * Singleton< Ogrian::Audio >::ms_Singleton = 0;

namespace Ogrian
{

Audio::Audio()
{
	FSOUND_Init(44100, 32, 0);
}

Audio::~Audio()
{

}

void Audio::playSound(String filename, Vector3 pos)
{
	// load the sound
	FSOUND_SAMPLE* sound = FSOUND_Sample_Load(FSOUND_FREE, filename, FSOUND_LOOP_NORMAL, 0, 0);

	// error if not found
	if (sound == 0) return;

	// play the sound
	int channel = FSOUND_PlaySound(FSOUND_FREE, sound);

	// set the position
	float posv[3];
	posv[0] = pos.x;
	posv[1] = pos.y;
	posv[2] = -pos.z;
	FSOUND_3D_SetAttributes(channel, &posv[0], 0);
}

void Audio::frame(Real time)
{
	Vector3 campos = Renderer::getSingleton().getCameraThing()->getPosition();
	Vector3 f = Renderer::getSingleton().getCameraThing()->getDirection();

	listenerpos[0] = campos.x;
	listenerpos[1] = campos.y;
	listenerpos[2] = -campos.z;

	FSOUND_3D_Listener_SetAttributes(&listenerpos[0], 0, f.x, f.y, -f.z, 0, 1, 0); // update 'ears'
	FSOUND_Update(); // needed to update 3d engine, once per frame
}




Audio& Audio::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Audio();
	}
	return Singleton<Audio>::getSingleton();
}

}