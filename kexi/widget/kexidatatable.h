/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include "kexidialogbase.h"

class KexiTableItem;
class QStatusBar;
class QLineEdit;

class KPrinter;

class KexiDBRecordSet;
class KexiView;
class KexiTableView;
class KexiDB;

class KexiDataTable : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiDataTable(KexiView *View,QWidget *parent, QString content, const char *name=0, bool embedd=false);
		~KexiDataTable();

		bool executeQuery(const QString &query);
		void setDataSet(KexiDBRecordSet *rec);

		virtual KXMLGUIClient *guiClient(){return new KXMLGUIClient();}
		virtual void print(KPrinter &printer);

	protected slots:
		void slotItemChanged(KexiTableItem *i, int col);
		void slotContextMenu(KexiTableItem *i, int col, const QPoint &pos);

		void slotRemoveCurrentRecord();
		void slotUpdated(QObject *sender, const QString &table, const QString &field,
		 uint record, QVariant &value);
		void slotRemoved(QObject *sender, const QString &table, uint record);

		void slotSearchChanged(const QString &);

	private:
		KexiTableView	*m_tableView;
		QStatusBar	*m_statusBar;
		QLineEdit	*m_search;

		KexiDBRecordSet	*m_record;

		bool		m_first;
		KexiDB		*m_db;
};

#endif
