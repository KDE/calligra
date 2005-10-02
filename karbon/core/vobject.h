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


#include <koRect.h>
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
class KoOasisContext;
class KoGenStyles;

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
		normal        = 0,	/// visible, not active
		normal_locked = 1,	/// visible, but locked (r/o)
		hidden        = 2,	/// hidden
		hidden_locked = 3,	/// hidden and locked (r/o)
		deleted       = 4,	/// deleted, nearly dead

		// shape specific states:
		selected      = 5,	/// visible, active and can be manipulated by tools
		edit          = 6	/// visible, active and is currently manipulated by a tool
	};

	VObject( VObject* parent, VState state = edit );
	VObject( const VObject& obj );

	virtual ~VObject();

	virtual DCOPObject* dcopObject();

	/**
	 * Draw the object to a painting device.
	 *
	 * @param painter abstraction that is used to render to a painting device.
	 * @param rect represents the visible rectangular area. If this object doesn't
	 *             intersect with this area it is not drawn.
	 */
	virtual void draw( VPainter* /*painter*/, const KoRect* /*rect*/ = 0L ) const {}

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

	void setParent( VObject* parent ) { m_parent = parent; }
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

	virtual VStroke* stroke() const { return m_stroke; }
	virtual VFill* fill() const { return m_fill; }

	virtual void setStroke( const VStroke& stroke );
	virtual void setFill( const VFill& fill );

	/**
	 * Save this object's state to xml.
	 */
	virtual void save( QDomElement& element ) const;

	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles ) const;

	/**
	 * Load this object's state from xml and initialize
	 * this object accordingly.
	 */
	virtual void load( const QDomElement& element );

	virtual bool loadOasis( const QDomElement &element, KoOasisContext &context );

	/**
	 * Create an exact copy of this object.
	 */
	virtual VObject* clone() const = 0;

	/// Accept a VVisitor.
	virtual void accept( VVisitor& /*visitor*/ ) {}

	/**
	 * This function is important for undo/redo. It inserts newObject in front
	 * of oldObject.
	 */
	virtual void insertInfrontOf( VObject* /*newObject*/, VObject* /*oldObject*/ ) { }

	virtual QString name() const;
	void setName( const QString & );

	VDocument *document() const;

protected:
	void addStyles( const QDomElement* style, KoOasisContext & context );

protected:
	/// Bounding box.
	mutable KoRect m_boundingBox;
	mutable VState m_state				: 8;
	mutable bool m_boundingBoxIsInvalid : 1;

	VStroke* m_stroke;
	VFill* m_fill;

	DCOPObject *m_dcop;

private:
	VObject* m_parent;
};

#endif

