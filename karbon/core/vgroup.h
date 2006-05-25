/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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

#ifndef __VGROUP_H__
#define __VGROUP_H__

#include <q3ptrlist.h>

#include "vobject.h"
#include <koffice_export.h>
typedef Q3PtrList<VObject> VObjectList;
typedef Q3PtrListIterator<VObject> VObjectListIterator;

/**
 * Base class for all sort of VObject conglomerats.
 */

class KARBONBASE_EXPORT VGroup : public VObject
{
public:
	/**
	 * Constructs a new group object that is child of parent and has the given state.
	 *
	 * The object's fill and stroke are created here.
	 *
	 * @param parent the new object's parent
	 * @param state the new object's state
	 */
	VGroup( VObject* parent, VState state = normal );

	/**
	 * Copy constructor.
	 *
	 * @param group the group to copy properties from
	 */
	VGroup( const VGroup& group );

	/** 
	 * Destroys the group and all of the child objects.
	 */
	virtual ~VGroup();

	virtual void draw( VPainter* painter, const KoRect* rect = 0L ) const;

	virtual const KoRect& boundingBox() const;

	virtual void setStroke( const VStroke& stroke );
	virtual void setFill( const VFill& fill );

	virtual void setState( const VState state );

	virtual void save( QDomElement& element ) const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;
	virtual void load( const QDomElement& element );
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );

	virtual VGroup* clone() const;

	virtual void accept( VVisitor& visitor );

	/**
	 * Removes the reference to child object, not the object itself.
	 *
	 * @param object the child object to remove
	 */
	void take( const VObject& object );

	/**
	 * Appends a new child object.
	 *
	 * @param object the object to append
	 */
	void append( VObject* object );

	/**
	 * This function is important for undo/redo. It inserts newObject in front
	 * of oldObject.
	 *
	 * @param newObject the new object to insert
	 * @param oldObject the old object the new object is inserted in front of
	 */
	virtual void insertInfrontOf( VObject* newObject, VObject* oldObject );

	/**
	 * Clears the group, without destroying the grouped objects.
	 */
	void clear();

	/**
	 * Read only access to the grouped objects.
	 *
	 * @return reference to the list of grouped objects
	 */
	const VObjectList& objects() const { return m_objects; }

protected:
	VObjectList m_objects; /**< the list of grouped objects */
};

#endif

