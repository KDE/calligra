/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <qptrlist.h>
#include <qstring.h>

#include "vgroup.h"

class QDomElement;


/**
 * VLayer manages a set of vobjects. It keeps the objects from bottom to top
 * in a list, ie. objects higher in the list are drawn above lower objects.
 * Objects in a layer can be manipulated and worked on independant of objects
 * in other layers.
 */
 
class VLayer : public VGroup
{
public:
	VLayer( VObject* parent = 0L, VState state = state_normal );
	VLayer( const VLayer& layer );

	virtual ~VLayer();

	virtual void draw( VPainter *painter, const KoRect& rect ) const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VLayer* clone() const;


	void bringToFront( const VObject& object );

	/// moves the object one step up the list.
	/// When the object is at the top this method has no effect.
	void upwards( const VObject& object );

	/// moves the object one step down the list.
	/// When the object is at the bottom this method has no effect.
	void downwards( const VObject& object );

	void sentToBack( const VObject& object );


	/// selects all objects that intersect with rect.
	VObjectList objectsWithinRect( const KoRect& rect ) const;

	/// clean up object list.
	/// All objects with state deleted are removed from the layer
	/// and destroyed.
	void removeDeletedObjects();

	const QString& name() { return m_name; }
	void setName( const QString& name ) { m_name= name; }

private:
	QString m_name;			/// id for the layer
};

#endif

