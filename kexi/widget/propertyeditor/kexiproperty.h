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

#ifndef KEXIPROPERTY_H
#define KEXIPROPERTY_H

#include <qvariant.h>

class QObject;
class QString;
class QStringList;

class KEXIPROPERTYEDITOR_EXPORT KexiProperty
{
	public:
		KexiProperty(const QString &name, QVariant value);
		KexiProperty(const QString &name, QVariant value, const QString &desc);
		KexiProperty(const QString &name, QVariant value, const QStringList &list);
		KexiProperty(const KexiProperty &property);
		//KexiProperty(const QString &name, QVariant value, QStringList *list);
		KexiProperty() { m_name=""; m_value=QVariant(0);m_list=0;}
		~KexiProperty();
		
		const KexiProperty& operator=(const KexiProperty &property);

		QString		name() const { return m_name; }
		QVariant	value() const { return m_value; }
		QString		desc() const { return m_desc; }
		QVariant::Type  type() const;
		QStringList*	list() const{ return m_list;}

		void setValue(const QVariant &v) { m_value = v; }

	private:
		QString		m_name;
		QString		m_desc;
		QVariant	m_value;
		QStringList	*m_list;
};

#endif
