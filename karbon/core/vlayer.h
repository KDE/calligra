/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qptrlist.h>
#include <qstring.h>

#include "vobject.h"

class KoRect;

// all vobjects exist inside a layer.

class VLayer
{
public:
	VLayer();
	~VLayer();

	void draw( QPainter& painter, const QRect& rect, const double& zoomFactor );

	void insertObject( const VObject* object );

	VObjectList objectsWithinRect( const KoRect& rect );

	// clean up object list:
	void removeDeletedObjects();

	bool isVisible() const { return m_isVisible; }
	bool isReadOnly() const { return m_isReadOnly; }

	// read-only access to objects:
	const VObjectList& objects() const { return m_objects; }

private:
	VObjectList m_objects;
	QString m_name;
	bool m_isVisible;
	bool m_isReadOnly;
};

typedef QPtrList<VLayer> VLayerList;
typedef QPtrListIterator<VLayer> VLayerListIterator;

#endif
