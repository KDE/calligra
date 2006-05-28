/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__


#include <QRectF>
#include <dcopobject.h>
#include <koffice_export.h>

class QDomElement;
class VDocument;
class VFill;
class VPainter;
class VStroke;
class VVisitor;
class DCOPObject;
class KoStore;
class KoXmlWriter;
class KoOasisLoadingContext;
class KoGenStyles;
class KoGenStyle;

/**
 * The base class for all karbon objects. Every object should
 * have the ability to draw itself using a painter, perform
 * hit detection, transform on demand, clone and load/save itself.
 * Also each object manages its own bounding box and keeps track of its
 * parent object.
 */
class KARBONBASE_EXPORT VObject
{
public:
	enum VState
	{
		normal        = 0,	/**< visible, not active */
		normal_locked = 1,	/**< visible, but locked (r/o) */
		hidden        = 2,	/**< hidden */
		hidden_locked = 3,	/**< hidden and locked (r/o) */
		deleted       = 4,	/**< deleted, nearly dead */

		// shape specific states:
		selected      = 5,	/**< visible, active and can be manipulated by tools */
		edit          = 6	/**< visible, active and is currently manipulated by a tool */
	};

	/**
	 * Constructs a new object that is child of parent and has the given state.
	 *
	 * @param parent the new object's parent
	 * @param state the new object's state
	 */
	VObject( VObject* parent, VState state = edit );
	
	/**
	 * Copy constructor.
	 * Copies parent, state and name of given object.
	 *
	 * @param obj the object to copy properties from
	 */
	VObject( const VObject& obj );

	/** 
	 * Destroys the object and deletes the stroke, fill and DCOP-object.
	 */
	virtual ~VObject();

	/**
	 * Returns pointer to internal DCOP object.
	 *
	 * If no internal DCOP object exist yet, it is created.
	 */
	virtual DCOPObject* dcopObject();

	/**
	 * Draw the object to a painting device.
	 *
	 * @param painter abstraction that is used to render to a painting device.
	 * @param rect represents the visible rectangular area. If this object doesn't
	 *             intersect with this area it is not drawn.
	 */
	virtual void draw( VPainter* painter, const QRectF* rect = 0L ) const 
	{ 
		Q_UNUSED( painter );
		Q_UNUSED( rect );
	}

	/**
	 * Calculates the tightest bounding box around the object.
	 *
	 * @return the bounding box.
	 */
	virtual const QRectF& boundingBox() const
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
	 * Sets a new parent object.
	 *
	 * @param parent the new parent object
	 */
	void setParent( VObject* parent ) { m_parent = parent; }

	/**
	 * Returns pointer to current parent object.
	 *
	 * @return pointer to current parent object or 0 if no parent object is set
	 */
	VObject* parent() const { return m_parent; }

	/**
	 * Get the state the object is in.
	 *
	 * @return the object state at time of calling.
	 */
	VState state() const { return m_state; }

	/**
	 * Sets the state to a specified new state.
	 * Note that this will not have any visual effect until draw() is
	 * called on this object.
	 *
	 * @param state the new state.
	 */
	virtual void setState( const VState state ) { m_state = state; }

	/**
	 * Gets the object's actual stroke.
	 *
	 * @return pointer to the object's stroke
	 */
	virtual VStroke* stroke() const { return m_stroke; }

	/**
	 * Gets the object's actual fill.
	 *
	 * @return pointer to the object's fill
	 */
	virtual VFill* fill() const { return m_fill; }

	/**
	 * Sets the stroke to a given new stroke.
	 *
	 * @param stroke the new stroke
	 */
	virtual void setStroke( const VStroke& stroke );

	/**
	 * Sets the fill to a given new fill.
	 *
	 * @param fill the new fill
	 */
	virtual void setFill( const VFill& fill );

	/**
	 * Save this object's state to xml.
	 * 
	 * @param element the DOM element to which the attributes are saved
	 */
	virtual void save( QDomElement& element ) const;

	/**
	 * Save this object's state to OpenDocument.
	 *
	 * @param store FIXME
	 * @param docWriter FIXME
	 * @param mainStyles FIXME
	 */
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;

	/**
	 * Load this object's state from xml and initialize
	 * this object accordingly.
	 *
	 * @param element the DOM element from which the attributes are read
	 */
	virtual void load( const QDomElement& element );

	/**
	 * Load this object's state from OpenDocument and initialize
	 * this object accordingly.
	 *
	 * @param element the DOM element to read attributes from
	 * @param context FIXME
	 */
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );

	/**
	 * Create an exact copy of this object.
	 *
	 * @return the exact object copy
	 */
	virtual VObject* clone() const = 0;

	/** 
	 * Accept a VVisitor.
	 */
	virtual void accept( VVisitor& /*visitor*/ ) 
		{ }

	/**
	 * This function is important for undo/redo. It inserts newObject in front
	 * of oldObject.
	 *
	 * @param newObject the new object to insert
	 * @param oldObject the old object the new object is inserted in front of
	 */
	virtual void insertInfrontOf( VObject* newObject, VObject* oldObject )
	{ 
		Q_UNUSED( newObject );
		Q_UNUSED( oldObject );
	}

	/**
	 * Returns the name of the object.
	 *
	 * @return the object's name
	 */
	virtual QString name() const;
	
	/**
	 * Sets the object's name to a given new name.
	 *
	 * @param s the new object name 
	 */
	void setName( const QString &s );

	/**
	 * Return document the object belongs to.
	 *
	 * @return pointer to parent document or 0 if object does not belong to a document
	 */
	VDocument *document() const;

protected:
	/**
	 * Adds a new given style to the specified OASIS context
	 *
	 * @param style FIXME
	 * @param context FIXME
	 */
	void addStyles( const QDomElement* style, KoOasisLoadingContext & context );

	virtual void saveOasisFill( KoGenStyles &mainStyles, KoGenStyle &stylesojectauto ) const;

protected:
	mutable QRectF m_boundingBox; /**< the object's bounding box */
	mutable VState m_state				: 8; /**< the object's state */
	mutable bool m_boundingBoxIsInvalid : 1; /**< the flag stating if the bounding box is valid */

	VStroke* m_stroke; /**< the object's stroke */
	VFill* m_fill; /**< the object's fill */

	DCOPObject *m_dcop; /**< the object's DCOP object */

private:
	VObject* m_parent;
};

#endif

