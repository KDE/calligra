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

#ifndef PROPERTYEDITORITEM_H
#define PROPERTYEDITORITEM_H

#include <klistview.h>

#include "propertyeditor.h"

class QStringList;

class KFORMEDITOR_EXPORT PropertyEditorItem : public KListViewItem
{
	public:
		PropertyEditorItem(KListView *parent, const QString& name, QVariant::Type type, QVariant value, QObject *o = 0L);
		PropertyEditorItem(PropertyEditorItem *parent, const QString& name, QVariant value);
		PropertyEditorItem(KListView *parent, const QString& name, QVariant::Type type, QVariant value, QStringList l);
		~PropertyEditorItem();

		const QString	name() { return m_name; }
		QVariant::Type	type() { return m_type; }
		QVariant	value() { return m_value; }
		QStringList	list() { return m_list; }
		QObject		*object() { return m_object; }

		void		setValue(QVariant value, bool sync=false);

		static QString	format(const QVariant &s);

	protected:
		virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);

	private:
		QString		m_name;
		QVariant::Type	m_type;
		QVariant	m_value;
		QObject		*m_object;
		QStringList m_list;
};

#endif
