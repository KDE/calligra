/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTLIST_H__
#define __VSEGMENTLIST_H__

#include <qptrlist.h>
#include <koPoint.h>

#include "vobject.h"
#include "vsegment.h"

class QDomElement;
class QWMatrix;


// The general list stuff is stolen from Qt.

class VSegmentListIteratorList;

class VSegmentList : public VObject
{
friend class VSegmentListIterator;

public:
	VSegmentList( VObject* parent = 0L );
	VSegmentList( const VSegmentList& list );
	virtual ~VSegmentList();

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

	bool isClosed() const { return m_isClosed; }
	void close();

	virtual void draw( VPainter*, const KoRect& ) {}

	virtual void transform( const QWMatrix& m );

	virtual const KoRect& boundingBox() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );


	// general list stuff:
	VSegmentList& operator=( const VSegmentList& list );

	bool insert( const VSegment* segment );
	bool insert( uint i, const VSegment* segment );
	void prepend( const VSegment* segment );
	void append( const VSegment* segment );
	void clear();

	uint count() const { return m_number; }

	VSegment* current() const { return m_current; }
	VSegment* getFirst() const { return m_first; }
	VSegment* getLast() const { return m_last; }
	VSegment* first();
	VSegment* last();
	VSegment* prev();
	VSegment* next();

private:
	VSegment* locate( uint index );

	bool m_isClosed;

	VSegment* m_first;
	VSegment* m_last;
	VSegment* m_current;

	int m_currentIndex;
	uint m_number;

	VSegmentListIteratorList* m_iteratorList;
};


class VSegmentListIterator
{
friend class VSegmentListIteratorList;

public:
	VSegmentListIterator( const VSegmentList& list );
	VSegmentListIterator( const VSegmentListIterator& itr );
	~VSegmentListIterator();

	VSegmentListIterator& operator=( const VSegmentListIterator& itr );

	VSegment* current() const { return m_current; }
	VSegment* operator()();
	VSegment* operator++();
	VSegment* operator+=( uint i );
	VSegment* operator--();
	VSegment* operator-=( uint i );

private:
	VSegmentList* m_list;
	VSegment* m_current;
};

#endif

