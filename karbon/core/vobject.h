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
	state_normal   = 0,	/// visible, not active
	state_selected = 1, /// visible, active and can be manipulated by tools
	state_edit     = 2, /// visible, active and can be manipulated by edit tools
	state_deleted  = 3  /// not visible
};


class QDomElement;
class QWMatrix;
class VPainter;
class VVisitor;

/**
 * The base class for all karbon objects. Every object should
 * have the ability to draw itself using a painter, manage
 * stroke/fill properties, be state aware, perform hit detection,
 * transform on demand, clone and load/save itself.
 * Also an extension mechanism is provided.
 */
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

	/**
	 * Draw the object to a painting device.
	 *
	 * @param painter abstraction that is used to render to a painting device.
	 * @param rect represents the visible rectangular area. If this object doesnt
	 *             intersect with this area it is not drawn.
	 */
	virtual void draw( VPainter* painter, const KoRect& rect ) = 0;

	/**
	 * Transform the object according to the given matrix.
	 * Note that this operation triggers an immediate object
	 * transformation and may be slow for complex objects.
	 *
	 * @param m matrix to use for the transformation.
	 */
	virtual void transform( const QWMatrix& m ) = 0;

	/**
	 * Calculates the tightest bounding box around the object.
	 *
	 * @param zoomFactor the zoom factor to take into account when calculating the bbox.
	 * @return the bounding box.
	 */
	virtual KoRect boundingBox( const double /*zoomFactor*/ ) const
		{ return KoRect(); }
	/**
	 * Tests whether this object intersects the given rectangle.
	 * Default is false, each VObject derivative has to implement
	 * this method.
	 *
	 * @param rect the rectangle to test against.
	 * @param zoomFactor 
	 * @return true indicates one or more segments intersect with rect, false otherwise.
	 */
	virtual bool intersects( const KoRect& /*rect*/, const double /*zoomFactor*/ ) const
		{ return false; };

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

	const VFill& fill() const { return m_fill; }
	const VStroke& stroke() const { return m_stroke; }

	virtual void setFill( const VFill& fill ) { m_fill = fill; }
	virtual void setStroke( const VStroke& stroke ) { m_stroke = stroke; }

	/**
	 * Create an exact copy of this object.
	 */
	virtual VObject* clone() = 0;

	/**
	 * Save this object's state to xml.
	 */
	virtual void save( QDomElement& element ) const;
	/**
	 * Load this object's state from xml and initialize
	 * this object accordingly.
	 */
	virtual void load( const QDomElement& element );

	/// Accept a VVisitor.
	virtual void accept( VVisitor& /*visitor*/ ) {}

protected:
	VFill m_fill;
	VStroke m_stroke;

private:
	VState m_state;
};

#endif
