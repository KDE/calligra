/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qptrlist.h>
#include <qstring.h>

#include "vobject.h"
#include "vobjectlist.h"

class QDomElement;

// all vobjects exist inside a layer.

class VLayer
{
public:
	VLayer();
	~VLayer();

	void draw( VPainter *painter, const KoRect& rect );

	void insertObject( const VObject* object );
	void prependObject( const VObject* object );
	void removeRef( const VObject* object );
	void moveObjectDown( const VObject* object );
	void moveObjectUp( const VObject* object );

	VObjectList objectsWithinRect( const KoRect& rect,
		const double zoomFactor ) const;

	// clean up object list:
	void removeDeletedObjects();

	bool visible() const { return m_visible; }
	bool readOnly() const { return m_readOnly; }

	const QString& name() { return m_name; }
	void setName( const QString& name ) { m_name= name; }

	// read-only access to objects:
	const VObjectList& objects() const { return m_objects; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VObjectList m_objects;
	QString m_name;
	bool m_visible;
	bool m_readOnly;
};

typedef QPtrList<VLayer> VLayerList;
typedef QPtrListIterator<VLayer> VLayerListIterator;

#endif
