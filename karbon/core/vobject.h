/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qrect.h>

class QPainter;
class QWMatrix;

class VCommand;

// The base class for all karbon objects.

class VObject
{
public:
	enum VState { normal=0, selected=1, edit=2, deleted=3, invisible=4 };

	VObject()
		: m_state( normal ) {}
	virtual ~VObject() {}

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor ) = 0;

	virtual VObject& transform( const QWMatrix& m ) = 0;

	virtual QRect boundingBox() const { return QRect(); }

	void setState( const VState state ) { m_state = state; }
	VState state() const { return m_state; }

	virtual VObject *clone() = 0;

private:
	VState m_state;
};

#endif
