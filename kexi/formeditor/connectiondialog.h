/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef FORMCONNECTIONDIALOG_H
#define FORMCONNECTIONDIALOG_H

#include <qintdict.h>
#include <kdialogbase.h>

namespace KexiDB {
	class ResultInfo;
}

class QLabel;
class QButton;
class KexiTableView;
class KexiTableViewData;
class KexiTableItem;

namespace KFormDesigner {

class Form;
class ConnectionBuffer;
class Connection;

class KFORMEDITOR_EXPORT ConnectionDialog : public KDialogBase
{
	Q_OBJECT

	public:
		ConnectionDialog(QWidget *parent);
		~ConnectionDialog() {;}

		void exec(Form *form);

	protected:
		void setStatusOk(KexiTableItem *item = 0);
		void setStatusError(const QString &msg, KexiTableItem *item = 0);
		void initTable();
		void updateTableData();
		void updateSlotList(KexiTableItem *item);
		void updateSignalList(KexiTableItem *item);

	protected slots:
		void slotCellChanged(KexiTableItem*, int, QVariant, KexiDB::ResultInfo*);
		void checkConnection(KexiTableItem *item);

		void newItemByDragnDrop();
		void newItem();
		void removeItem();

		void slotConnectionCreated(Form *form, Connection &connection);
		void slotConnectionAborted(Form *form);

		void slotCellSelected(int col, int row);
		void slotRowInserted(KexiTableItem*,bool);

		virtual void slotOk();

	protected:
		enum {BAdd = 10, BRemove};
		Form    *m_form;
		ConnectionBuffer *m_buffer;
		KexiTableView  *m_table;
		KexiTableViewData  *m_data;
		KexiTableViewData *m_widgetsColumnData, *m_slotsColumnData, *m_signalsColumnData;
		QLabel  *m_pixmapLabel, *m_textLabel;
		QIntDict<QButton>  m_buttons;
};

}

#endif
