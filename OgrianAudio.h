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
	virtual ~Audio();

	// load a sound. returns the id number of the sound
	int loadSound(String filename, bool loop=false);

	// start playing a sound. Returns the channel of the sound.
	int playSound(int id, Vector3 pos);
	
	// set the position of a sound
	void setSoundPosition(int channel, Vector3 pos);

	// stop a sound
	void stopSound(int channel);

	// start playing a song. 
	void playSong(String filename);

	// stop playing the song
	void stopSong();

	// update the 3d audio engine
	void frame(Real time);

	// start the Audio engine
	void start();

	// stop the Audio engine
	void stop();

	static Audio& getSingleton(void);

private:
	Audio();
    float listenerpos[3]; 
	FSOUND_STREAM* mSongStream;
	int mSongChannel;
	bool mRunning; // wether or not the engine is running
	Real mScale; // a scalar applied to 3d positioning

	std::vector<FSOUND_SAMPLE*> mSamples;
};

}

#endif