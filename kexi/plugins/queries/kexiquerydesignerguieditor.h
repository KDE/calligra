/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <qguardedptr.h>
#include <qsplitter.h>

#include <kexiviewbase.h>

class KexiMainWindow;
class KexiTableViewData;
class KexiDataTable;
class KexiTableItem;
class KexiRelationWidget;
class KexiSectionHeader;

namespace KexiPart
{
	class Item;
}

namespace KexiDB
{
	class QuerySchema;
	class Connection;
};
class KexiQueryDocument;

class KexiQueryDesignerGuiEditor : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiQueryDesignerGuiEditor(KexiMainWindow *mainWin, QWidget *parent, KexiQueryDocument *doc, const char *name = 0);
		~KexiQueryDesignerGuiEditor();

		KexiDB::QuerySchema	*schema();

		KexiRelationWidget *relationView() const;

		virtual QSize sizeHint() const;

	protected:
		void			initTable();
		void			addRow(const QString &tbl, const QString &field);
		void			restore();
		virtual bool beforeSwitchTo(int mode, bool &cancelled);
		virtual bool afterSwitchFrom(int mode, bool &cancelled);

		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata);
		virtual bool storeData();

	protected slots:
		void slotDragOverTableRow(KexiTableItem *item, int row, QDragMoveEvent* e);
		void slotDroppedAtRow(KexiTableItem *item, int row, QDropEvent *ev);

	private:
		KexiTableViewData *m_data;
		KexiDataTable *m_dataTable;
		QGuardedPtr<KexiDB::Connection> m_conn;
		KexiRelationWidget *m_relations;
		KexiQueryDocument *m_doc;
		KexiSectionHeader *m_head;
		QSplitter *m_spl;
};

#endif

