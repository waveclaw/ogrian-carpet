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
 * \file SpinBuffer.cpp
 * \author Jeremiah D. Powell <waveclaw@hot_nospam_mail.com>
 * \brief implements SpinBuffers (Prashanth Hirematada, DDJ July 2007)
 **/
#ifndef __SPINBUFFER_CPP__
#define __SPINBUFFER_CPP__

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

/* \class noSuchBuffer SpinBuffer.hpp "include/SpinBuffer.hpp"
 * \brief implements an exception for noSuchBuffer
 * This is an exception for SpinBuffer.
 **/
class noSuchBufferException: public exception
{
	public:
  	/**
  	 * Mandatory throw function
  	 **/ 
  	virtual const char* what() const throw()
  	{
	    return "Buffer Position requested outside limit.";
  	}
} noSuchBuffer;

/* \class emptyBufferException SpinBuffer.hpp "include/SpinBuffer.hpp"
 * \brief implements an exception for emptyBuffers
 * This is an exception for SpinBuffer.
 **/
class emptyBufferException: public exception
{
	public:
  	/**
  	 * Mandatory throw function
  	 **/ 
  	virtual const char* what() const throw()
  	{
	    return "All buffers are empty.";
  	}
} emptyBuffer;

/* \class outOfBuffersException SpinBuffer.hpp "include/SpinBuffer.hpp"
 * \brief implements an exception for running out of buffers
 * This is an exception for SpinBuffer.
 **/
class outOfBuffersException: public exception
{
	public:	
  	/**
  	 * Mandatory throw function
  	 **/ 
  	virtual const char* what() const throw()
  	{
	    return "Cannot add anything else, all buffers are used.";
  	}
} outOfBuffers;

/* \class SpinBuffer SpinBuffer.hpp "include/SpinBuffer.hpp"
 * \brief implements a SpinBuffer
 * This is a template specialization of the SpinBuffer, so implementation
 * and definition have to occur in this file, hence the .cpp.
 * You cannot belive the amount of duplication this causes.
 */
template <class _ST, int _BS=NICE_BUFFER_SIZE> class SpinBuffer {
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
		SpinBuffer(SpinBuffer<_ST, _BS> &);	

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
		SpinBuffer& operator=(SpinBuffer<_ST, _BS> &);		

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
		string ToString(void);		

		/**
		 * Write to the SpinBuffer
		 **/		
		bool put(_ST) throw(outOfBuffersException);

		/**
		 * Read, destructively, a SpinBuffer
		 * @exception Throws an exception if the buffer is empty.
		 **/		
		_ST get(void) throw(emptyBufferException);
}; // end class SpinBuffer

//-----------------------------------------------------------------------------

/**
 * Create a SpinBuffer
 **/
template<class _ST, int _BS>
SpinBuffer<_ST,_BS>::SpinBuffer() 
{
    mWritePosition = 0;
    mReadPosition = 0;  
    for ( int i=0; i<SPIN_BUFFER_SIZE; i++ ) 
    {
    	mFree[i] = true;
        mBufferUpperBound[i] = mBufferLowerBound[i] = 0;
        mArrayBuffer[i] = new _ST[_BS];
    }
    mPrimed = mFree[0] = false;
} // end new SpinBuffer

/**
 * Destroy a SpinBuffer
 **/	 	
 template<class _ST, int _BS>
SpinBuffer<_ST,_BS>::~SpinBuffer(void)
{
	delete [] mFree, mBufferLowerBound, mBufferUpperBound;
    for ( int i=0; i<SPIN_BUFFER_SIZE; i++ ) 
    {
        delete [] mArrayBuffer[i];
    };	
    delete [] mArrayBuffer;
} // end destroy SpinBuffer

/**
 * Concat two SpinBuffers 
 **/	 	
template<class _ST, int _BS>
SpinBuffer<_ST, _BS>& SpinBuffer<_ST,_BS>::operator+(SpinBuffer<_ST, _BS> &right)
{
	return *this;
} // end concat two SpinBuffers			

/**
 * Copy a SpinBuffer onto another SpinBuffer 
 **/	 	
template<class _ST, int _BS>
SpinBuffer<_ST, _BS>& SpinBuffer<_ST,_BS>::operator=(SpinBuffer<_ST, _BS> &right)
{
	return *right;
} // end assign one SpinBuffer to another	

