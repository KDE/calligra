/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>

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

#ifndef KEXIDATATABLEVIEW_H
#define KEXIDATATABLEVIEW_H

#include "kexitableview.h"

class KexiDB;
class KexiDBRecordSet;
class KexiTableItem;
class QVariant;

/**
 * database aware table widget
 */
class KexiDataTableView : public KexiTableView
{
	Q_OBJECT

	public:
		/**
		 * creates a blank widget
		 */
		KexiDataTableView(QWidget *parent, const char *name =0);
		/**
		 * creates a table widget and fills it (partly ;) with recordset rec
		 */
		KexiDataTableView(QWidget *parent, const char *name, KexiDBRecordSet *rec);
		~KexiDataTableView();

		/**
		 * fills a table with recordset rec and clears it before, if there already
		 * was data.
		 */
		void		setDataSet(KexiDBRecordSet *rec);

		/**
		 * @returns the number of records in the recordset
		 * @note not all of the records have to be processed
		 */
		int		records() { return m_records; }

		#ifndef KEXI_NO_PRINT
//		virtual void print(KPrinter &printer);
		#endif

	protected:
		void		init();
		virtual QSize	tableSize() const;
		void		appendInsertItem();

	protected slots:
		void		recordInsertFinished(KexiDBUpdateRecord*);
		void		slotMoving(int);
		void		insertNext();

		void		slotItemChanged(KexiTableItem*, int, QVariant);

	private:
		//db stuff
		KexiDBRecordSet	*m_record;
		bool		m_first;
		KexiDB		*m_db;
		int		m_maxRecord;
		int		m_records;

		QMap<KexiDBUpdateRecord*,KexiTableItem*> m_insertMapping;
};

#endif
