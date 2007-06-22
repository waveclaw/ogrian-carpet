/*****************************************************************************
	Copyright 2007 Jeremiah Powell
		
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

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
#include "SpinBuffer.h"
#include <iostream>

using namespace std;
using namespace util;

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

	SpinBuffer<int,500> *integerBuffer1 = new SpinBuffer<int,500>();
	
	SpinBuffer<char, 100> *characterBuffer1 = new SpinBuffer<char, 100>();
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
	cout << "Testing linear puts on integers." << endl;
    for (int i = 0;i < 25;i++)
        integerBuffer1->put(i); 
    	cout <<".";cout.flush();
    cout << ".okay" << endl;   	  
    	
	cout << "Testing linear puts on characters." << endl;    	
    char alphabet[27] = {'a','b','c','d','e','f','g','h','i','j','k',
    'l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

    for (int i = 0;i < 6 && characterBuffer1->put(alphabet[i]);i++) 
			cout <<".";cout.flush();
   	//(void) characterBuffer1->get();
   	for (int i = 6;i < 26 && characterBuffer1->put(alphabet[i]);i++) 
			cout <<".";cout.flush();			

    cout << ".okay" << endl;
    
	// content test
    cout << "Testing raw output:" << endl;
    cout << characterBuffer1->str() << endl;
    cout << integerBuffer1->str() << endl;
    
    //Linear Gets
 	cout << "Testing linear gets on characters."; cout.flush();
    do
    {
    	cout << characterBuffer1->get() << " ";cout.flush();
    	//sleep(1);
    } while (!characterBuffer1->empty());    	
    cout << ".okay" << endl;

	cout << "Testing linear gets on integers."; cout.flush();
	do    
    {
    	cout << " " << integerBuffer1->get();cout.flush();
    	//sleep(1);    	    	 
    } while (!integerBuffer1->empty());
    cout << ".okay" << endl;
        
    //Buffered puts and gets
    cout << "Testing buffered gets and puts on characters." << endl;
   	for (int i = 0;i < 27;i++) 
	    {
	    	cout << "Put " << alphabet[i];cout.flush();
	    	(void) characterBuffer2->put(alphabet[i]);
			cout << " and got " << characterBuffer2->get() << endl;			
    	};
    cout << ".okay" << endl;   	
 
    cout << "Testing buffered gets and puts on integers." << endl;
    for (int i = 0;i < 32;i++) 
	    {
	    	cout << "Put " << i;cout.flush();
	    	(void) integerBuffer2->put(i);
			cout << " and got " << integerBuffer2->get() << endl;	
			
    	};  
    cout << ".okay" << endl;
        
cout << "Testing buffered gets and puts on char pointers." << endl;
	char *strStuff = "nasty!";
   	try 
   	{
   	for (int i = 0;i < 2;i++) 
	    {
	    	cout << "Put " << strStuff;cout.flush();
	    	(void) strBuffer1->put(strStuff);
			cout << " and got " << strBuffer1->get() << endl;			
    	};
   	}
   	catch (exception &e) 
   	{
   		e.what();
   	};    
    cout << ".okay" << endl;
    
cout << "Testing buffered gets and puts on strings." << endl;
	string struff = "funky!";
   	try 
   	{
   	for (int i = 0;i < 2;i++) 
	    {
	    	cout << "Put " << struff;cout.flush();
	    	(void) stringsBuffer1->put(struff);
			cout << " and got " << stringsBuffer1->get() << endl;			
    	};
   	}
   	catch (exception &e) 
   	{
   		e.what();
   	};    
    cout << ".okay" << endl;
        	          
    return 0;
} // end main
