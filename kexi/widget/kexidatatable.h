/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include "kexidialogbase.h"

class KexiTableItem;
class QStatusBar;
class QLineEdit;
class QComboBox;
class QLabel;

class KPrinter;

class KexiDBUpdateRecord;
class KexiView;
class KexiDataTableView;
class KexiProjectHandlerItem;
class KexiDataTable;

namespace KexiDB {
	class Cursor;
}

/*class TableGUIClient : public KXMLGUIClient
{
	public:
		TableGUIClient(KexiDataTable *t);
		~TableGUIClient();

};
*/

class KEXIEXTWIDGETS_EXPORT KexiDataTable : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiDataTable(KexiView *view, QString caption, QString identifier, QWidget *parent = 0, bool embedd=false);
		KexiDataTable(KexiView *view, KexiProjectHandlerItem *item, QWidget *parent = 0, bool embedd=false);
		~KexiDataTable();

//		bool executeQuery(const QString &query);
		void setDataSet(KexiDB::Cursor *rec);

		bool readOnly();

# ifndef KEXI_NO_DATATABLE_SEARCH
		bool isSarchVisible() { return m_searchVisible; }
# endif

# ifndef KEXI_NO_PRINT
		virtual void print(KPrinter &printer);
# endif

	public slots:
		virtual void setFocus();
		void setSearchVisible(bool visible);

	protected slots:
		void init(/*QString caption, QString identifier, bool embedd*/);
//		void slotItemChanged(KexiTableItem *i, int col, QVariant oldValue);

		void slotRemoveCurrentRecord();
//		void slotUpdated(QObject *sender, const QString &table, const QString &field,
//		 uint record, QVariant &value);
//		void slotRemoved(QObject *sender, const QString &table, uint record);

//		void recordInsertFinished(KexiDBUpdateRecord*);

		void slotSearchChanged(const QString &);
		void slotSerachColChanged(int index);
		void slotTableSearchChanged(int col);

	private:
		//gui
		KexiDataTableView	*m_tableView;
		QStatusBar	*m_statusBar;
		bool m_embed;
# ifndef KEXI_NO_DATATABLE_SEARCH
		//search stuff
		QLineEdit	*m_search;
		QComboBox	*m_searchCol;
		QLabel		*m_lSearch;
		bool		m_searchVisible;
# endif

		//db stuff
//		KexiDB::Cursor	*m_record;

//		bool		m_first;
//		KexiDB		*m_db;
//		QMap<KexiDBUpdateRecord*,KexiTableItem*> m_insertMapping;
};

#endif
