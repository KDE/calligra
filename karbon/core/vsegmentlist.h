/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTLIST_H__
#define __VSEGMENTLIST_H__

#include <qptrlist.h>
#include <koPoint.h>

#include "vsegment.h"

class QDomElement;
class QWMatrix;

class VSegmentList : public QPtrList<VSegment>
{
public:
	VSegmentList();
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

	void transform( const QWMatrix& m );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	bool m_isClosed;
};

typedef QPtrListIterator<VSegment> VSegmentListIterator;

#endif

