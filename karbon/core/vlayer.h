/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qptrlist.h>
#include <qstring.h>
#include "vobject.h"

// Each graphical object lies on a layer.

class VObject;

class VLayer
{
public:
	VLayer();
	~VLayer();

	void draw( QPainter& painter, const QRect& rect, const double& zoomFactor );

	void insertObject( const VObject* object );

	void selectObjects( const QRect &rect, QPtrList<VObject> &list );
	void selectAllObjects();
	void unselectObjects();
	void deleteObjects( QPtrList<VObject> &list );

	const bool& isVisible() const { return m_isVisible; }
	const bool& isReadOnly() const { return m_isReadOnly; }

	// read-only access to objects:
	const QPtrList<VObject>& objects() const { return m_objects; }

private:
	QPtrList<VObject> m_objects;
	QString m_name;
	bool m_isVisible;
	bool m_isReadOnly;
};

#endif
