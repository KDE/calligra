/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qptrlist.h>
#include <qstring.h>

#include "vobjectlist.h"

class QDomElement;
class VObject;

/**
 * VLayer manages a set of vobjects. It keeps the objects from bottom to top
 * in a list, ie. objects higher in the list are drawn above lower objects.
 * Objects in a layer can be manipulated and worked on independant of objects
 * in other layers.
 */
class VLayer : public VObject
{
public:
	VLayer();
	~VLayer();

	void draw( VPainter *painter, const KoRect& rect );

	virtual void transform( const QWMatrix& ) {}

	/// appends the object relative to the current position in the object list
	void appendObject( VShape* object );

	/// prepends the object relative to the current position in the object list
	void prependObject( VShape* object );

	/// removes the reference to the object, not the object itself
	void removeRef( const VShape* object );


	/// moves the object one step down the list.
	/// When the object is at the bottom this method has no effect.
	void moveObjectDown( const VShape* object );

	/// moves the object one step up the list.
	/// When the object is at the top this method has no effect.
	void moveObjectUp( const VShape* object );


	/// selects all objects that intersect with rect.
	VObjectList objectsWithinRect( const KoRect& rect ) const;

	/// clean up object list.
	/// All objects with state deleted are removed from the layer
	/// and destroyed.
	void removeDeletedObjects();

	bool visible() const { return m_visible; }
	bool readOnly() const { return m_readOnly; }

	const QString& name() { return m_name; }
	void setName( const QString& name ) { m_name= name; }

	// read-only access to objects:
	const VObjectList& objects() const { return m_objects; }

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	virtual VObject* clone() { return 0L; }

private:
	VObjectList m_objects;	/// all objects in this layer
	QString m_name;			/// id for the layer
	bool m_visible;			/// can we see the layer
	bool m_readOnly;		/// is the layer locked
};

typedef QPtrList<VLayer> VLayerList;
typedef QPtrListIterator<VLayer> VLayerListIterator;

#endif
