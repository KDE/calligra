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

#ifndef __VSELECTION_H__
#define __VSELECTION_H__


#include <q3ptrlist.h>

#include <KoRect.h>

#include "vobject.h"
#include "vvisitor.h"
#include <koffice_export.h>
class KoPoint;
class QObject;
class VPainter;
class VVisitor;
class VSegment;

typedef Q3PtrList<VObject> VObjectList;
typedef Q3PtrListIterator<VObject> VObjectListIterator;


/// Ids of manipulation nodes.
enum VHandleNode
{
	node_none = 0,
	node_lt = 1,
	node_mt = 2,
	node_rt = 3,
	node_lm = 4,
	node_mm = 5,
	node_rm = 6,
	node_lb = 7,
	node_mb = 8,
	node_rb = 9
};


/**
 * VSelection manages a set of selected vobjects.
 */
class KARBONBASE_EXPORT VSelection : public VObject, public VVisitor
{
public:
	/** 
	 * Constructs a vselection with the specified parent.
	 * 
	 * @param parent the selection's parent
	 */
	VSelection( VObject* parent = 0L );

	/**
	 * Constructs a vselection by copying the specified selection.
	 *
	 * @param selection the selection to copy from
	 */
	VSelection( const VSelection& selection );

	/**
	 * Destroys the selection.
	 */
	virtual ~VSelection();

	/**
	 * Paint selected objects outline and handle.
	 */
	void draw( VPainter* painter, double zoomFactor ) const;

	virtual const KoRect& boundingBox() const;

	virtual VSelection* clone() const;

	virtual void accept( VVisitor& visitor );

	/**
	 * Adds all objects to the selection.
	 */
	void append();

	/**
	 * Adds an object to the selection.
	 */
	void append( VObject* object );

	/**
	 * Adds all objects of the specified object list to the selection.
	 *
	 * @param objects the list of objects to add
	 */
	void append( const VObjectList &objects );

	/**
	 * Adds all objects ( selectObjects == true ) or all nodes
	 * ( selectObjects == false ) within rect to the selection.
	 */
	bool append( const KoRect& rect, bool selectObjects = true, bool exclusive = true );

	/**
	 * Removes the reference to the object, not the object itself.
	 */
	void take( VObject& object );

	/**
	 * Removes all objects ( selectObjects == true ) or all nodes
	 * ( selectObjects == false ) within rect from the selection.
	 */
	bool take( const KoRect& rect, bool selectObjects = true, bool exclusive = true );

	/**
	 * Removes the references to all objects, not the objects themselves.
	 */
	void clear();

	/**
	 * Read only access to the selected objects.
	 */
	const VObjectList& objects() const { return m_objects; }

	/**
	 * Returns a list of segments that have at least one control point inside the specified rect.
	 *
	 * @param rect the selection rect
	 * @return the list of segments
	 */
	Q3PtrList<VSegment> getSegments( const KoRect& rect );

	/**
	 * Selects or deselects all nodes.
	 *
	 * @param select controls if nodes are selected or deselected
	 */
	void selectNodes( bool select = true );

	/**
	 * Returns the handle node id, the KoPoint is inside.
	 *
	 * @param point the selection point 
	 * @return the handle the point is inside or node_none if point is not inside any node
	 */
	VHandleNode handleNode( const KoPoint &point ) const;

	/**
	 * Toggle selection handles on/off.
	 *
	 * @param handle controls if handle are shown or not
	 */
	void showHandle( bool handle = true ) { m_showhandle = handle; }

	/**
	 * Toggles selection of objects/nodes.
	 *
	 * @param selectObjects controls if objects or nodes are selected
	 */
	virtual void setSelectObjects( bool selectObjects = true ) { m_selectObjects = selectObjects; }

	static void setHandleSize( uint size )
		{ m_handleNodeSize = size; }

	static uint handleSize()
		{ return m_handleNodeSize; }
private:
	/**
	 * Show/Hide handle.
	 */
	bool m_showhandle;

	/**
	 * Select objects and not nodes?
	 */
	bool m_selectObjects;

	/**
	 * The list of currently selected objects.
	 */
	VObjectList m_objects;

	/**
	 * Paint coordinates of handle rectangle and handle nodes.
	 * Used for handle node determination and handle node drawing.
	 */
	KoRect *m_handleRect;

	/**
	 * Paint size of nodes.
	 */
	static uint m_handleNodeSize;
};

#endif

