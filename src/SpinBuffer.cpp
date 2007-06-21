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

namespace util {
#define SPIN_BUFFER_SIZE 3

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
 * and definition have to occur in this file, hence the .hpp.
 * You cannot belive the amount of duplication this causes.
 */
template <class _ST, int _BS=5> class SpinBuffer {
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
		void put(_ST) throw(outOfBuffersException);

		/**
		 * Read, destructively, a SpinBuffer
		 * @exception Throws an exception if the buffer is empty.
		 **/		
		_ST get(void) throw(emptyBufferException);

		/**
		 * Examine a SpinBuffer without destroying 
		 * @exception Throws an exception if the buffer is empty.
		 **/
		_ST peek(void) throw(emptyBufferException);
	protected:
		/**
		 * Internal only use.  Reorder the buffers by inserting this item into buffer int.
		 * @exception Throws an expception if the indicated buffer is outside the valid range for this spinBuffer. 
		 **/	
		void poke(_ST, int) throw(noSuchBufferException);
	private:
		/**
		 *\note
		 * The 3 buffers:
		 * 	a. The buffer of 0 is 1
		 *  b. The buffer of 1 is 2
		 *  c. The buffer of 2 is 0 
		 * Hence 'Spin' buffered.
		 **/
		_ST* mOrderedBuffer[SPIN_BUFFER_SIZE];
		int mReadPtr;
		int mWritePtr;
		int ArrayIndex[SPIN_BUFFER_SIZE];
}; // end PacketSpinBuffer

//-------------------------------------------------------------------

/**
 * Create a SpinBuffer
 **/
template<class _ST, int _BS>
SpinBuffer<_ST, _BS>::SpinBuffer() {
	mReadPtr = mWritePtr = 0;
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++) 
	{
		mOrderedBuffer[i] = new _ST[_BS]; // yes, this is silly
		ArrayIndex[i] = 0;
	};
	//put((_ST) NULL);// <-- prime the pump
	//mWritePtr = (mWritePtr + 1) % SPIN_BUFFER_SIZE; // <-- prime the pump
#ifdef DEBUG	
	cout << "util::SpinBuffer::SpinBuffer: created a new SpinBuffer" << endl;
#endif
}; // end constructor

/**
 * Destroy a SpinBuffer
 **/
template<class _ST, int _BS>
SpinBuffer<_ST, _BS>::~SpinBuffer(void) {
	mReadPtr = mWritePtr = 0;
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++) {
		mOrderedBuffer[i].clear();
	};	
	delete [] mOrderedBuffer;
#ifdef DEBUG
		cout << "util::SpinBuffer::~SpinBuffer: Deleted SpinBuffer." << endl;
#endif
}; // end delete

/**
 * Get a string image of this SpinBuffer
 **/
template<class _ST, int _BS>	 	
string SpinBuffer<_ST, _BS>::ToString(void) {
	ostringstream description;
	description << "SpinBuffer: reading at " << mReadPtr << ", writing to " <<
	 mWritePtr << ", with buffers of: ";
	 for (int i = 0;i < SPIN_BUFFER_SIZE;i++) 
	 {
	 	description << " " << ArrayIndex[i];
	 }; 
	 for (int i = 0;i < SPIN_BUFFER_SIZE;i++) 
	 {  
	 	for (int j = 0;j < ArrayIndex[i];j++)
	 	{
	 		if ((_ST) NULL != mOrderedBuffer[i][j]) 
	 			description << 	" " << mOrderedBuffer[i][j];
	 	}
	 }
	string desc = description.str(); 		
#ifdef DEBUG	 		
	 cout << "util::SpinBuffer::SpinBuffer: Convert to string:" << desc << endl;
#endif	 
	 return desc;

}; // end ToString		

/**
 * Concat two SpinBuffers 
 **/	 	
template<class _ST, int _BS> 
SpinBuffer<_ST, _BS>& SpinBuffer<_ST, _BS>::operator+(SpinBuffer<_ST, _BS>& right) 
{
	try {
		//get everything in the read buffer, in reading order
		for (int i = right.mReadPtr; i < right.arrayIndex[right.mReadPtr];i++) {
			if ((_ST) NULL != right.mOrderedBuffer[right.mReadPtr][i]) {
				put(right.mOrderedBuffer[right.mReadPtr][i]);
			}; // if item not null, copy it
		}; // for each possible item
		// get everything that was written, but not read
		for (int i = right.mWritePtr; i >= 0;i--) {
			if ((_ST) NULL != right.mOrderedBuffer[right.mWritePtr][i]) {
				put(right.mOrderedBuffer[right.mWritePtr][i]);
			}; // if item not null, copy it
		}; // for each possible item		  
		 // who cares about the free buffers!!!		 
	}
	catch (exception &e) {
		// cout << e.what() << endl;
	};
#ifdef DEBUG	
	cout << "util::SpinBuffer::operator+: Adding two SpinBuffers" <<  endl;
#endif	
	return *this;	
}; // end operator+

/**
 * Copy a SpinBuffer onto another SpinBuffer 
 **/	 	
template<class _ST, int _BS> 
SpinBuffer<_ST, _BS>& SpinBuffer<_ST, _BS>::operator=(SpinBuffer<_ST, _BS> &right) 
{
	mWritePtr = right.mWritePtr;
	mReadPtr = right.mReadPtr;
	ArrayIndex = right.ArrayIndex;
	try {	
	for (int i = 0;i < SPIN_BUFFER_SIZE;i++) 
		mOrderedBuffer[i] = right.mOrderedBuffer[i];
	}
	catch (exception &e) {
#ifdef DEBUG		
		cout << e.what() << endl;
#endif		
	};
#ifdef DEBUG
	cout << "util::SpinBuffer::operator=: assignment called."<< endl;
#endif
	return *this; 
}; // end operator=		
	

