/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIRELATIONVIEWTABLE_H
#define KEXIRELATIONVIEWTABLE_H

#include <qobject.h>

#include <klistview.h>

#include "kexirelation.h"
#include "kexirelationview.h"

class KexiRelationView;

class KexiRelationViewTable : public KListView
{
	Q_OBJECT

	public:
		KexiRelationViewTable(KexiRelationView *parent, QString table, QStringList fields, const char *name=0);
		~KexiRelationViewTable();

		QString			table() const { return m_table; };
		int			globalY(const QString &item);
		void setReadOnly(bool);

	signals:
		void			tableScrolling(QString);

	protected:
		virtual bool		acceptDrag(QDropEvent *e) const;
		virtual QDragObject	*dragObject();

	protected slots:
		void			slotDropped(QDropEvent *e);
		void			slotContentsMoving(int, int);

	private:
		QStringList		m_fieldList;
		QString			m_table;

		KexiRelationView	*m_parent;
};

#endif
