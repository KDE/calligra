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

#include "kexialtertabledialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>

#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include <kexipropertybuffer.h>
#include <kexiproperty.h>
#include "kexipropertyeditor.h"

#include "kexitableview.h"

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
	KexiDB::TableSchema &table, const char *name)
 : KexiViewBase(win, parent, name)
{
	m_table = &table;
	m_constraints.resize(101);
	m_row = 0;
	init();
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
}

void KexiAlterTableDialog::init()
{
	KexiTableViewData *data = new KexiTableViewData();
	data->setInsertingEnabled( false );
	data->addColumn( new KexiTableViewColumn(i18n("Field name"), KexiDB::Field::Text) );
//js TODO: COMBO
	KexiDB::Field *f = new KexiDB::Field(i18n("Data type"), KexiDB::Field::Enum);
	KexiDB::FieldTypeNames t;
	f->setEnumHints(t);

	data->addColumn( new KexiTableViewColumn(*f) );
	data->addColumn( new KexiTableViewColumn(i18n("Comments"), KexiDB::Field::Text) );

/*	KexiTableItem *item = new KexiTableItem(0);
	item->push_back(QVariant("name"));
	item->push_back(QVariant("Text"));
	item->push_back(QVariant(""));
	data->append(item);
*/
	for(unsigned int i=0; i < m_table->fieldCount(); i++)
	{
		KexiDB::Field *field = m_table->field(i);
		KexiTableItem *item = new KexiTableItem(0);
		item->push_back(QVariant(field->name()));
		item->push_back(QVariant(field->type()));
		item->push_back(QVariant(""));
		data->append(item);

		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
		buff->insert("pkey", KexiProperty("pkey", QVariant(field->isPrimaryKey(), 4), i18n("Primary Key")));
		int len = field->length();
		if(len == 0)
			len = field->precision();

		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
		m_constraints.insert(i, buff);
	}

	for (int i=m_table->fieldCount(); i<40; i++) {
		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
		buff->insert("pkey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
		m_constraints.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(3);
		data->append(item);
	}

	QSplitter *splitter = new QSplitter(Vertical, this);

	kdDebug() << "KexiAlterTableDialog::init(): vector contains " << m_constraints.size() << " items" << endl;

	m_view = new KexiTableView(data, splitter, "tableview");
	m_view->setNavigatorEnabled(false);
	m_view->setSortingEnabled(false);//no, sorting is not good idea here
	m_view->adjustColumnWidthToContents(0); //adjust column width
	m_view->adjustColumnWidthToContents(1); //adjust column width
	m_view->setColumnStretchEnabled( true, 2 ); //last column occupies the rest of the area
	setFocusProxy(m_view);
	

	connect(m_view, SIGNAL(cellSelected(int,int)), this, SLOT(slotCellSelected(int,int)));
/*	//! before closing - we'are accepting editing
	connect(this,SIGNAL(closing()),m_view,SLOT(acceptRowEdit()));

	//! updating actions on start/stop editing
	connect(m_view, SIGNAL(rowEditStarted(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(rowEditTerminated(int)), this, SLOT(slotUpdateRowActions(int)));

*/
	m_properties = new KexiPropertyEditor(splitter);
	m_properties->setBuffer(m_constraints.at(0));

	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(splitter);
	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	resize( preferredSizeHint( m_view->sizeHint() ) );
	m_view->setFocus();
	initActions();
}

void
KexiAlterTableDialog::initActions()
{
/*
	plugSharedAction("edit_delete_row", m_view, SLOT(deleteCurrentRow()));
	plugSharedAction("edit_delete_row", m_view->popup());
	m_view->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

	plugSharedAction("edit_delete",m_view, SLOT(deleteAndStartEditCurrentCell()));
	m_view->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

	plugSharedAction("data_save_row",m_view, SLOT(acceptRowEdit()));
	m_view->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut

	slotCellSelected( m_view->currentColumn(), m_view->currentRow() );
	*/
}

QWidget* KexiAlterTableDialog::mainWidget() 
{
	return m_view;
}

QSize KexiAlterTableDialog::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view->minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiAlterTableDialog::sizeHint() const
{
	return m_view->sizeHint();
}

// update actions --------------


void KexiAlterTableDialog::slotCellSelected(int, int row)
{
	kdDebug() << "KexiAlterTableDialog::slotCellSelected()" << endl;
	if(row == m_row)
		return;

	m_properties->setBuffer(m_constraints.at(row));
	m_row = row;
}

bool KexiAlterTableDialog::beforeSwitchTo(int)
{
	KexiDB::TableSchema *nt = new KexiDB::TableSchema(m_table->name());
	nt->setCaption(m_table->caption());

	KexiTableViewData *data = m_view->data();
	int i=0;
	for(KexiTableItem *it = data->first(); it; it = data->next())
	{
		if(!it->at(0).toString().isEmpty())
		{
			KexiDB::Field *f = new KexiDB::Field(nt);
			f->setName(it->at(0).toString());
			f->setType((KexiDB::Field::Type)it->at(1).toInt());
			f->setPrimaryKey(m_constraints.at(i)->find("pkey").data().value().toBool());

			nt->addField(f);
		}
		i++;
	}

	KexiDB::TableSchema *s = mainWin()->project()->dbConnection()->tableSchema(m_table->name());
	if(!s)
	{
		KexiDB::TableSchema *ts = mainWin()->project()->dbConnection()->tableSchema("kexi__objects");
		mainWin()->project()->dbConnection()->dropTable(m_table->name());
		mainWin()->project()->dbConnection()->createTable(nt);
	}
/*	else
	{
		KexiDB::Cursor *cursor = mainWin()->project()->dbConnection()->executeQuery(*s, KexiDB::Cursor::Buffered);
		mainWin()->project()->dbConnection()->dropTable(m_table->name());
		mainWin()->project()->dbConnection()->createTable(nt);
		for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
		{
			
		}
		mainWin()->project()->dbConnection()->deleteCursor(cursor);
	}*/

	return true;
}

/*
void KexiAlterTableDialog::slotUpdateRowActions(int row)
{
	setAvailable("edit_delete_row", !m_view->isReadOnly() && !(m_view->isInsertingEnabled() && row==m_view->rows()) );
	setAvailable("data_save_row", m_view->rowEditing());
}*/


#include "kexialtertabledialog.moc"

