/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGROUP_H__
#define __VGROUP_H__

#include <qptrlist.h>

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
	VGroup( VObject* parent, VState state = normal );
	VGroup( const VGroup& group );

	virtual ~VGroup();

	virtual void draw( VPainter* painter, const KoRect& rect ) const;

	virtual void transform( const QWMatrix& m, bool selectedSubObjects = false );

	virtual const KoRect& boundingBox() const;

	virtual void setStroke( const VStroke& stroke );
	virtual void setFill( const VFill& fill );

	virtual void setState( const VState state );

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

	virtual VGroup* clone() const;

	virtual void accept( VVisitor& visitor );


	/**
	 * Removes the reference to the object, not the object itself.
	 */
	void take( const VObject& object );

	/**
	 * Appends a new object.
	 */
	void append( VObject* object );

	/**
	 * This function is important for undo/redo. It insert newObject in front
	 * of oldObject and hide oldObject.
	 */
	void insertNewDeleteOld( VObject* newObject, VObject* oldObject );

	/**
	 * Clears the group, without destroying the grouped objects.
	 */
	void clear();

	/**
	 * Read only access to the grouped objects.
	 */
	const VObjectList& objects() const { return m_objects; }

protected:
	VObjectList m_objects;
};

#endif

