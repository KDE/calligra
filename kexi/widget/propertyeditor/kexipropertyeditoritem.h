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

#ifndef KEXIPROPERTYEDITORITEM_H
#define KEXIPROPERTYEDITORITEM_H

#include <klistview.h>
#include <qptrlist.h>
#include <qdict.h>

#include "kexipropertyeditor.h"
#include "kexiproperty.h"

typedef QDict<KexiPropertyEditorItem> ChildDict;

class KEXIPROPERTYEDITOR_EXPORT KexiPropertyEditorItem : public KListViewItem
{
	public:
		KexiPropertyEditorItem(KListView *parent, KexiProperty *property);
		KexiPropertyEditorItem(KexiPropertyEditorItem *parent, KexiProperty *property);
		~KexiPropertyEditorItem();

		const QString	name() { return m_property->name(); }
		QVariant::Type	type() { return m_property->type(); }
		QVariant	value() { return m_value; }
		QVariant	oldValue() { return m_oldvalue; }
		KexiProperty*	property() { return m_property;}

		void		setValue(QVariant value);
		QVariant	getComposedValue();

		static QString	format(const QVariant &s);
		
		bool		modified() { return !(m_value==m_oldvalue);}

	protected slots:
		void childChanged(KexiPropertyEditorItem *item);

	protected:
		virtual void paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align);

	private:
		QVariant	m_value;
		QVariant	m_oldvalue;
		KexiProperty	*m_property;
		QPtrList<KexiProperty>	*m_childprop;
		ChildDict	*m_children;
};

#endif
