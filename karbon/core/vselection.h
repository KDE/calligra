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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VSELECTION_H__
#define __VSELECTION_H__


#include <qptrlist.h>

#include <koRect.h>

#include "vobject.h"
#include "vvisitor.h"
#include <koffice_export.h>
class KoPoint;
class QObject;
class VPainter;
class VVisitor;
class VSegment;

typedef QPtrList<VObject> VObjectList;
typedef QPtrListIterator<VObject> VObjectListIterator;


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
 * VSelection manages a set of vobjects. It keeps the objects from bottom to top
 * in a list, ie. objects higher in the list are drawn above lower objects.
 * Objects in a layer can be manipulated and worked on independant of objects
 * in other layers.
 */
class KARBONBASE_EXPORT VSelection : public VObject, public VVisitor
{
public:
	VSelection( VObject* parent = 0L );
	VSelection( const VSelection& selection );

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
	 * Removes the references to all objects, not the objects themselves.
	 */
	void clear();

	/**
	 * Read only access to the selected objects.
	 */
	const VObjectList& objects() const { return m_objects; }


	QPtrList<VSegment> getSegments( const KoRect& rect );

	/**
	 * Deselects all nodes.
	 */
	void selectNodes( bool select = true );

	/**
	 * Returns the handle node id, the KoPoint is inside.
	 */
	VHandleNode handleNode( const KoPoint &point ) const;

	void showHandle( bool handle = true ) { m_showhandle = handle; }

	virtual void setSelectObjects( bool selectObjects = true ) { m_selectObjects = selectObjects; }

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
	static const uint m_handleNodeSize = 3;
};

#endif

