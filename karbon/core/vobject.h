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
	state_edit     = 2, /// visible, active and is currently manipulated by a tool
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
	VObject( VObject *parent = 0L, VState state = state_normal ) : m_parent( parent )
	{
		m_state = state;
		m_boundingBoxIsInvalid = true;
	}
	VObject( const VObject &obj )
	{
		m_fill   = obj.m_fill;
		m_stroke = obj.m_stroke;
		m_state  = obj.m_state;
		m_boundingBoxIsInvalid = true;
		m_parent = obj.m_parent;
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
	 * @return the bounding box.
	 */
	virtual const KoRect& boundingBox() const
		{ return m_boundingBox; }

	/**
	 * Checks if the bounding box is invalid and needs to be recalculated.
	 *
	 * @return true if bounding box is invalid.
	 */
	bool boundingBoxIsInvalid() const
		{ return m_boundingBoxIsInvalid; }

	/**
	 * Invalidates the bounding box, so it has to be recalculated.
	 */
	void invalidateBoundingBox()
		{ m_boundingBoxIsInvalid = true; }

	/**
	 * Tests whether this object intersects the given rectangle.
	 * Default is false, each VObject derivative has to implement
	 * this method.
	 *
	 * @param rect the rectangle to test against.
	 * @param zoomFactor 
	 * @return true indicates one or more segments intersects or is inside rect,
	 * false otherwise.
	 */
	virtual bool isInside( const KoRect& /*rect*/ ) const
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

	void setParent( VObject *parent ) { m_parent = parent; }
	VObject *parent() { return m_parent; }

protected:
	VFill m_fill;
	VStroke m_stroke;

	/// Bounding box.
	mutable KoRect m_boundingBox;
	mutable bool m_boundingBoxIsInvalid;

private:
	VState m_state;
	VObject *m_parent;
};

#endif
