/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSELECTION_H__
#define __VSELECTION_H__

#include <koRect.h>

#include <qptrlist.h>

#include "vobject.h"

class QObject;
class QPainter;
class QPoint;
class QRect;

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
class VSelection : public VObject
{
public:
	VSelection( VObject* parent = 0L );
	VSelection( const VSelection& selection );

	virtual ~VSelection();

	void draw( QPainter* painter, double zoomFactor ) const;

	virtual const KoRect& boundingBox() const;

	virtual VSelection* clone() const;


	/**
	 * Removes the reference to the object, not the object itself.
	 */
	void take( VObject& object );

	/**
	 * Adds all object to the selection.
	 */
	void append();

	/**
	 * Adds an object to the selection.
	 */
	void append( VObject* object );

	/**
	 * Adds all objects within rect to the selection.
	 */
	void append( const KoRect& rect );

	/**
	 * Removes the references to all objects, not the objects itselves.
	 */
	void clear();

	/**
	 * Read only access to the selected objects.
	 */
	const VObjectList& objects() const { return m_objects; }


	/**
	 * Returns the node id, the QPoint is inside.
	 */
	VHandleNode node( const QPoint& point ) const;

private:
	/**
	 * A list of selected objects.
	 */
	VObjectList m_objects;

	/**
	 * Handle and nodes paint coordinates.
	 */
	QRect* m_qrect;

	/**
	 * Paint size of nodes.
	 */
	static const uint m_nodeSize = 3;
};

#endif

