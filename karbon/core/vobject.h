/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qptrlist.h>
#include <koRect.h>

#include "vfill.h"
#include "vstroke.h"

enum VState
{
	state_normal   = 0,
	state_selected = 1,
	state_edit     = 2,
	state_deleted  = 3
};


class QDomElement;
class VPainter;
class QWMatrix;


// The base class for all karbon objects.

class VObject
{
public:
	VObject()
		: m_state( state_normal ) {}
	VObject( const VObject &obj ) { m_fill = obj.m_fill; m_stroke = obj.m_stroke; m_state = obj.m_state; }
	virtual ~VObject() {}

	virtual void draw( VPainter* painter, const KoRect& rect,
		const double zoomFactor = 1.0  ) = 0;

	virtual void transform( const QWMatrix& m ) = 0;

	virtual KoRect boundingBox( const double /*zoomFactor*/ ) const
		{ return KoRect(); }
	virtual bool intersects( const KoRect& /*rect*/, const double /*zoomFactor*/ ) const
		{ return false; };

	VState state() const { return m_state; }
	virtual void setState( const VState state ) { m_state = state; }

	VFill& fill() { return m_fill; }
	VStroke& stroke() { return m_stroke; }

	virtual VObject* clone() = 0;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

private:
	VState m_state;
	VFill m_fill;
	VStroke m_stroke;
};

#endif
