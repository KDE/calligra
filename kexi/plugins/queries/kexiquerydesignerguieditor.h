/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIQUERYDESIGNERGUIEDITOR_H
#define KEXIQUERYDESIGNERGUIEDITOR_H

#include <qwidget.h>

class KexiMainWindow;
class KexiTableViewData;
class KexiTableView;

namespace KexiDB
{
	class QuerySchema;
	class Connection;
};

class KexiQueryDesignerGuiEditor : public QWidget
{
	Q_OBJECT

	public:
		KexiQueryDesignerGuiEditor(QWidget *parent, KexiMainWindow *win);
		~KexiQueryDesignerGuiEditor();

		KexiDB::QuerySchema	*schema();

	protected:
		void			initTable();
		void			addRow(const QString &tbl, const QString &field);

	protected slots:
		void			slotDropped(QDropEvent *ev);

	private:
		KexiTableViewData	*m_data;
		KexiTableView		*m_table;
		KexiDB::Connection	*m_conn;
};

#endif