/**
 * Is this buffer empty?
 **/	 	
template<class _ST, int _BS>
bool SpinBuffer<_ST, _BS>::empty(void) {
	bool result = (_BS == (1 + ArrayIndex[mWritePtr]) &&
   	 mReadPtr == ((mWritePtr + 1) % SPIN_BUFFER_SIZE));
		if (!result) {	
#ifdef DEBUG
	cout << "util::SpinBuffer::empty: buffer IS empty." << endl;
#endif	
			return true;
		} else {
#ifdef DEBUG
	cout << "util::SpinBuffer::empty: buffer NOT empty. At least " <<
	ArrayIndex[mReadPtr] << " items left." << endl;
#endif				
			return false;
		};
} // end empty		
	
/**
 * Is this buffer full?
 **/	 	
template<class _ST, int _BS>
bool SpinBuffer<_ST, _BS>::full(void) {
	bool result = (_BS == (1 + ArrayIndex[mWritePtr]) &&
   	 mReadPtr == ((mWritePtr + 1) % SPIN_BUFFER_SIZE));
		if (result) {	
#ifdef DEBUG
	cout << "util::SpinBuffer::full: buffer IS full." << endl;
#endif	
			return true;
		} else {
#ifdef DEBUG
	cout << "util::SpinBuffer::full: buffer NOT full." << endl;
#endif				
			return false;
		};
} // end full
	
/**
 * Write to the SpinBuffer
 **/		
template<class _ST, int _BS>
void SpinBuffer<_ST, _BS>::put(_ST element) throw(outOfBuffersException) 
{
	// a consuquence of using fix-sized arrays
   	//if (full()) { throw(outOfBuffers); return;};
   	if (full()) return;
   /**
    * 1. Put the item into the write buffer.
    * 2. If the next buffer is free, 
    * 	make the free buffer become the write buffer, 
    * 	and the current write buffer becomes free.
    **/
    int i = (mWritePtr + 1) % SPIN_BUFFER_SIZE;
   	mOrderedBuffer[mWritePtr][ArrayIndex[mWritePtr]] = element;
#ifdef DEBUG   	
   	cout << "util:SpinBuffer::put: Added item. Writing at buffer " << 
   	 mWritePtr << " and at array pos " << ArrayIndex[mWritePtr] << endl;
#endif  	
   	// march along the array
   	ArrayIndex[mWritePtr] = (ArrayIndex[mWritePtr] + 1) % _BS;
   	if (mReadPtr != i && 0 == ArrayIndex[mWritePtr]) 
   	{ 
   		// moving to the next buffer, clear out
   		mWritePtr = i; 
   		//ArrayIndex[mWritePtr] = 0;
   	};   		
}; // end put

/**
 * Read, destructively, a SpinBuffer
 **/		
template<class _ST, int _BS>
_ST SpinBuffer<_ST, _BS>::get(void) throw(emptyBufferException) 
{
	//if (empty()) throw(emptyBuffer);
	try {
	/**
	 *  1. Read the item from the read buffer.
	 *  2. If the current read buffer is empty 
	 * 		and the next buffer is free, 
	 * 		make the next buffer the read buffer, 
	 * 		and the current read buffer becomes free.
	 **/
		_ST element = mOrderedBuffer[mReadPtr][ArrayIndex[mReadPtr]];
		mOrderedBuffer[mReadPtr][ArrayIndex[mReadPtr]] = (_ST) NULL;
		// march along the array
		ArrayIndex[mWritePtr] = (ArrayIndex[mWritePtr] + 1) % _BS; 
		int i = (mReadPtr + 1) % SPIN_BUFFER_SIZE;		
		if (mWritePtr != i) {
			mReadPtr = i;
			ArrayIndex[mWritePtr] = 0;
		};
#ifdef DEBUG		
   		cout << "util::SpinBuffer::get: Returning item " << element << 
   		  " from Buffer " << mReadPtr << " at pos " << ArrayIndex[mReadPtr] << endl;
#endif		
		return element;			
	 }
    catch (exception& e)
	{
#ifdef DEBUG		
		cout << e.what() << endl;
#endif		
	}
}; // end get
/**
 * Examine a SpinBuffer without destroying 
 **/
template<class _ST, int _BS>
_ST SpinBuffer<_ST, _BS>::peek(void) throw(emptyBufferException) 
{
	if (0 == ArrayIndex[mReadPtr] &&
	  (_ST) NULL == mOrderedBuffer[mReadPtr][ArrayIndex[mReadPtr]]) throw(emptyBuffer);
#ifdef DEBUG	  
   	cout << "util::SpinBuffer::peek: Examined item " << endl;
#endif   		  
	return mOrderedBuffer[mReadPtr][ArrayIndex[mReadPtr]];
}; //end peek		

/**
 * Internal only use.  Reorder the buffers by inserting this item. 
 **/	
template<class _ST, int _BS> 
void SpinBuffer<_ST, _BS>::poke(_ST element, int pos) throw(noSuchBufferException) 
{
	if ((0 <= pos) && (pos < SPIN_BUFFER_SIZE)) {
		try {
#ifdef DEBUG			
		cout << "util::SpinBufer::poke: Inserted item "  << endl;
#endif
		mOrderedBuffer[pos][ArrayIndex[mWritePtr]] = element;
		ArrayIndex[mWritePtr] = (ArrayIndex[mWritePtr] + 1) % _BS;
		} 
	    catch (exception& e)
  		{
#ifdef DEBUG		
		cout << e.what() << endl;
#endif		
  		}
	} else {
		throw(noSuchBuffer);
	};
}; // end poke

} // end namespace util

#endif
