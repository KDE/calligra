/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECTLIST_H__
#define __VOBJECTLIST_H__

#include <qptrlist.h>
#include "vobject.h"

class QRect;

class VObjectList : public QPtrList<VObject>
{
public:
	VObjectList();
	virtual ~VObjectList();

	QRect boundingBox( const double zoomFactor ) const;
};

typedef QPtrListIterator<VObject> VObjectListIterator;

#endif

