/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Cedric Pasteur <cedric.pasteur@free.fr>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qstringlist.h>
#include <kdebug.h>
#include "kexiproperty.h"


KexiProperty::KexiProperty(const QString &name, QVariant value)
{
	m_name = name;
	m_value = value;
	m_list = 0;
	m_desc = name;
	kdDebug() << "KexiProperty::KexiProperty(): standard property (" << m_desc << ")" << endl;
}

KexiProperty::KexiProperty(const QString &name, QVariant value, const QStringList &list)
{
	m_name = name;
	m_value = value;
	m_list = new QStringList(list);
	m_desc = name;
	kdDebug() << "creating stringlist property" << endl;
}

KexiProperty::KexiProperty(const QString &name, QVariant value, const QString &desc)
{
	m_name = name;
	m_value = value;
	m_desc = desc;
	m_list = 0;
	kdDebug() << "KexiProperty::KexiProperty(): labeled property (" << m_desc << ")" << endl;
}

/*KexiProperty::KexiProperty(const QString &name, QVariant value, QStringList *list)
{
	m_name = name;
	m_value = value;
	m_list = list;
}*/

KexiProperty::KexiProperty(const KexiProperty &property)
{
	m_name = property.m_name;
	m_value = property.m_value;
	if(property.m_list)
		m_list = new QStringList(*(property.m_list));
	else
		m_list=0;
}

const KexiProperty&
KexiProperty::operator=(const KexiProperty &property)
{
	if(&property!=this)
	{
	if(m_list)
	{
		delete m_list;
		m_list=0;
	}
	
	m_name = property.m_name;
	m_value = property.m_value;
	m_desc = property.m_desc;

	if(property.m_list)
		m_list = new QStringList(*(property.m_list));
	else
		m_list=0;
	}
	return *this;
}

QVariant::Type  KexiProperty::type() const
{
	if(m_list)
		return QVariant::StringList;
	else
		return m_value.type();
}


KexiProperty::~KexiProperty()
{
	if(m_list)
		delete m_list;
}