/**
 * Add an item to the buffer
 * The SpinBuffer algorithm for puts is:
 * 	1.	Put the Item into the write buffer
 *  2.	If the next buffer is free, make the free buffer the write buffer
 *  	 and make the old write buffer the free buffer
 **/
template<class _ST, int _BS>
bool SpinBuffer<_ST,_BS>::put(_ST element) throw(outOfBuffersException)
{
#ifdef DEBUG
	cout << endl;
	cout <<	"Writing to " << mWritePosition << "," << mBufferUpperBound[mWritePosition] << endl;
#endif
 	if (mPrimed) 
 	{
 		if (full()) 
 		{
#ifdef DEBUG
	cout << endl;
	cout <<	"Writing to full buffers" << endl;
#endif	 			
 			// throw(outOfBuffers);
 			return false;
 		};
		mArrayBuffer[mWritePosition][mBufferUpperBound[mWritePosition]] = element;
 		int next = (mWritePosition + 1) % SPIN_BUFFER_SIZE;
 		if (mFree[next]) 
 		{
 			// move to next buffer
 			mFree[mWritePosition] = true;
 			mFree[next] = false;
 			mWritePosition = next; 			 
 		} else {
 			// stick it in the next part of this buffer
 			mBufferUpperBound[mWritePosition]++;
 		}; 				
 	} else {
 		// need to drop off an item for get() while we move along
 		mPrimed = true;
 		mArrayBuffer[mWritePosition][mBufferUpperBound[mWritePosition]] = element;
 		//mBufferUpperBound[mWritePosition]++; // means we leave behind upper bounds == _BS
 		mWritePosition++;
 	};
    return true;
} // end put

/**
 * Get an item out of the buffer
 * The SpinBuffer algorithm is
 *  1.	Read the item from the read buffer
 *  2.	If the urrent read buffer is empty and the next buffer is free,
 * 			make the next bufer the read buffer and the old read buffer
 * 			becomes the free buffer.
 **/
template<class _ST, int _BS>
_ST SpinBuffer<_ST,_BS>::get(void) throw(emptyBufferException)
{
#ifdef DEBUG
	cout << endl;
	cout <<	"Reading from " << mReadPosition << "," << mBufferLowerBound[mReadPosition] << 
		" of " << mBufferUpperBound[mReadPosition] << " total." << endl;
#endif	
    _ST element = (_ST) NULL;
    int next = (mReadPosition + 1) % SPIN_BUFFER_SIZE;    
    if (mBufferLowerBound[mReadPosition] <= mBufferUpperBound[mReadPosition]) 
    {
    	// ooh! elements to read
    	element = mArrayBuffer[mReadPosition][mBufferLowerBound[mReadPosition]];
    	mBufferLowerBound[mReadPosition]++;
    } else {
       // this buffer is empty, check the next
    	mBufferLowerBound[mReadPosition] = mBufferUpperBound[mReadPosition] = 0;       
    	//if (mFree[next]) 
    	mFree[mReadPosition] = true;
    	mFree[next] = false;
    	mReadPosition = next; // we've moved forward, possible into the write buffer
    	mBufferLowerBound[mReadPosition] = 0;
   		if (empty())
   		{
   		    	// else return NULL, we're empty
#ifdef DEBUG
	cout << endl;
	cout <<	"Reading from empty buffers" << endl;
#endif	
   		 	// throw(emptyBuffers);
   		   	mPrimed = false;
   		} else {
	   	    // more itemz
	    	element = mArrayBuffer[mReadPosition][mBufferLowerBound[mReadPosition]];
	    	mBufferLowerBound[mReadPosition]++;
   		 } // end if all buffers empty
    } // end if this array is empty
    return element;
} // end get

/**
 * Is this buffer full?
 **/
template<class _ST, int _BS>
bool SpinBuffer<_ST, _BS>::empty(void) {
	return (!mPrimed || (mReadPosition == mWritePosition && 
		mBufferLowerBound[mReadPosition] == mBufferUpperBound[mWritePosition]));
} // end empty

/**
 * Is this buffer full?
 **/
template<class _ST, int _BS>
bool SpinBuffer<_ST, _BS>::full(void) {
    int next = (mWritePosition+1)%SPIN_BUFFER_SIZE;	
	return (!mFree[next] && _BS == mBufferUpperBound[mWritePosition]);
} // end full


} // end namespace util

#endif
