/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGROUP_H__
#define __VGROUP_H__

#include "vshape.h"
#include "vobjectlist.h"

class QDomElement;

// grouping of VObjects

class VGroup : public VShape
{
public:
	VGroup( VObject* parent = 0L );
	VGroup( const VObjectList &, VObject* parent = 0L );
	VGroup( const VGroup & );
	~VGroup();

	void draw( VPainter *painter, const KoRect& rect );

	void setState( const VState state );
	// clear the group without deleting the objects
	void ungroup();
	void insertObject( VShape* object );

	virtual void transform( const QWMatrix& m );

    virtual const KoRect& boundingBox() const;

    virtual bool isInside( const KoRect& rect ) const;

    virtual VShape* clone();

	// read-only access to objects:
	const VObjectList& objects() const { return m_objects; }

	virtual void setFill( const VFill &fill );
    virtual void setStroke( const VStroke &stroke );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:
	VObjectList m_objects;
};

#endif
