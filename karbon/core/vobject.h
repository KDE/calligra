/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include <qptrlist.h>
#include <koRect.h>

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
	VObject( VObject *parent = 0L ) : m_parent( parent )
	{
		m_boundingBoxIsInvalid = true;
	}
	VObject( const VObject &obj )
	{
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
	 * Save this object's state to xml.
	 */
	virtual void save( QDomElement& element ) const = 0;
	/**
	 * Load this object's state from xml and initialize
	 * this object accordingly.
	 */
	virtual void load( const QDomElement& element ) = 0;

	/// Accept a VVisitor.
	virtual void accept( VVisitor& /*visitor*/ ) {}

	void setParent( VObject *parent ) { m_parent = parent; }
	VObject *parent() { return m_parent; }

protected:
	/// Bounding box.
	mutable KoRect m_boundingBox;
	mutable bool m_boundingBoxIsInvalid;

private:
	VObject *m_parent;
};

#endif
