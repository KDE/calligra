/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSHAPE_H__
#define __VSHAPE_H__

#include "vobject.h"
#include "vfill.h"
#include "vstroke.h"

enum VState
{
	state_normal   = 0,	/// visible, not active
	state_selected = 1, /// visible, active and can be manipulated by tools
	state_edit     = 2, /// visible, active and is currently manipulated by a tool
	state_deleted  = 3  /// not visible
};

/**
 * The base class for all karbon objects. Every object should
 * have the ability to draw itself using a painter, manage
 * stroke/fill properties, be state aware, perform hit detection,
 * transform on demand, clone and load/save itself.
 * Also an extension mechanism is provided.
 */
class VShape : public VObject
{
public:
	VShape( VObject *parent = 0L, VState state = state_normal ) : VObject( parent )
	{
		m_state = state;
		m_boundingBoxIsInvalid = true;
	}
	VShape( const VShape &shape ) : VObject( shape )
	{
		m_fill   = shape.m_fill;
		m_stroke = shape.m_stroke;
		m_state  = shape.m_state;
		m_boundingBoxIsInvalid = true;
	}
	virtual ~VShape() {}

	/**
	 * Get the state the object is in.
	 *
	 * @return the object state at time of calling.
	 */
	VState state() const { return m_state; }
	/**
	 * Sets the state to a specified new state.
	 * Note that this will not have any effect until draw() is
	 * called on this object.
	 *
	 * @param state the new state.
	 */
	virtual void setState( const VState state ) { m_state = state; }

	/**
	 * Create an exact copy of this object.
	 *
	 */
	virtual VShape* clone() = 0;

	const VFill& fill() const { return m_fill; }
	const VStroke& stroke() const { return m_stroke; }

	virtual void setFill( const VFill& fill ) { m_fill = fill; }
	virtual void setStroke( const VStroke& stroke ) { m_stroke = stroke; }

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

protected:
	VFill m_fill;
	VStroke m_stroke;

private:
	VState m_state;
};

#endif
