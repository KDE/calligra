/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <koRect.h>

class QDomElement;
class QWMatrix;
class VFill;
class VPainter;
class VStroke;
class VVisitor;


enum VState
{
	state_normal        = 0,	/// visible, not active
	state_normal_locked = 1,	/// visible, but locked (r/o)
	state_hidden        = 2,	/// hidden
	state_hidden_locked = 3,	/// hidden and locked (r/o)
	state_deleted       = 4,	/// deleted, nearly dead

	// shape specific states:
	state_selected      = 5,	/// visible, active and can be manipulated by tools
	state_edit          = 6		/// visible, active and is currently manipulated by a tool
};

/**
 * The base class for all karbon objects. Every object should
 * have the ability to draw itself using a painter, perform
 * hit detection, transform on demand, clone and load/save itself.
 * Also each object manages its own bounding box and keeps track of its
 * parent object.
 */
class VObject
{
public:
	VObject( VObject* parent = 0L, VState state = state_normal );
	VObject( const VObject& obj );

	virtual ~VObject();

	/**
	 * Draw the object to a painting device.
	 *
	 * @param painter abstraction that is used to render to a painting device.
	 * @param rect represents the visible rectangular area. If this object doesnt
	 *             intersect with this area it is not drawn.
	 */
	virtual void draw( VPainter* /*painter*/, const KoRect& /*rect*/ ) const {}

	/**
	 * Transform the object according to the given matrix.
	 * Note that this operation triggers an immediate object
	 * transformation and may be slow for complex objects.
	 *
	 * @param m matrix to use for the transformation.
	 */
	virtual void transform( const QWMatrix& /*m*/ ) {}

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
	 * This function is public so visitors can access it themself at the right
	 * time when they manipulate many VSegments.
	 */
	void invalidateBoundingBox()
	{
		m_boundingBoxIsInvalid = true;

		if( m_parent )
			m_parent->invalidateBoundingBox();
	}

	/**
	 * Tests whether this object is inside or intersects the given rectangle.
	 * Default is false, each VObject derivative has to implement
	 * this method.
	 *
	 * @param rect the rectangle to test against.
	 * @param zoomFactor 
	 * @return true indicates one or more segments intersects or is inside rect,
	 * false otherwise.
	 */
	virtual bool isInside( const KoRect& /*rect*/ ) const
		{ return false; }

	void setParent( VObject* parent ) { m_parent = parent; }
	VObject* parent() { return m_parent; }

	/**
	 * Get the state the shape is in.
	 *
	 * @return the shape state at time of calling.
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

	const VStroke* stroke() const { return m_stroke; }
	const VFill* fill() const { return m_fill; }

	virtual void setStroke( const VStroke& stroke );
	virtual void setFill( const VFill& fill );

	/**
	 * Save this object's state to xml.
	 */
	virtual void save( QDomElement& element ) const;

	/**
	 * Load this object's state from xml and initialize
	 * this object accordingly.
	 */
	virtual void load( const QDomElement& element );

	/**
	 * Create an exact copy of this shape.
	 */
	virtual VObject* clone() const = 0;

	/// Accept a VVisitor.
	virtual void accept( VVisitor& /*visitor*/ ) {}

protected:
	/// Bounding box.
	mutable KoRect m_boundingBox;
	mutable bool m_boundingBoxIsInvalid;

	VStroke* m_stroke;
	VFill* m_fill;

private:
	VObject* m_parent;

	VState m_state;
};

#endif

