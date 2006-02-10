/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VPATH_H__
#define __VPATH_H__


#include <KoPoint.h>

#include "vobject.h"
#include <koffice_export.h>

class QDomElement;
class QWMatrix;
class VSubpathIteratorList;
class VSegment;
class VVisitor;


/**
 * VSubpath provides a sophisticated list of VSegment. Noted: it also may contain
 * segments which are marked "deleted". If you are not interested in those undo/redo
 * housholding data, please always use a VSubpathIterator to access segments.
 */

class KARBONBASE_EXPORT VSubpath : public VObject
{
	friend class VSubpathIterator;

public:
	VSubpath( VObject* parent );
	VSubpath( const VSubpath& list );
	VSubpath( const VSegment& segment );
	virtual ~VSubpath();

	const KoPoint& currentPoint() const;

	bool moveTo( const KoPoint& p );
	bool lineTo( const KoPoint& p );
	bool curveTo(
		const KoPoint& p1, const KoPoint& p2, const KoPoint& p3 );
	bool curve1To(
		const KoPoint& p2, const KoPoint& p3 );
	bool curve2To(
		const KoPoint& p1, const KoPoint& p3 );
	bool arcTo(
		const KoPoint& p1, const KoPoint& p2, const double r );

	bool isClosed() const
	{
		return m_isClosed;
	}

	void close();


	/**
	 * Returns true if point p is located inside the path.
	 * The winding number test is used.
	 */
	bool pointIsInside( const KoPoint& p ) const;

	/**
	 * Returns true if the segment intersects this path.
	 */
	bool intersects( const VSegment& segment ) const;


	/**
	 * Returns false if segmentlist is oriented clockwise.
	 */
	bool counterClockwise() const;

	/**
	 * Reverts the winding orientation.
	 */
	void revert();


	/**
	 * Returns true if the current path is "emtpy". That means that it has
	 * zero or just one ( == "begin") segment.
	 */
	bool isEmpty() const
	{
		return count() <= 1;
	}


	virtual const KoRect& boundingBox() const;


	virtual void save( QDomElement& /*element*/) const
		{ }	// VSubpaths cant be saved.

	// TODO: remove this backward compatibility function after koffice 1.3.x
	virtual void load( const QDomElement& element );

	void saveSvgPath( QString & ) const;


	virtual VSubpath* clone() const;

	virtual void accept( VVisitor& visitor );


	// General list stuff.
	VSubpath& operator=( const VSubpath& list );

	bool insert( const VSegment* segment );
	bool insert( uint i, const VSegment* segment );
	void prepend( const VSegment* segment );
	void append( const VSegment* segment );
	void clear();

	uint count() const
	{
		return m_number;
	}

	VSegment* current() const
	{
		return m_current;
	}

	VSegment* getFirst() const
	{
		return m_first;
	}

	VSegment* getLast() const
	{
		return m_last;
	}

	VSegment* first();
	VSegment* last();
	VSegment* prev();
	VSegment* next();

private:
	VSegment* locate( uint index );

	VSegment* m_first;
	VSegment* m_last;
	VSegment* m_current;

	int m_currentIndex;
	uint m_number : 31;

	bool m_isClosed : 1;

	VSubpathIteratorList* m_iteratorList;
};


/**
 * VSubpathIterator provides an iterator class for highlevel path access.
 * Use VSubpathIterator whenever you want to access segments but are not interested
 * in undo/redo operations with (deleted) segments.
 */

class KARBONBASE_EXPORT VSubpathIterator
{
	friend class VSubpathIteratorList;

public:
	VSubpathIterator( const VSubpath& list );
	VSubpathIterator( const VSubpathIterator& itr );
	~VSubpathIterator();

	VSubpathIterator& operator=( const VSubpathIterator& itr );

	VSegment* current() const;
	VSegment* operator()();
	VSegment* operator++();
	VSegment* operator+=( uint i );
	VSegment* operator--();
	VSegment* operator-=( uint i );

private:
	VSubpath* m_list;
	VSegment* m_current;
};

#endif

