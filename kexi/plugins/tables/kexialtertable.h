/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>

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

#ifndef _KEXIALTERTABLE_H_
#define _KEXIALTERTABLE_H_

#include "kexidialogbase.h"
#include "kexiDB/kexidb.h"

/*
class QStringList;

class KStatusBar;

class KexiTableView;
class KexiDBRecord;
class KexiTableItem;

class KexiAlterTable : public KexiDialogBase
{
	Q_OBJECT
	public:
		KexiAlterTable(KexiView *view, QWidget *parent, const QString &table, const char *name=0);

		virtual KXMLGUIClient *guiClient(){return new KXMLGUIClient();}
	protected:
		void initTable();
	protected slots:
		void slotItemChanged(KexiTableItem *i, int col);
	private:
		KexiTableView* m_view;
		KStatusBar* m_statusbar;
		QString m_table;
		QStringList m_fieldnames;
};
*/

class KexiTableView;
class KexiTableItem;
class PropertyEditor;
class PropertyEditorItem;

class KexiAlterTable : public KexiDialogBase
{
	Q_OBJECT
	public:
		KexiAlterTable(KexiView *view, QWidget *parent, const QString &table, const char *name=0);

		virtual KXMLGUIClient *guiClient(){return new KXMLGUIClient();}

	protected:
		void initView();
		void getFields();

	protected slots:
		void changeShownField(KexiTableItem* i, int);

	private:
		QString m_table;
		KexiTableView* m_fieldTable;
		PropertyEditor* m_propList;
		KexiDBTableStruct m_tableFields;
		PropertyEditorItem* m_nameItem;
		PropertyEditorItem* m_datatypeItem;
		PropertyEditorItem* m_lengthItem;
		PropertyEditorItem* m_requiredItem;
		PropertyEditorItem* m_defaultItem;
		PropertyEditorItem* m_unsignedItem;
		PropertyEditorItem* m_precisionItem;
		PropertyEditorItem* m_autoIncItem;
};

#endif
