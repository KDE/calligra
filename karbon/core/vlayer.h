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
	VLayer( VObject* parent, VState state = normal );
	VLayer( const VLayer& layer );

	virtual ~VLayer();

	virtual void draw( VPainter *painter, const KoRect& rect ) const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VLayer* clone() const;

	virtual void accept( VVisitor& visitor );


	void bringToFront( const VObject& object );

	/// moves the object one step up the list.
	/// When the object is at the top this method has no effect.
	void upwards( const VObject& object );

	/// moves the object one step down the list.
	/// When the object is at the bottom this method has no effect.
	void downwards( const VObject& object );

	void sendToBack( const VObject& object );

	const QString& name() { return m_name; }
	
	bool isLocked() { return m_locked; }
	bool isPrintable() { return m_printable; }
	bool isVisible() { return m_visible; }
	
	void setLocked( bool locked ) { m_locked = locked; }
	void setName( const QString& name ) { m_name = name; }
	void setPrintable( bool printable ) { m_printable = printable; }
	void setVisible( bool visible ) { m_visible = visible; }

private:
	QString m_name;  /// id for the layer
	bool m_locked, m_printable, m_visible; //is layer editable, printable, visible
};

#endif

