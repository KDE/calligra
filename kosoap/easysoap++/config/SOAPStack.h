/* 
 * EasySoap++ - A C++ library for SOAP (Simple Object Access Protocol)
 * Copyright (C) 2001 David Crowley; SciTegic, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __SOAPSTACK_H__
#define __SOAPSTACK_H__

#include "SOAPArray.h"

template<typename T>
class SOAPStack
{
private:
	SOAPArray<T> m_array;

public:
	// can initialize the internal vector to the given size
	SOAPStack(size_t numitems = 0) : m_array(numitems) {m_array.Resize(0);}

	// returns true if there are no items on the stack
	bool IsEmpty() const {return Size() == 0;}

	// put an item on the top of the stack
	T& Push(const T& x)
	{
		return m_array.Add(x);
	}

	// removes the item at the top of the stack
	// and returns it
	void Pop()
	{
		if (IsEmpty())
			throw SOAPException("SOAPStack is empty");

		long top = m_array.Size() - 1;
		m_array.Resize(top);
	}

	// Returns the item at the top of the stack without
	// removing it.
	const T& Top() const
	{
		if (IsEmpty())
			throw SOAPException("SOAPStack is empty");

		size_t top = m_array.Size() - 1;
		return m_array[top];
	}

	// how many items are on the stack
	long Size() const {return m_array.Size();}

	// Pop everything off of the stack
	void Clear() {m_array.Resize(0);}
};


#endif // __SOAPSTACK_H__


