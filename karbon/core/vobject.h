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
class QWMatrix;
class VPainter;
class VVisitor;

// The base class for all karbon objects.

class VObject
{
public:
	VObject( VState state = state_normal ) { m_state = state; }
	VObject( const VObject &obj )
	{
		m_fill   = obj.m_fill;
		m_stroke = obj.m_stroke;
		m_state  = obj.m_state;
	}
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

	const VFill& fill() const { return m_fill; }
	VFill& fill() { return m_fill; }
	const VStroke& stroke() const { return m_stroke; }
	VStroke& stroke() { return m_stroke; }

	virtual void setFill( const VFill &fill ) { m_fill = fill; }
	virtual void setStroke( const VStroke &stroke ) { m_stroke = stroke; }

	virtual VObject* clone() = 0;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	/// Accept a VVisitor.
	virtual void accept( const VVisitor& /*visitor*/ ) {}

protected:
	VFill m_fill;
	VStroke m_stroke;

private:
	VState m_state;
};

#endif
