/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VGROUP_IFACE_H__
#define __VGROUP_IFACE_H__

#include <qptrlist.h>

#include <dcopobject.h>
#include <dcopref.h>

class VGroup;

//typedef QPtrList<VObject> VObjectList;
//typedef QPtrListIterator<VObject> VObjectListIterator;


class VGroupIface : public DCOPObject
{
	K_DCOP

public:
	VGroupIface( VGroup *group );

k_dcop:
	//void take( const VObject& object );

	//void append( VObject* object );

	//virtual void insertInfrontOf( VObject* newObject, VObject* oldObject );

	void clear();

	//const VObjectList& objects() const { return m_objects; }

private:
	VGroup *m_group;
};

#endif

