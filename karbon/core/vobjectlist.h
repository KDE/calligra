/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VOBJECTLIST_H__
#define __VOBJECTLIST_H__

#include <qptrlist.h>
#include "vshape.h"

class KoRect;

class VObjectList : public QPtrList<VShape>
{
public:
	VObjectList();
	virtual ~VObjectList();

	const KoRect& boundingBox() const;
	bool boundingBoxIsInvalid() const { return m_boundingBoxIsInvalid; }
	void invalidateBoundingBox() { m_boundingBoxIsInvalid = true; }

private:
	mutable KoRect m_boundingBox;
	mutable bool m_boundingBoxIsInvalid;
};

typedef QPtrListIterator<VShape> VObjectListIterator;

#endif

