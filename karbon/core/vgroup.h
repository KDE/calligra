/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGROUP_H__
#define __VGROUP_H__

#include "qptrlist.h"

#include "vobject.h"

class KoRect;
class QDomElement;
class VPainter;


typedef QPtrList<VObject> VObjectList;
typedef QPtrListIterator<VObject> VObjectListIterator;


/**
 * Base class for all sort of VObject conglomerats.
 */

class VGroup : public VObject
{
public:
	VGroup( VObject* parent = 0L, VState state = state_normal );
	VGroup( const VGroup& group );

	virtual ~VGroup();

	virtual void draw( VPainter* painter, const KoRect& rect );

	virtual void transform( const QWMatrix& m );

	virtual const KoRect& boundingBox() const;

	virtual bool isInside( const KoRect& rect ) const;

	virtual void setState( const VState state );

	virtual void setStroke( const VStroke& stroke );
	virtual void setFill( const VFill& fill );

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VObject* clone() const;


	/// removes the reference to the object, not the object itself
	void take( const VObject& object );

	void prepend( VObject* object );
	void append( VObject* object );

	void clear() { kdDebug() << "notimplemnted!!!" << endl; }

	// clear the group without deleting the objects
	void ungroup();	// TODO !!! use clear()?

	// read-only access to objects:
	const VObjectList& objects() const { return m_objects; }

protected:
	VObjectList m_objects;
};

#endif

