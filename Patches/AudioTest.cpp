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
 *  In the src/ directory:
 * 1. copy or link this file and Patches/stub.cpp to the src directory.
 *    and make the header changes in ../include/OgrianAudio.h or -DUSE_STUB_H in 
 *    the Makefile
 * 2. build it's pre-req with
 * \code 
  	make OgrianAudio.o audioTest.o stub.o
 * \endcode
 * 3. link with
  * \code 
  	/bin/sh ../libtool --tag=CXX --mode=link g++ -DUSE_STUB_H -DOGRE_GUI_gtk \
   -DOGRE_CONFIG_LITTLE_ENDIAN -I/usr/local/include \
   -I/usr/local/include/OGRE   -I/usr/local/include/RakNet \
   -I/usr/local/include   -I/usr/local/include/fmodex/   -I/usr/include/CEGUI \
   -Wall -Wimplicit -Wunused -g -pg -g -O2 -L/usr/local/lib64 -lOgreMain \
   -L/usr/local/lib64 -lRakNet -lpthread   -lfmodex64   -lCEGUIBase  \
   -L/usr/local/lib64 -lOIS    -o test OgrianAudio.o audioTest.o stub.o
 * \endcode   
 * 4. run the test with
 * \code
   test ../Media/sounds/hum1.wav ../Media/music/Bulerias.ogg
 * \endcode
 */

/**
 * \file main.cpp
 * \author Jeremiah Powell<waveclaw@hot_nospam_mail.com
 * \brief stub to test the Ogrian Audio system. 
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
