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
 * \file OgrianAudio.cpp
 * \author Mike Prosser <mikeprosser@gmail.com>
 * \brief The audio handler
 **/
#include "OgrianAudio.h"

template<> Ogrian::Audio * Singleton< Ogrian::Audio >::ms_Singleton = 0;

namespace Ogrian
{

//----------------------------------------------------------------------------

/**
 * The private constructor.
 * Initializes the fMod audio system.   
 * @exception Warns if the sound system fails to initialize
 * @exception Warns if the sound system fails to start  
 **/
Audio::Audio()
{
	mRunning = false;
	int numberOfVoices = 100;
	int extraDriverData = 0;
	mScale = 0;//FIXME: CONR("SOUND_SCALE");
	mCurrentSong = "nothing";
	FMOD_RESULT result;
	mSongStream = (FMOD::Sound *) 0;
	mSongChannel = (FMOD::Channel*) -1;
	
	result = FMOD::System_Create(&mSoundSystem);
	if (result != FMOD_OK) 
{
   	Except( Exception::ERR_NOT_IMPLEMENTED, 
  	    String("Error pre-initalizing sound system. fMod had an error: ") + String(FMOD_ErrorString(result)),
        "Audio::Audio");
    stop();
}
	result = mSoundSystem->init(numberOfVoices, (FMOD_MODE)(FMOD_INIT_NORMAL), (void *) extraDriverData);
if (result != FMOD_OK)
{
   	Except( Exception::ERR_NOT_IMPLEMENTED, 
  	    String("Error iniitalizing sound system. fMod had an error: ") + String(FMOD_ErrorString(result)),
        "Audio::Audio");   
    stop();

}
	 
}

//----------------------------------------------------------------------------

/**
 * The destructor for cleaning up audio objects.
 * This uses the existing stream to determine 
 * if this sound should stop.
 */
Audio::~Audio()
{
	FMOD_RESULT result;
	if (mSongStream != 0) result = mSongStream->release();
    if (result != FMOD_OK)
	    {
        	Except( Exception::ERR_NOT_IMPLEMENTED, 
        	    String("Error destoying Audio object. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	        "Audio::~Audio");
    		    // stop(); <-- pointless
	    }	
	mSongStream = (FMOD::Sound *) 0;
}

//----------------------------------------------------------------------------

/**
 * Load a song and play it.
 * If the audio volume is set to mute, then stop playing any sounds.
 * If the song/sound has never been loaded, load it and set the volume.
 * Play the song once it is ready.
 * @exception Warns on files that were not found. 
 */ 
void Audio::playSong(String filename, Real volume)
{
	FMOD_CREATESOUNDEXINFO *extendedInfo = 0;
	FMOD_MODE streamMode = FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;
    FMOD_RESULT result;
    bool notPaused = false;
	
	if (volume == 0)
	{
		stopSong();
		return;
	}

	if (mCurrentSong != filename || mSongStream == 0)
	{
        stopSong();

		// load teh new song
		result = mSoundSystem->createSound(filename.c_str(), streamMode, extendedInfo, &mSongStream);

        if (result != FMOD_OK)
	    {
        	Except( Exception::ERR_NOT_IMPLEMENTED, 
        	    String("Error iniitalizing song. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	        "Audio::playSong");
    		    stop();
	    }

		// play the new song
 		result = mSoundSystem->playSound(FMOD_CHANNEL_FREE, mSongStream, notPaused, &mSongChannel);
		if (result != FMOD_OK)
		{
        	Except( Exception::ERR_NOT_IMPLEMENTED, 
        	    String("Error playing song. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	        "Audio::playSong");
    		stop();
		}
		
		mCurrentSong = filename;
		mSongChannel->setVolume(volume);
	} // end if not current song or if first stream

	// set the volume
}

//----------------------------------------------------------------------------

/**
 * Attempt to load a sound from a file.
 * @param filename Name of the file with the sound
 * @param isLong Is this a long sound, or something short?
 * @param loop Should this sound be looped continually?
 * @exception Warns on files that were not found, if cannot load song.
 * @return The number of Samples loaded at this time.
  */
int Audio::loadSound(String filename, bool isLong, bool loop)
{
	FMOD_RESULT result;
	FMOD::Sound *sound = 0;
	
	if (loop) {
		result = mSoundSystem->createSound(filename.c_str(), 
			(FMOD_MODE)(FMOD_SOFTWARE | FMOD_3D | FMOD_LOOP_NORMAL), 0, &sound);
	} else
	{
		result = mSoundSystem->createSound(filename.c_str(), 
			(FMOD_MODE)(FMOD_SOFTWARE | FMOD_3D | FMOD_LOOP_OFF), 0, &sound);
	}
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error loading song. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::loadSound");
    		stop();
	}

	// error if not found
	if (sound == 0 | result == FMOD_ERR_FILE_NOTFOUND) 
	{
		Except( Exception::ERR_FILE_NOT_FOUND, String("Error: Sound file not found:") + filename,
				"Audio::loadSound" );	
	}

	mSamples.push_back(sound);
	mIsLong.push_back(isLong);

	return (int)mSamples.size()-1;
}

//----------------------------------------------------------------------------

/**
 * Answer the question: this this sound long?
 * @param id The ID of the sound.
 * @return The long or not status of the sound identified by ID.
 */
bool Audio::isLong(int id)
{
	return mIsLong[id];
}

//----------------------------------------------------------------------------

/**
 * Actually play a sound.
 * @param id The ID of the sound you got from loadSound.
 * @param pos Where in the 3D soundspace should the sound be positioned.
 * @exception Warns if there were errors in setup, in locatin or in playing
 * @return The channel the sound will play on.
 */
FMOD::Channel *Audio::playSound(int id, Vector3 pos)
{
	if (!mRunning) return (FMOD::Channel *) -1;
	bool notPaused = false;
	bool paused = true;
    FMOD::Channel *soundChannel = 0;
    FMOD_RESULT result;	
    FMOD_VECTOR position = {pos.x, pos.y, pos.z};
    FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f};

	// get the sample
	FMOD::Sound *sound = mSamples[id];

	// get ready to play the sound
	result = mSoundSystem->playSound(FMOD_CHANNEL_FREE, sound, paused, &soundChannel);
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error loading sound. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::playSound");
    		stop();
	}
		
	// set the position while this particular sound is muted
    result = soundChannel->set3DAttributes(&position, &velocity);
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error setting location of the sound. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::playSound");
    		stop();
	}
	
    // actually play the sound
    result = soundChannel->setPaused(notPaused);	
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error actually playing the sound. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::playSound");
    		stop();
	}
	return soundChannel;
}

//----------------------------------------------------------------------------

/**
 * Move the location of a sound.
 * @param id The existing sound which has been played.
 * @param pos The new location for the sound.
 */
void Audio::setSoundPosition(FMOD::Channel *channel, Vector3 pos)
{
	FMOD_RESULT result;
    FMOD_VECTOR position = {pos.x * mScale, pos.y * mScale, -pos.z * mScale};
    FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f};	

	if (!mRunning) return;
	if (channel < 0) return;

	// set the position while this particular sound 
    result = channel->set3DAttributes(&position, &velocity);
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error setting location of the sound. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::setSoundPosition");
    		stop();
	}
		
}

