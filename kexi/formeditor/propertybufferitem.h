/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#ifndef PROPERTYBUFFERITEM_H
#define PROPERTYBUFFERITEM_H

#include <qvariant.h>

class QObject;
class QString;

#define KFORMEDITOR_EXPORT

class PropertyBufferItem
{
	public:
		PropertyBufferItem(QObject *o, const QString &name, QVariant value);
//		PropertyBufferItem();
		~PropertyBufferItem();

		QObject		*object() { return m_object; }
		QString		name() { return m_name; }
		QVariant	value() { return m_value; }

		void setValue(const QVariant &v) { m_value = v; }

	private:
		QObject		*m_object;
		QString		m_name;
		QVariant	m_value;
};

#endif
