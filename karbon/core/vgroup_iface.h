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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VGROUP_IFACE_H__
#define __VGROUP_IFACE_H__

#include <q3valuelist.h>

#include "vobject_iface.h"

class VGroup;

//typedef QPtrList<VObject> VObjectList;
//typedef QPtrListIterator<VObject> VObjectListIterator;

class VGroupIface : public VObjectIface
{
	K_DCOP

public:
	VGroupIface( VGroup *group );

k_dcop:
	//void take( const VObject& object );

	//void append( DCOPRef object );

	//virtual void insertInfrontOf( VObject* newObject, VObject* oldObject );

	void clear();
	Q3ValueList<DCOPRef> objects();

private:
	VGroup *m_group;
};

#endif