//----------------------------------------------------------------------------

/**
 * Stop playing a sound.
 * @param The existing channel for a sound which has been played.
 */
void Audio::stopSound(FMOD::Channel *channel)
{
	FMOD_RESULT result;
	if (!mRunning) return;

	if (channel >= 0) {
		result = channel->stop(); // this will free the channel, BTW
		if (result != FMOD_OK)
	    {
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error stopping a sound. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::stopSound");
    		// stop(); <-- pointless, really.
	    }
	    channel = 0;
	}
		
}

//----------------------------------------------------------------------------

/**
 * Stop a long or looping sound such as a song.
 * Stops the current background music by stoping the stream on which it plays.
 */
void Audio::stopSong()
{
	FMOD_RESULT result;
	// stop the current song
	if (mSongStream != 0) 
	{
		// can't actually stop a streaming song, but you can stop it's channel.
		result = mSongChannel->stop();
		if (result != FMOD_OK)
	    {
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error stopping the song. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::stopSong");
    		// stop(); <-- pointless, really.
	    }		
		result = mSongStream->release();
		if (result != FMOD_OK)
	    {
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error cleaning up the song. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::stopSong");
    		// stop(); <-- pointless, really.
	    }		
		mSongStream = 0;
		mSongChannel = (FMOD::Channel*) -1;
	}
}

//----------------------------------------------------------------------------

/**
 * Update the sound information per drawn frame.
 * While this ties the sound system to the number of frames that can be 
 * drawn in a given time, it lets the sound system lock in the 
 * current player's position.
 * @param time What time is it?
 */
void Audio::frame(Real time)
{
	if (!mRunning) return;

	CameraThing *camera = Renderer::getSingleton().getCameraThing();
	int listener = 0; //hey, that's you!
	FMOD_RESULT result;
	
	Vector3 cameraInfo;
	FMOD_VECTOR forward;
	FMOD_VECTOR listenerPosition;
	FMOD_VECTOR velocity;
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
 
	if (camera != 0) 
	{

		cameraInfo = camera->getDirection();
		forward.x = cameraInfo.x;
		forward.y = cameraInfo.y;
		forward.z = -cameraInfo.z;
		cameraInfo = camera->getVelocity();
		velocity.x = cameraInfo.x;
		velocity.y = cameraInfo.y;
		velocity.z = cameraInfo.z;
		cameraInfo = camera->getPosition();
		listenerPosition.x = cameraInfo.x * mScale; 
		listenerPosition.y = cameraInfo.y * mScale;
		listenerPosition.z = -(cameraInfo.z * mScale);
	}

	result = mSoundSystem->set3DListenerAttributes(listener, 
		&listenerPosition, &velocity, &forward, &up);
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error setting location each frame. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::frame");
    		stop();
	}	

	if (mSongStream != 0) {
		cameraInfo[0] = listenerPosition.x;
		cameraInfo[1] = listenerPosition.y;
		cameraInfo[2] = listenerPosition.z;
		setSoundPosition(&mSongChannel[0], cameraInfo); // keep the music between the ears
	}
	
	result = mSoundSystem->update(); // new for fMod 4, must call once per frame.
	if (result != FMOD_OK)
	{
    	Except( Exception::ERR_NOT_IMPLEMENTED, 
    	    String("Error updating each frame. fMod had an error: ") + String(FMOD_ErrorString(result)),
    	    "Audio::frame");
    		stop(); // don't know about you, but a billion dupe errors on the console is shitty.
	}	
    
}

//----------------------------------------------------------------------------

/**
 * Set the audio system to play.
 */
void Audio::start()
{
	mRunning = true;
}

//----------------------------------------------------------------------------

/**
 * Set the audo system to more than mute.
 */
void Audio::stop()
{
	mRunning = false;
}

//----------------------------------------------------------------------------

/**
 * Implement Singlton.
 * Per the singleton pattern, create an audio object if does not exist or return
 * the current one.
 * @return Address of the current audio object in memory.
 */
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
