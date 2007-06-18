/**
 * This is the unit test for SpinBuffers.
 */

/**
 * \file SpinBufferTest.cpp
 * \author Jeremiah Powell<waveclaw@hot_nospam_mail.com
 * \brief Unit test for an implementation of SpinBuffers
 * Based on work by Prashanth Hirematada, 
 * published in Dr. Dobbs Journal, July 2007 by CMP Media. 
 */
#include "SpinBuffer.cpp"
#include <iostream>

using namespace std;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include <stdio.h>
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT unused)
#else // #ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[], char **envp)
#endif
{
	SpinBuffer<int> *integerBuffer1 = new SpinBuffer<int>();
	
	SpinBuffer<char> *characterBuffer1 = new SpinBuffer<char>();
	SpinBuffer<char *> *strBuffer1 = new SpinBuffer<char *>();
	cout << "Pre-allocation test....okay." << endl;

	SpinBuffer<int> *integerBuffer2;
	SpinBuffer<char> *characterBuffer2;
	SpinBuffer<char *> *strBuffer2;
	
	cout << "Testing post-allocation."; cout.flush();
	integerBuffer2 = new SpinBuffer<int>();
	cout <<".";cout.flush();
	characterBuffer2 = new SpinBuffer<char>();
	cout <<".";cout.flush();
	strBuffer2 = new SpinBuffer<char *>();
	cout <<".okay" << endl;
	
	cout << "Testing class allocation..."; cout.flush();
	SpinBuffer<string> *stringsBuffer1 = new SpinBuffer<string>();
	cout << ".okay" << endl;
			
    //Linear Puts
	cout << "Testing linear puts on integers."; cout.flush();    
    for (int i = 0;i < 32;i++) {
    	integerBuffer1->put(i);
    	cout <<".";cout.flush();
    };
    cout << ".okay" << endl;    
    	
	cout << "Testing linear puts on characters."; cout.flush();    	
    char alphabet[27] = {'a','b','c','d','e','f','g','h','i','j','k',
    'l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    for (int i = 0;i < 27;i++) {
    	characterBuffer1->put(alphabet[i]);
		cout <<".";cout.flush();
    };    	
    cout << ".okay" << endl;
    
    //Linear Gets
 	cout << "Testing linear gets on characters."; cout.flush();
    while (!characterBuffer1->empty()) {
    	cout << characterBuffer1->get() << " ";cout.flush();
    };    	
    cout << ".okay" << endl;

	cout << "Testing linear gets on integers."; cout.flush();
	int crap;    
    while (!integerBuffer1->empty()) {
    	crap = integerBuffer1->get();
    	cout << " " << &crap;cout.flush();    	 
    };
        cout << ".okay" << endl;
 
    
    return 0;
} // end main
