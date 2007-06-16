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

/**
 * \file OgrianAudio.h
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief The audio handler
 */

#ifndef __OgrianAudio_H__
#define __OgrianAudio_H__

#include <Ogre.h>
#include <fmod.hpp>
#include <fmod_errors.h>
//this file is used to bypass the Ogrian*.h files, use this before testing 
#ifdef USE_STUB_H 
#include "../Patches/stub.hpp"
#else
#include "OgrianRenderer.h"
#include "OgrianCameraThing.h"
#endif

using namespace Ogre;

namespace Ogrian
{
/*! \class Audio OgrianAudio.h "include/OgrianAudio.h"
 * \brief The audio engine for Ogrian Capret
 */
class Audio : public Singleton< Audio >
{
public:
	/**
 	* The destructor for cleaning up audio objects.
 	* This uses the existing stream to determine 
 	* if this sound should stop.
 	*/
	virtual ~Audio();

	/**
	 * Attempt to load a sound from a file.
	 * @param filename Name of the file with the sound
	 * @param isLong Is this a long sound, or something short?
	 * @param loop Should this sound be looped continually?
	 * @exception Warns on files that were not found, if cannot load song.
	 * @return The number of Samples loaded at this time.
	 */
	int loadSound(String filename, bool isLong=false, bool loop=false);

	/**
	 * Actually play a sound.
	 * @param id The ID of the sound you got from loadSound.
	 * @param pos Where in the 3D soundspace should the sound be positioned.
	 * @exception Warns if there were errors in setup, in locatin or in playing
	 * @return The channel the sound will play on.
	 */
	FMOD::Channel *playSound(int id, Vector3 pos);

	/**
	 * Answer the question: this this sound long?
	 * @param id The ID of the sound.
	 * @return The long or not status of the sound identified by ID.
	 */
	bool isLong(int id);
	
	/**
	 * Move the location of a sound.
	 * @param id The existing sound which has been played.
	 * @param pos The new location for the sound.
	 */
	void setSoundPosition(FMOD::Channel *channel, Vector3 pos);

	/**
	 * Stop playing a sound.
	 * @param The existing channel for a sound which has been played.
	 */
	void stopSound(FMOD::Channel *channel);

	/**
	 * Load a song and play it.
	 * If the audio volume is set to mute, then stop playing any sounds.
	 * If the song/sound has never been loaded, load it and set the volume.
	 * Play the song once it is ready.
	 * @exception Warns on files that were not found. 
	 */  
	void playSong(String filename, Real volume);

	/**
	 * Stop a long or looping sound such as a song.
	 * Stops the current background music by stoping the stream on which it plays.
	 */
	void stopSong();

	/**
	 * Update the sound information per drawn frame.
	 * While this ties the sound system to the number of frames that can be 
	 * drawn in a given time, it lets the sound system lock in the 
	 * current player's position.
	 * @param time What time is it?
	 */
	void frame(Real time);

	/**
	 * Set the audio system to play.
	 */
	void start();

	/**
	 * Set the audo system to more than mute.
	 */
	void stop();
	
	/**
	 * Implement Singlton.
	 * Per the singleton pattern, create an audio object if does not exist or return
	 * the current one.
	 * @return Address of the current audio object in memory.
	 */
	static Audio& getSingleton(void);

private:
	/**
	 * The private constructor.
	 * Initializes the fMod audio system.   
	 * @exception Warns if the sound system fails to initialize
	 * @exception Warns if the sound system fails to start  
	 */
	Audio();
	/**
	 * Where are you?
	 */
    float listenerpos[3];
    /**
     * The stream in which the current background song plays.
     * As there is only 1 song to play, this is simply a sond with FMOD_STREAM set.
     */
	FMOD::Sound *mSongStream;
	/**
	 * The sound system itself.
	 */
	FMOD::System *mSoundSystem;
	/**
	 * The default channel for the default song.
	 * Channels can be used to group sounds. 
	 * We only play 1 song for background music at a time, so only need 1 channel.
	 */
	FMOD::Channel *mSongChannel;
	/**
	 * Should the sound engine be running?
	 */
	bool mRunning; 
	/**
	 *  A scalar applied to 3d positioning
	 */
	Real mScale;
	/**
	 * The collection of sounds currently loaded.
	 * A vector of the loaded sounds so the engine can run them.
	 */
	std::vector<FMOD::Sound *> mSamples;
	/**
	 * Is this a long song?
	 */
	std::vector<bool> mIsLong;
	/**
	 * Title of the current Song.
	 * Used to short-cut loading of duplicate songs.
	 */
	String mCurrentSong;
};

}

#endif
