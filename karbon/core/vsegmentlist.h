/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSEGMENTLIST_H__
#define __VSEGMENTLIST_H__

#include <qptrlist.h>

#include "vsegment.h"

class QDomElement;

class VSegmentList : public QPtrList<VSegment>
{
public:
	VSegmentList();
	VSegmentList( const VSegmentList& list );
	virtual ~VSegmentList();

	bool isClosed() const { return m_isClosed; }
	void close();

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	bool m_isClosed;
};

typedef QPtrListIterator<VSegment> VSegmentListIterator;

#endif

