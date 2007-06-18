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
    along with Ogrian Carpet; if not, write to the Free Software
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
#include <vector>

using namespace std;

#define SPIN_BUFFER_SIZE 3
#define SPIN_BUFFER_ARRAY_SIZE 5

/* \class noSuchBuffer SpinBuffer.hpp "include/SpinBuffer.hpp"
 * \brief implements an exception for noSuchBuffer
 * This is an exception for SpinBuffer.
 **/
class noSuchBufferException: public exception
{
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
 * and definition have to occur in this file, hence the .hpp.
 * You cannot belive the amount of duplication this causes.
 */
template <class T> class SpinBuffer {
	public:
		/**
		 * Create a SpinBuffer
		 **/
	 	SpinBuffer();
		/**
		 * Copy a SpinBuffer (they will go out of sync immediately)
		 **/	 	
		SpinBuffer(SpinBuffer<T> &);	
		/**
		 * Destroy a SpinBuffer
		 **/	 	
		~SpinBuffer(void);
		/**
 		 * Concat two SpinBuffers 
		 **/	 	
		SpinBuffer& operator+(SpinBuffer<T> &);			
		/**
 		 * Copy a SpinBuffer onto another SpinBuffer 
		 **/	 	
		SpinBuffer& operator=(SpinBuffer<T> &);		
		/**
		 * Get a string image of this SpinBuffer
		 **/	 	
		string Tostring(void);
		/**
		 * Is this buffer empty?
		 **/	 	
		bool empty(void);		
		/**
		 * Write to the SpinBuffer
		 **/		
		void put(T) throw(outOfBuffersException);
		/**
		 * Read, destructively, a SpinBuffer
		 * @exception Throws an exception if the buffer is empty.
		 **/		
		T get(void) throw(emptyBufferException);
		/**
		 * Examine a SpinBuffer without destroying 
		 * @exception Throws an exception if the buffer is empty.
		 **/
		T peek(void) throw(emptyBufferException);
	protected:
		/**
		 * Internal only use.  Reorder the buffers by inserting this item into buffer int.
		 * @exception Throws an expception if the indicated buffer is outside the valid range for this spinBuffer. 
		 **/	
		void poke(T, int) throw(noSuchBufferException);
	private:
		/**
		 *\note
		 * The 3 buffers:
		 * 	a. The buffer of 0 is 1
		 *  b. The buffer of 1 is 2
		 *  c. The buffer of 2 is 0 
		 * Hence 'Spin' buffered.
		 **/
		T* mOrderedBuffer[SPIN_BUFFER_SIZE];
		int mReadPtr;
		int mWritePtr;
		int mReadArrayIndex;
		int mWriteArrayIndex;
}; // end PacketSpinBuffer

//-------------------------------------------------------------------

/**
 * Create a SpinBuffer
 **/
template<class T>
SpinBuffer<T>::SpinBuffer() {
	mReadPtr = mWritePtr = 0;
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++)
		mOrderedBuffer[i] = new T[SPIN_BUFFER_ARRAY_SIZE]; // yes, this is silly
	// put(NULL); <-- prime the pump
	mWritePtr = (mWritePtr + 1) % SPIN_BUFFER_SIZE; // <-- prime the pump
}; // end constructor

/**
 * Concat two SpinBuffers 
 **/	 	
template<class T> 
SpinBuffer<T>& SpinBuffer<T>::operator+(SpinBuffer<T>& right) 
{
	try {
		//get everything in the read buffer, in reading order
		for (int i = right.mReadPtr; i < SPIN_BUFFER_ARRAY_SIZE;i++) {
			if (right.mOrderedBuffer[right.mReadPtr][i] != NULL) {
				put(right.mOrderedBuffer[right.mReadPtr][i]);
			}; // if item not null, copy it
		}; // for each possible item
		// get everything that was written, but not read
		for (int i = right.mWritePtr; i >= 0;i--) {
			if (right.mOrderedBuffer[right.mWritePtr][i] != NULL) {
				put(right.mOrderedBuffer[right.mWritePtr][i]);
			}; // if item not null, copy it
		}; // for each possible item		  
		 // nobody cares about the free buffers!!!		 
	}
	catch (exception &e) {
		// cout << e.what() << endl;
	};
	return *this;	
}; // end operator+

/**
 * Copy a SpinBuffer onto another SpinBuffer 
 **/	 	
template<class T> 
SpinBuffer<T>& SpinBuffer<T>::operator=(SpinBuffer &right) 
{
	mWritePtr = right.mWritePtr;
	mReadPtr = right.mReadPtr;
	mWriteArrayIndex = right.mWriteArrayIndex;
	mReadArrayIndex = right.mReadArrayIndex;
	try {	
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++) 
		mOrderedBuffer[i] = right.mOrderedBuffer[i];
	}
	catch (exception &e) {
		// cout << e.what() << endl;
	};		
	return *this; 
}; // end operator=		
	

