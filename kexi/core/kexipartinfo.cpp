/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

namespace KexiPart
{

Info::Info(KService::Ptr ptr, Manager *manager)
{
	m_ptr = ptr;
	m_manager = manager;
	m_instance = 0;
	m_broken = false;
}

const QString
Info::groupName()
{
	return m_ptr->name();
}

const QString
Info::mime()
{
	return m_ptr->property("X-Kexi-TypeMime").toString();
}

const QString
Info::groupIcon()
{
	return m_ptr->property("X-Kexi-GroupIcon").toString();
}

const QString
Info::itemIcon()
{
	return m_ptr->property("X-Kexi-ItemIcon").toString();
}

const QString
Info::objectName()
{
	return m_ptr->property("X-Kexi-TypeName").toString();
}

int
Info::projectPartID()
{
	if(objectName() == "table")
		return KexiDB::TableObjectType;
	else if(objectName() == "query")
		return KexiDB::QueryObjectType;
	else
		return m_projectPartID;
}

Part *
Info::instance()
{
	if(m_instance)
		return m_instance;

	m_instance = m_manager->load(this);
	return m_instance;
}

Info::~Info()
{
}

}

