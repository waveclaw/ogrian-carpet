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
 * \note Usage
 * Run the test with
 * \code
   test ../Media/sounds/hum1.wav ../Media/music/Bulerias.ogg
 * \endcode
 */

/**
 * \file AudioTest.cpp
 * \author Jeremiah Powell<waveclaw@hot_nospam_mail.com
 * \brief Unit test for the Ogrian Audio system.
 * Unit testing the Audio system can use either the existing system or
 * the stub system.  To use the stub system, define USE_STUB_H when compling. 
 */
#include "OgrianAudio.h"
#include <iostream>

using namespace std;
using namespace Ogrian;

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT unused)

#else // #ifdef WIN32

int main(int argc, char *argv[])

#endif // #ifdef WIN32
{

	Audio &testSoundPlayer = Audio::getSingleton();
	FMOD::Channel* channel;
	Vector3 position = Vector3(0.0, 0.0, 0.0);
	Real time = 1.0;
	Real volume = 5.0;
	
	if (argc != 3) {
		cout << "I want a sound followed by a song." << endl;
		return 1;
	};
	
	String soundfile = argv[1];
	String songfile = argv[2];
	int id;
	
	id = testSoundPlayer.loadSound(soundfile); // islong = false, loop = false
	if (-1 == id) {
		cout << "Unable to load Sound" << soundfile << endl;
		return 1;
	};

	testSoundPlayer.start(); // volume up
	channel = testSoundPlayer.playSound(id, position);
	cout << "Testing sound: ok" << endl;
	testSoundPlayer.stop();	//volume off

	if (testSoundPlayer.isLong(id)) {
			cout << "A short sound reported false longness" << endl;		
			return 1;
	}
	
	sleep(1);
	testSoundPlayer.start(); // volume up
	position = Vector3(0.1, 0.1, 0.1);
	testSoundPlayer.setSoundPosition(channel, position);
	channel = testSoundPlayer.playSound(id, position);
	cout << "Testing Moved sound: ok" <<  endl;

	position = Vector3(0, 0, 0);
	testSoundPlayer.setSoundPosition(channel, position);
	testSoundPlayer.stop();	//volume off
	
	testSoundPlayer.playSong(songfile, volume);
	testSoundPlayer.start(); // volume up
	cout << "Testing song:";cout.flush();
	for (int i=1;i<5;i++) {
		testSoundPlayer.frame(time); // play for a while
		sleep(1);
		cout << ".";cout.flush();
	};
	cout << "ok" << endl;
	testSoundPlayer.stop();	//volume off
	

	testSoundPlayer.stopSong();
	testSoundPlayer.start(); // volume up
	cout << "Testing stopped song:";cout.flush();
	for (int i=1;i<5;i++) {
		testSoundPlayer.frame(time); // play for a while
		sleep(1);
		cout << ".";cout.flush();
	};
	cout << "ok" << endl;
	testSoundPlayer.stop();	//volume off

	testSoundPlayer.playSong(songfile, volume);
	testSoundPlayer.start(); // volume up
	cout << "Testing song and sound:";cout.flush();
	for (int i=1;i<5;i++) {
		testSoundPlayer.frame(time); // play for a while
		sleep(1);
		if (i % 2)	channel = testSoundPlayer.playSound(id, position);
		cout << ".";cout.flush();
	};
	cout << "ok" << endl;
	testSoundPlayer.stop();	//volume off

	testSoundPlayer.stopSound(channel);	

    return 0;
}
