/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIALTERTABLEDIALOG_H
#define KEXIALTERTABLEDIALOG_H

#include <qptrvector.h>
#include <kexiviewbase.h>

class KPopupMenu;
class KexiMainWindow;
class KexiTableView;
class KexiPropertyEditor;
class KexiPropertyBuffer;

typedef QPtrVector<KexiPropertyBuffer> Constraints;

namespace KexiDB
{
	class Cursor;
	class TableSchema;
}

class KexiAlterTableDialog : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, KexiDB::TableSchema &table, 
			const char *name = 0);
		~KexiAlterTableDialog();

		virtual QWidget* mainWidget();
//		KexiDataTableView* tableView() const { return m_view; }

		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;


	protected:
		void init();
		void initActions();

		virtual bool beforeSwitchTo(int mode);

		virtual KexiPropertyBuffer *propertyBuffer();

	protected slots:
		void slotCellSelected(int col, int row);
//		void slotUpdateRowActions(int row);

	private:
		KexiTableView *m_view;
		KexiDB::TableSchema *m_table;
//		KexiPropertyEditor *m_properties;
		Constraints m_constraints;
		int m_row;
};

#endif

