/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/global.h>
#include "kexipartinfo.h"

using namespace KexiPart;

Info::Info(KService::Ptr ptr) //, Manager *manager)
{
	m_ptr = ptr;
	m_idStoredInPartDatabase = false;

	m_groupName = m_ptr->name();
	m_mime = m_ptr->property("X-Kexi-TypeMime").toCString();
//	m_groupIcon = m_ptr->property("X-Kexi-GroupIcon").toString();
	m_itemIcon = m_ptr->property("X-Kexi-ItemIcon").toString();
	m_objectName = m_ptr->property("X-Kexi-TypeName").toString();

	if(m_ptr->property("X-Kexi-NoObject").toInt() == 1)
		m_addTree = false;
	else
		m_addTree = true;

	//! @todo (js)..... now it's hardcoded!
	if(objectName() == "table")
		m_projectPartID = KexiDB::TableObjectType;
	else if(objectName() == "query")
		m_projectPartID = KexiDB::QueryObjectType;
#ifdef OOPL_VERSION //tmp
	else if(objectName() == "html")
		m_projectPartID = KexiDB::LastObjectType+1; //4?
#endif
	else
		m_projectPartID = -1; //TODO!!

	m_broken = false;
}

Info::~Info()
{
}

//--------------

QCString KexiPart::nameForCreateAction(const Info& info)
{
	return (info.objectName()+"part_create").latin1();
}