/**
 * Get a string image of this SpinBuffer
 **/
template<class T>	 	
string SpinBuffer<T>::Tostring(void) {
	string description = string("SpinBuffer: reading at ") + string(mReadPtr) + 
	 string(", writing to ") + string(mWritePtr) + string(", with buffers of: ");
	 for (int i = 0;i < SPIN_BUFFER_SIZE;i++) {
	 	description += 	string(" ") + string(mOrderedBuffer[i].size()) + 
	 	string("/") + string(mOrderedBuffer[i].capacity());
	 };
	 return description; 
}; // end ToString		

/**
 * Destroy a SpinBuffer
 **/
template<class T>
SpinBuffer<T>::~SpinBuffer(void) {
	mReadPtr = mWritePtr = 0;
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++) {
		mOrderedBuffer[i].clear();
	};	
	delete [] mOrderedBuffer;
}; // end delete

/**
 * Is this buffer empty?
 **/	 	
template<class T>
bool SpinBuffer<T>::empty(void) {
	return (0 == mReadArrayIndex &&
	  NULL == mOrderedBuffer[mReadPtr][mReadArrayIndex]);
} // end empty		
	
/**
 * Write to the SpinBuffer
 **/		
template<class T>
void SpinBuffer<T>::put(T element) throw(outOfBuffersException) 
{
	// a consuquence of using fix-sized arrays
   	if (SPIN_BUFFER_ARRAY_SIZE == (mWriteArrayIndex + 1) &&
   	 mReadPtr == (mWritePtr + 1)) throw(outOfBuffers);
   /**
    * 1. Put the item into the write buffer.
    * 2. If the next buffer is free, 
    * 	make the free buffer become the write buffer, 
    * 	and the current write buffer becomes free.
    **/
    int i = (mWritePtr + 1) % SPIN_BUFFER_SIZE;
   	mOrderedBuffer[mWritePtr][mWriteArrayIndex] = element;
   	// march along the array
   	int mWriteArrayIndex = (mWriteArrayIndex + 1) % SPIN_BUFFER_ARRAY_SIZE;
   	if (mReadPtr != i) mWritePtr = i;
}; // end put

/**
 * Read, destructively, a SpinBuffer
 **/		
template<class T>
T SpinBuffer<T>::get(void) throw(emptyBufferException) 
{
	if ( (mReadPtr == mWritePtr)) //|| mOrderedBuffer[mReadPtr].empty() ) 
	 throw(emptyBuffer);
	try {
	/**
	 *  1. Read the item from the read buffer.
	 *  2. If the current read buffer is empty 
	 * 		and the next buffer is free, 
	 * 		make the next buffer the read buffer, 
	 * 		and the current read buffer becomes free.
	 **/
		T element = mOrderedBuffer[mReadPtr][mReadArrayIndex];
		mOrderedBuffer[mReadPtr][mReadArrayIndex] = NULL;
		// march along the array
		mReadArrayIndex = (mReadArrayIndex + 1) % SPIN_BUFFER_ARRAY_SIZE; 
		int i = (mReadPtr + 1) % SPIN_BUFFER_SIZE;		
		if (empty() && mWritePtr != i) mReadPtr = i;
		return element;			
	 }
    catch (exception& e)
	{
		//cout << e.what() << endl;
	}
}; // end get
/**
 * Examine a SpinBuffer without destroying 
 **/
template<class T>
T SpinBuffer<T>::peek(void) throw(emptyBufferException) 
{
	if (0 == mReadArrayIndex &&
	  NULL == mOrderedBuffer[mReadPtr][mReadArrayIndex]) throw(emptyBuffer);
	return mOrderedBuffer[mReadPtr][mReadArrayIndex];
}; //end peek		

/**
 * Internal only use.  Reorder the buffers by inserting this item. 
 **/	
template<class T> 
void SpinBuffer<T>::poke(T element, int pos) throw(noSuchBufferException) 
{
	if ((0 <= pos) && (pos < SPIN_BUFFER_SIZE)) {
		try {
		mOrderedBuffer[pos][mWriteArrayIndex] = element;
		mWriteArrayIndex = (mWriteArrayIndex + 1) % SPIN_BUFFER_ARRAY_SIZE;
		} 
	    catch (exception& e)
  		{
    		//cout << e.what() << endl;
  		}
	} else {
		throw(noSuchBuffer);
	};
}; // end poke


#endif
