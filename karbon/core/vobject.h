/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qptrlist.h>
#include <qrect.h>

class QDomElement;
class QPainter;
class QWMatrix;

class VCommand;

// The base class for all karbon objects.

class VObject
{
public:
	enum VState { normal, selected, edit, deleted };

	VObject()
		: m_state( normal ) {}
	virtual ~VObject() {}

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor ) = 0;

	virtual VObject& transform( const QWMatrix& m ) = 0;

	virtual QRect boundingBox( const double zoomFactor ) const { return QRect(); }
	virtual bool intersects( const QRect& rect, const double zoomFactor ) const
		{ return false; };

	void setState( const VState state ) { m_state = state; }
	VState state() const { return m_state; }

	virtual VObject *clone() = 0;

	virtual void save( QDomElement& element ) const = 0;
	virtual void load( const QDomElement& element ) = 0;

private:
	VState m_state;
};

typedef QPtrList<VObject> VObjectList;
typedef QPtrListIterator<VObject> VObjectListIterator;

#endif
