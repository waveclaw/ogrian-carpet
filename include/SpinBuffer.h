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
 * \file SpinBuffer.h
 * \author Jeremiah D. Powell <waveclaw@hot_nospam_mail.com>
 * \brief implements SpinBuffers (Prashanth Hirematada, DDJ July 2007)
 * Becuase this implements a generic class (e.g. a templated class) it must
 * include ALL the parts in the header.  The .cpp should not be compiled, BTW.
 **/
#ifndef __SPINBUFFER_H__
#define __SPINBUFFER_H__

#include <exception>
#include <string>
#include <sstream>
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

#define SPIN_BUFFER_SIZE 3
#define NICE_BUFFER_SIZE 5

namespace util {

/* \class SpinBuffer SpinBuffer.h "include/SpinBuffer.h"
 * \brief implements a SpinBuffer
 * This is a template specialization of the SpinBuffer, so implementation
 * and definition have to occur in this file, hence the .cpp.
 * You cannot belive the amount of duplication this causes.
 */
template <class _ST, int _BS=NICE_BUFFER_SIZE> class SpinBuffer 
{
    	_ST*   mArrayBuffer[SPIN_BUFFER_SIZE];
    	bool  mFree[SPIN_BUFFER_SIZE];
    	int   mBufferLowerBound[SPIN_BUFFER_SIZE];
    	int   mBufferUpperBound[SPIN_BUFFER_SIZE];
    	int   mWritePosition;
    	int   mReadPosition;
    	bool  mPrimed;
    public:
	/**
	 * Create a SpinBuffer
	 **/
 	SpinBuffer();

	/**
	 * Copy a SpinBuffer (they will go out of sync immediately)
	 **/	 	
	SpinBuffer(const SpinBuffer<_ST, _BS> &);	

	/**
	 * Destroy a SpinBuffer
	 **/	 	
	~SpinBuffer(void);

	/**
	 * Concat two SpinBuffers 
	 **/	 	
	SpinBuffer& operator+(SpinBuffer<_ST, _BS> &);			

	/**
	 * Copy a SpinBuffer onto another SpinBuffer 
	 **/	 	
	SpinBuffer& operator=(const SpinBuffer<_ST, _BS> &);		

	/**
	 * Compare this SpinBuffer and another SpinBuffer 
	 * this is a deep, logical and expensive comparison
 	 **/	 	
	bool operator==(const SpinBuffer<_ST, _BS> &);

	/**
	 * Is this buffer empty?
	 **/	 	
	bool empty(void);
			
	/**
	 * Is this buffer full?
	 **/	 	
	bool full(void);
	
	/**
	 * Get a string image of this SpinBuffer
	 **/	 	
	string str(void);		

	/**
	 * Write to the SpinBuffer
	 **/		
	bool put(_ST);

	/**
	 * Read, destructively, a SpinBuffer
	 * @exception Throws an exception if the buffer is empty.
	 **/		
	_ST get(void);
}; // end class SpinBuffer

} // end namespace util

// pull in the functions which won't linke w/o keyword 'export' support
#include "../src/SpinBuffer.cpp"
#endif
