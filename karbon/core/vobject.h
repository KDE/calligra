/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qrect.h>

#include "vtool.h"

class QPainter;
class QWMatrix;

class VCommand;

// The base class for all karbon objects.

class VObject
{
public:
	enum VState { normal, edit, deleted, invisible };

	VObject()
		: m_state( normal ) {}
	virtual ~VObject() {}

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor ) = 0;

	virtual VCommand* accept( const VTool& tool ) { return tool.manipulate( this ); }

	virtual VObject& transform( const QWMatrix& m ) = 0;

	QRect boundingBox() const { return QRect(); }

	void setState( const VState state ) { m_state = state; }
	bool state() const { return m_state; }

private:
	VState m_state;
};

#endif
