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


KexiProperty::KexiProperty(const QString &name, QVariant value, const QString &desc)
{
	m_name = name;
	m_value = value;
	m_changed = false;
	m_visible = true;
	m_desc = desc;
	m_list = 0;
	m_autosync = -1;
	if (!m_desc.isEmpty())
		kdDebug() << "KexiProperty::KexiProperty(): labeled property (" << m_desc << ")" << endl;
}

KexiProperty::KexiProperty(const QString &name, QVariant value, const QStringList &list, const QString &desc)
{
	m_name = name;
	m_value = value;
	m_changed = false;
	m_visible = true;
	m_list = new QStringList(list);
	m_desc = name;
	m_autosync = -1;
	//kdDebug() << "creating stringlist property" << endl;
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
	m_changed = property.m_changed;
	m_visible = property.m_visible;
	m_desc = property.m_desc;
	m_autosync = property.m_autosync;
	if(property.m_list)
		m_list = new QStringList(*(property.m_list));
	else
		m_list=0;
}

KexiProperty::KexiProperty()
{
	m_name="";
//	m_value=QVariant();
	m_changed = false;
	m_list=0;
}

KexiProperty::~KexiProperty()
{
	if(m_list)
		delete m_list;
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
	m_changed = property.m_changed;
	m_desc = property.m_desc;
	m_autosync = property.m_autosync;

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

void KexiProperty::setValue(const QVariant &v, bool saveOldValue)
{
	if (saveOldValue) {
		if (!m_changed) {
			m_oldValue = m_value; //store old
		}
		m_changed = true;
	}
	else {
		m_oldValue = QVariant(); //clear old
		m_changed = false;
	}
	m_value = v;
}

void KexiProperty::setChanged(bool set)
{
	if (m_changed==set)
		return;
	m_changed=set;
	if (!m_changed)
		m_oldValue = QVariant();
	else
		m_oldValue = m_value; //store
}
