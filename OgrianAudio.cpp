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
OgrianAudio.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: The audio handler
 *------------------------------------*/

#include "OgrianAudio.h"
#include "OgrianRenderer.h"
#include "OgrianCameraThing.h"

template<> Ogrian::Audio * Singleton< Ogrian::Audio >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

Audio::Audio()
{
	FSOUND_Init(44100, 32, 0);
	mSongStream = 0;
	mSongChannel = -1;
	mRunning = false;
	mScale = CONR("SOUND_SCALE");
	mCurrentSong = "nothing";
}

//----------------------------------------------------------------------------

Audio::~Audio()
{
	if (mSongStream != 0) FSOUND_Stream_Stop(mSongStream);
}

//----------------------------------------------------------------------------

void Audio::playSong(String filename, Real volume)
{
	if (volume == 0)
	{
		stopSong();
		return;
	}

	if (mCurrentSong != filename || mSongStream == 0)
	{
        stopSong();

		// load teh new song
		mSongStream = FSOUND_Stream_Open(filename.c_str(), FSOUND_LOOP_NORMAL, 0, 0);
		
		// error if not found
		if (mSongStream == 0) 
		{
			Except( Exception::ERR_FILE_NOT_FOUND, String("Error: Song file not found:") + filename,
					"Audio::playSong" );	
		}

		// play the new song
		mSongChannel = FSOUND_Stream_Play(FSOUND_FREE, mSongStream);

		mCurrentSong = filename;
	}

	// set the volume
	FSOUND_SetVolume(mSongChannel, volume);
}

//----------------------------------------------------------------------------

int Audio::loadSound(String filename, bool isLong, bool loop)
{
	// load the sound
	FSOUND_SAMPLE* sound = FSOUND_Sample_Load(FSOUND_FREE, filename.c_str(), 
		loop ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF,
		0, 0);

	// error if not found
	if (sound == 0) 
	{
		Except( Exception::ERR_FILE_NOT_FOUND, String("Error: Sound file not found:") + filename,
				"Audio::playSound" );	
	}

	mSamples.push_back(sound);
	mIsLong.push_back(isLong);

	return (int)mSamples.size()-1;
}

//----------------------------------------------------------------------------


bool Audio::isLong(int id)
{
	return mIsLong[id];
}

//----------------------------------------------------------------------------

int Audio::playSound(int id, Vector3 pos)
{
	if (!mRunning) return -1;

	// get the sample
	FSOUND_SAMPLE* sound = mSamples[id];

	// play the sound
	int channel = FSOUND_PlaySound(FSOUND_FREE, sound);

	// set the position
	setSoundPosition(channel, pos);

	return channel;
}

//----------------------------------------------------------------------------

void Audio::setSoundPosition(int channel, Vector3 pos)
{
	if (!mRunning) return;
	if (channel < 0) return;

	// set the position
	float posv[3];
	posv[0] = pos.x * mScale;
	posv[1] = pos.y * mScale;
	posv[2] = -pos.z * mScale;
	FSOUND_3D_SetAttributes(channel, &posv[0], 0);
}

//----------------------------------------------------------------------------

void Audio::stopSound(int channel)
{
	if (!mRunning) return;

	if (channel >= 0)
		FSOUND_StopSound(channel);
}

//----------------------------------------------------------------------------

void Audio::stopSong()
{
	// stop the current song
	if (mSongStream != 0) 
	{
		FSOUND_Stream_Stop(mSongStream);
		mSongStream = 0;
		mSongChannel = -1;
	}
}

//----------------------------------------------------------------------------

void Audio::frame(Real time)
{
	if (!mRunning) return;

	CameraThing* cam = Renderer::getSingleton().getCameraThing();

	Vector3 campos;
	Vector3 f = Vector3(0,0,1);
	if (cam != 0) 
	{
		campos = cam->getPosition();
		f = cam->getDirection();
	}

	listenerpos[0] = campos.x * mScale;
	listenerpos[1] = campos.y * mScale;
	listenerpos[2] = -campos.z * mScale;

	FSOUND_3D_Listener_SetAttributes(&listenerpos[0], 0, f.x, f.y, -f.z, 0, 1, 0); // update 'ears'

	if (mSongStream != 0)
		FSOUND_3D_SetAttributes(mSongChannel, &listenerpos[0], 0); // keep the music between the ears
	
	FSOUND_Update(); // update 3d engine
}

//----------------------------------------------------------------------------

void Audio::start()
{
	mRunning = true;
}

//----------------------------------------------------------------------------

void Audio::stop()
{
	mRunning = false;
}

//----------------------------------------------------------------------------

Audio& Audio::getSingleton(void)
{
	if (!ms_Singleton) 
	{
		ms_Singleton = new Audio();
	}
	return Singleton<Audio>::getSingleton();
}

//----------------------------------------------------------------------------

}
