/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VOBJECT_H__
#define __VOBJECT_H__

#include "vrect.h"
#include "vtool.h"

class QPainter;

class VAffineMap;
class VCommand;

// The base class for all karbon objects.

class VObject
{
public:
	VObject()
		: m_isDeleted( false ) {}
	virtual ~VObject() {}

	virtual void draw( QPainter& painter, const QRect& rect,
		const double zoomFactor ) = 0;

	virtual VCommand* accept( VTool& tool ) { return tool.manipulate( this ); }

	virtual VObject& transform( const VAffineMap& affMap ) = 0;

	const VRect& boundingBox() const { return m_boundingBox; }

	// the "m_isDeleted" flag is needed e.g to be able to undo deletion of objects:
	void setDeleted( bool flag = true ) { m_isDeleted = flag; }
	bool isDeleted() { return m_isDeleted; }

protected:
	VRect m_boundingBox;

private:
	bool m_isDeleted;
};

#endif
