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
 * Hello, World! for CppUnit based on http://pantras.free.fr/articles/helloworld.html
 * by rf.eerf@sartnap:otliam on May 12, 2005
 */
#include "../include/SpinBuffer.h"

#include <iostream>

#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
//include <cppunit/TextTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

using namespace std;
using namespace util;

static char alphabet[27] = {'a','b','c','d','e','f','g','h','i','j','k',
    'l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};


class SpinBufferTest : public CPPUNIT_NS::TestCase
{
  // uses the helper macros to register the protected test function(s)
  // techniccally, this makes TestCase a 'Test Suite' which is run
  // bellow by the call bellow to runner.run( controller )
  CPPUNIT_TEST_SUITE(SpinBufferTest);
  CPPUNIT_TEST(Creation);
  CPPUNIT_TEST(logicalEquality);
  CPPUNIT_TEST(Copy);
  CPPUNIT_TEST(Content);
  CPPUNIT_TEST(LinearCharacters);
  CPPUNIT_TEST(LinearIntegers);
  CPPUNIT_TEST(BufferedChracters);
  CPPUNIT_TEST(BufferedIntegers);
  CPPUNIT_TEST(BufferedCharPointers);
  CPPUNIT_TEST(BufferedStrings);
  // check throws(X) too
  // CPPUNIT_TEST_EXCEPTION( testAddThrow, IncompatibleMoneyError );
  CPPUNIT_TEST_SUITE_END();

  bool isInAlphabet(const char charToFind) {
	for (int i = 0;i < 26;i++) {
		if (charToFind == alphabet[i]) return true;
	};
	return false;
  }; // end isInAlphabet

public:
  void setUp(void) { ; };
  void tearDown(void) { ; } ;
  // can also override the test runner: void runTest() {}
protected:
  void Creation(void) {
    SpinBuffer<int, 500> *integerBuffer = new SpinBuffer<int, 500>();
    CPPUNIT_ASSERT(integerBuffer != NULL);
    CPPUNIT_ASSERT(integerBuffer->empty());

    SpinBuffer<char, 100> *characterBuffer = new SpinBuffer<char, 100>();
    CPPUNIT_ASSERT(characterBuffer != NULL);
    CPPUNIT_ASSERT(characterBuffer->empty());

    SpinBuffer<char *> *strBuffer = new SpinBuffer<char *>();
    CPPUNIT_ASSERT(strBuffer != NULL);
    CPPUNIT_ASSERT(strBuffer->empty());
}; // testCreation

   void logicalEquality(void) {
    SpinBuffer<char,5> *stringBuffer1 = new SpinBuffer<char,5>();
    SpinBuffer<char,5> *stringBuffer2 = new SpinBuffer<char,5>();
    CPPUNIT_ASSERT(stringBuffer1 == stringBuffer2);
    stringBuffer1->put(1);
    stringBuffer2->put(1);
    CPPUNIT_ASSERT(stringBuffer1 == stringBuffer2);
  }; // end testEquality

   void Copy(void) {
    SpinBuffer<string> *stringBuffer1 = new SpinBuffer<string>();
    SpinBuffer<string> *stringBuffer2 = stringBuffer1;
    CPPUNIT_ASSERT(stringBuffer1 == stringBuffer2);
  }; // end testEquality


  void Content(void) {
    //cout << "Testing raw output:" << endl;
    SpinBuffer<char,25> *characterBuffer = new SpinBuffer<char,25>();	
    SpinBuffer<int,32> *integerBuffer = new SpinBuffer<int,32>();	
    //string chars = characterBuffer->str();
    //CPPUNIT_ASSERT(chars == "");
    //string ints = integerBuffer->str();
    //CPPUNIT_ASSERT(ints =="");
  }; // end testContent

  void LinearCharacters(void) {
	const char testValue = '\0';
	SpinBuffer<char,25> *characterBuffer = new SpinBuffer<char,25>();	
    for (int i = 0;i < 6;i++) {
	CPPUNIT_ASSERT(characterBuffer->put(alphabet[i]));
    };
    for (int i = 6;i < 26;i++) {
	CPPUNIT_ASSERT(characterBuffer->put(alphabet[i]));
    };
    do {
    	CPPUNIT_ASSERT(isInAlphabet(characterBuffer->get()));
    	//sleep(1);
    } while (!characterBuffer->empty());    	
  }; // end testLinearCharacters

  void LinearIntegers(void) {
	int testValue = 0;
	SpinBuffer<int,25> *integerBuffer = new SpinBuffer<int,25>();	
    //Linear Puts
    for (int i = 0;i < 25;i++) {
        CPPUNIT_ASSERT(integerBuffer->put(i)); 
    };
    do {
	testValue = integerBuffer->get();
    	CPPUNIT_ASSERT(0 <= testValue && testValue <= 25);
    	//sleep(1);    	    	 
    } while (!integerBuffer->empty());
  }; // testLinearIntegers
        
  void BufferedChracters(void) {
    //cout << "Testing buffered gets and puts on characters." << endl;
    SpinBuffer<char,26> *characterBuffer = new SpinBuffer<char,26>();	
    for (int i = 0;i < 27;i++) {
    	//cout << "Put " << alphabet[i];cout.flush();
    	CPPUNIT_ASSERT(characterBuffer->put(alphabet[i]));
	CPPUNIT_ASSERT(characterBuffer->get() == alphabet[i]);			
    };    
  }; // end testBufferedChars
 
  void BufferedIntegers(void) {
    //cout << "Testing buffered gets and puts on integers." << endl;
    SpinBuffer<int,32> *integerBuffer = new SpinBuffer<int,32>();	
    for (int i = 0;i < 32;i++) {
	CPPUNIT_ASSERT(integerBuffer->put(i));
	CPPUNIT_ASSERT(integerBuffer->get() == i);	
    };  
  }; // end testBufferedIntegers
        
  void BufferedCharPointers(void) {
    //cout << "Testing buffered gets and puts on char pointers." << endl;
    SpinBuffer<char *,5> *strBuffer = new SpinBuffer<char *,5>();	
    char *charPtr = "testBufferedCharPointers";
    try {
       for (int i = 0;i < 2;i++) {
	  CPPUNIT_ASSERT(strBuffer->put(charPtr));
	  CPPUNIT_ASSERT(strBuffer->get() == charPtr);			
       };
    } catch (exception &e) {
   		e.what();
    };    
  }; // end testBufferedCharPointers

  void BufferedStrings(void) {    
    //cout << "Testing buffered gets and puts on strings." << endl;
	string String = "testBufferedStrings";
        SpinBuffer<string,5> *stringsBuffer = new SpinBuffer<string,5>();	
   	try 
   	{
   	for (int i = 0;i < 2;i++) {
	    	CPPUNIT_ASSERT(stringsBuffer->put(String));
		CPPUNIT_ASSERT(stringsBuffer->get() == String);			
    	};
   	}
   	catch (exception &e) 
   	{
   		e.what();
   	};    
  }; // end testBufferedStrings
}; // end class SpingBufferTest

/**
 * \function CPPUNIT_TEST_SUITE_REGISTRATION
 * \parameter Test the 'test' class to add the 'suite' of things to test
 * \returns Nothing Not  defined, it's a macro for a huge mess of boilerplate code
 **/
CPPUNIT_TEST_SUITE_REGISTRATION(SpinBufferTest);

/**
 * \function main
 * \parameter variable dependant on UNIX or Windows platform
 * \returns 0 if all tests pass, some error code otherwise
 **/
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
  //cout << "Create the event manager and test controller" << endl;
  CPPUNIT_NS::TestResult controller;

  //cout << "Add a listener that colllects test result" << endl;
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  //cout << "Add a listener that print dots as test run." << endl;
  CPPUNIT_NS::BriefTestProgressListener progress;
  //CPPUNIT_NS::TextTestProgressListener progress;
  controller.addListener( &progress );      

  //cout << "Add the top suite to the test runner" << endl;
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );

  runner.run( controller );

  return result.wasSuccessful() ? 0 : 1;
}
