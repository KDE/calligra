/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include "vgroup.h"
#include <koffice_export.h>
class QDomElement;
class DCOPObject;


/**
 * VLayer manages a set of vobjects. It keeps the objects from bottom to top
 * in a list, ie. objects higher in the list are drawn above lower objects.
 * Objects in a layer can be manipulated and worked on independant of objects
 * in other layers.
 */

class KARBONBASE_EXPORT VLayer : public VGroup
{
public:
	/**
	 * Constructs a new layer object that is child of parent and has the given state.
	 *
	 * @param parent the new object's parent
	 * @param state the new object's state
	 */
	VLayer( VObject* parent, VState state = normal );

	/**
	 * Copy constructor.
	 *
	 * @param layer the layer to copy properties from
	 */
	VLayer( const VLayer& layer );

	virtual ~VLayer();
	virtual DCOPObject* dcopObject();

	virtual void draw( VPainter *painter, const KoRect* rect = 0L ) const;

	virtual void save( QDomElement& element ) const;
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;
	virtual void load( const QDomElement& element );

	virtual VLayer* clone() const;

	virtual void accept( VVisitor& visitor );

	/**
	 * Moves the object to the top of the list.
	 *
	 * When the object is at the top this method has no effect.
	 *
	 * @param object the object to move
	 */
	void bringToFront( const VObject& object );

	/** 
	 * Moves the object one step up the list.
	 *
	 * When the object is at the top this method has no effect.
	 *
	 * @param object the object to move
	 */
	void upwards( const VObject& object );

	/** 
	 * Moves the object one step down the list.
	 *
	 * When the object is at the bottom this method has no effect.
	 *
	 * @param object the object to move
	 */
	void downwards( const VObject& object );

	/** 
	 * Moves the object to the end of the list.
	 *
	 * When the object is at the bottom this method has no effect.
	 *
	 * @param object the object to move
	 */
	void sendToBack( const VObject& object );

	/**
	 * Selects or unselects the layer 
	 *
	 * @param state the new selection state
	 */
	void setSelected( bool state ) { setState( state ? VObject::selected : VObject::normal ); }

	/**
	 * Returns the selection state of the layer
	 *
	 * @return the actual selection state
	 */
	bool selected() { return state() == VObject::selected; }
};

#endif

