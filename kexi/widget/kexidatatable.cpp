/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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


#include <qvariant.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpopupmenu.h>
#include <qcombobox.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kaction.h>
#include "koApplication.h"


#include "kexidb/connection.h"
#include "kexidb/cursor.h"
//#include "kexiDB/kexidbupdaterecord.h"

#include "kexidatatable.h"
#include "kexidatatableview.h"
#include "kexitableheader.h"
#include "kexiproject.h"
#include "kexiview.h"
#include "kexiprojecthandleritem.h"

KexiDataTable::KexiDataTable(KexiView *view, QString caption, QString identifier, 
	QWidget *parent, bool embed)
	: KexiDialogBase(view, identifier, parent, identifier.latin1()),m_embed(embed)
{
	init(/*caption, identifier, embedd*/);
	setCustomCaption(caption);
}

KexiDataTable::KexiDataTable(KexiView *view, KexiProjectHandlerItem *item, 
	QWidget *parent, bool embed)
//	: KexiDialogBase(view, parent, item->fullIdentifier().latin1())
	: KexiDialogBase(view, item, parent),m_embed(embed)
{
//	init(item->title(), item->fullIdentifier(), embedd);
	init();
	//item->title(), item->fullIdentifier(), embedd);
}

void
KexiDataTable::init(/*QString caption, QString identifier, bool embedd*/)
{
//	m_record=0;
//	setCaption(i18n("%1 - Table").arg(caption));

//	QGridLayout *g = new QGridLayout(this);
//	g->setMargin(4);
	m_tableView = new KexiDataTableView(this);
	m_tableView->setEditableOnDoubleClick( true );
//js	m_statusBar = new QStatusBar(this);
#ifndef KEXI_NO_DATATABLE_SEARCH
	// greate linux mess ;)
	m_searchVisible=true;
	m_lSearch = new QLabel(i18n("Search:"), this);
	m_searchCol = new QComboBox(this);
	m_search = new QLineEdit("", this);

	gridLayout()->addWidget(m_lSearch,			0,	0);
	gridLayout()->addWidget(m_searchCol,		0,	1);
	gridLayout()->addWidget(m_search,			0,	2);

	connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(slotSearchChanged(const QString &)));
	connect(m_searchCol, SIGNAL(activated(int)), this, SLOT(slotSerachColChanged(int)));
	connect(m_tableView, SIGNAL(sortedColumnChanged(int)), this, SLOT(slotTableSearchChanged(int)));
#endif

	gridLayout()->addMultiCellWidget(m_tableView,	1,	1,	0,	2);
//js	gridLayout()->addMultiCellWidget(m_statusBar,	2,	2,	0,	2);

//	connect(m_tableView, SIGNAL(itemChanged(KexiTableItem *, int,QVariant)), this, SLOT(slotItemChanged(KexiTableItem *, int,QVariant)));
//	connect(m_tableView, SIGNAL(contextMenuRequested(KexiTableItem *, int, const QPoint &)), this,
//	 SLOT(slotContextMenu(KexiTableItem *, int, const QPoint &)));
	m_tableView->setDeletionPolicy( KexiDataTableView::SignalDelete );

	connect(m_tableView, SIGNAL(currentItemRemoveRequest()), this, SLOT(slotRemoveCurrentRecord()));

//	m_db = m_view->project()->db();

//	connect(m_db->watcher(), SIGNAL(updated(QObject *, const QString &, const QString &, uint, QVariant &)), this,
//	 SLOT(slotUpdated(QObject *, const QString &, const QString &, uint, QVariant &)));
//	connect(m_db->watcher(), SIGNAL(removed(QObject *, const QString &, uint)), this,
//	 SLOT(slotRemoved(QObject *, const QString &, uint)));

//	m_first = true;

	if(!m_embed)
		registerAs(DocumentWindow, m_identifier);
//js	else
//js		m_statusBar->hide();

	setXMLFile("kexitableview.rc");
#ifndef KEXI_NO_DATATABLE_SEARCH
	//TODO
//	new KToggleAction(i18n("Show Incremental Search"), 0, t, SLOT(showISearch()), 0, "showISearch");
#endif

}

void KexiDataTable::setDataSet(KexiDB::Cursor *rec)
{
	m_tableView->setDataSet(rec);
// Not yet please. Later special navigating widget will 
// be added in place of status bar:
//	m_statusBar->message(i18n("%1 records.").arg(m_tableView->records()));
	
}

bool
KexiDataTable::readOnly()
{
	return m_tableView->readOnly();
//	return m_record ? m_record->readOnly() : true;
}

/*
bool
KexiDataTable::executeQuery(const QString &queryStatement)
{
	kdDebug() << "KexiDataTable::executeQuery(): executing query: '" << queryStatement << "'" << endl;
	if(queryStatement.isEmpty())
		return false;

	m_record = kexiProject()->db()->queryRecord(queryStatement, true);
	if (!m_record) {
		kdDebug() << "KexiDataTable::executeQuery(): db-error" << endl;
		kexiProject()->db()->latestError()->toUser(this);
		return false;
	}

	kdDebug() << "KexiDataTable::executeQuery(): record: " << m_record << endl;

	setDataSet(m_record);
	return true;
}*/

/*void
KexiDataTable::slotItemChanged(KexiTableItem *i, int col,QVariant oldValue)
{
	if(i->isInsertItem())
	{
		i->setInsertItem(false);
		i->setHint("UPDATING");//;QVariant(m_record->insert()));
		KexiDBUpdateRecord *urec=m_record->insert(true);
		urec->setValue(col,i->getValue(col));
		m_insertMapping.insert(urec,i);

//		m_record->update(i->getHint().toInt(), col, i->getValue(col));

		if ((!m_record->writeOut(urec))) //i->getHint().toInt(), true))
		{
			KMessageBox::detailedError(this, i18n("Error occurred while updating table."), m_record->latestError()->message(),
			 i18n("Database Error"));
//			err.toUser(this);
			return;
		}
//		i->setInsertItem(false);

		KexiDBField *fi = m_record->fieldInfo(col);
		m_db->watcher()->update(this, fi->table(), fi->name(), i->getHint().toUInt(),
		 i->getValue(col));

		KexiTableItem *newinsert = new KexiTableItem(m_tableView);
		newinsert->setHint(QVariant(i->getHint().toInt() + 1));
		newinsert->setInsertItem(true);

		m_tableView->takeInsertItem();
		m_tableView->recordMarker()->setInsertRow(m_tableView->rows());
		m_tableView->setInsertItem(newinsert);
	}
	else
	{


		QMap<QString,QVariant> fnvm;
		for (int c=0;c<m_tableView->cols();c++)
			fnvm[m_tableView->column(c)]=((c==col)?oldValue:i->getValue(c));
		KexiDBUpdateRecord *ur=m_record->update(fnvm);
		if (ur) {
			ur->setValue(col,i->getValue(col));
			m_record->writeOut();
		}
#if 0
		int record = i->getHint().toInt();
		kdDebug() << "KexiDataTable::slotItemChanged(" << record << ")" << endl;
		if(m_record->update(record, col, i->getValue(col)))
		{
			m_record->commit(i->getHint().toInt(), false);
			KexiDBField *fi = m_record->fieldInfo(col);
			m_db->watcher()->update(this, fi->table(), fi->name(), i->getHint().toUInt(),
			 i->getValue(col));
		}
#endif
	}
}

void
KexiDataTable::recordInsertFinished(KexiDBUpdateRecord* ur) {
	kdDebug()<<"KexiDataTable::recordInsertFinished:INSERT FINISHED CALLED"<<endl;
	if (m_insertMapping.contains(ur)) {
		KexiTableItem *it=m_insertMapping[ur];
		m_insertMapping.remove(ur);
		for (int i=0;i<m_tableView->cols();i++)
			it->setValue(i,ur->value(i));
	}
}

void
KexiDataTable::slotUpdated(QObject *sender, const QString &table, const QString &fieldName,
 uint record, QVariant &value)
{
return;
	kdDebug() << "KexiDataTable::slotUpdated() " << this << endl;
	kdDebug() << "KexiDataTable::slotUpdated() table: " << table << endl;
	kdDebug() << "KexiDataTable::slotUpdated() field: " << fieldName << endl;
	kdDebug() << "KexiDataTable::slotUpdated() record: " << record << endl;

	for(uint f=0; f < m_record->fieldCount(); f++)
	{
		KexiDBField *field = m_record->fieldInfo(f);
		if(table == field->table() && fieldName == field->name())
		{
			kdDebug() << "KexiDataTable::slotUpdated(): meta match" << endl;
			for(int i=0; i < m_tableView->rows(); i++)
			{
				KexiTableItem *item = m_tableView->itemAt(i);
				kdDebug() << "KexiDataTable::slotUpdated(): current record:" << item->getHint().toInt() <<
				 " " << item->isInsertItem() << endl;
				if(item->getHint().toUInt() == record)
				{
					kdDebug() << "KexiDataTable::slotUpdated(): record match:" << endl;
					if(!item->isInsertItem())
					{
						item->setValue(f, value);
						m_tableView->updateCell(i, f);
					}
					else
					{
						item->setInsertItem(false);
						item->setValue(f, value);

						KexiTableItem *newinsert = new KexiTableItem(m_tableView);
						newinsert->setHint(QVariant(item->getHint().toInt() + 1));
						newinsert->setInsertItem(true);
					}

				}
			}
		}
	}
}

void
KexiDataTable::slotRemoved(QObject *sender, const QString &table, uint record)
{
	if(sender == this)
		return;

	kdDebug() << "KexiDataTable::slotRemoved()" << endl;
	for(uint f=0; f < m_record->fieldCount(); f++)
	{
		KexiDBField *field = m_record->fieldInfo(f);
		if(table == field->table())
		{
			kdDebug() << "KexiDataTable::slotRemoved(): table match" << endl;

			for(int i=0; i < m_tableView->rows(); i++)
			{
				KexiTableItem *item = m_tableView->itemAt(i);
				if(item->getHint().toUInt() == record)
				{
					kdDebug() << "KexiDataTable::slotRemoved(): record match" << endl;
					m_tableView->remove(m_tableView->itemAt(i));
//					m_tableView->setCursor(i, -1);
//					slotRemoveCurrentRecord();
				}
			}
		}
	}
}*/


void
KexiDataTable::slotRemoveCurrentRecord()
{
#ifndef Q_WS_WIN
#warning FIXME
#endif
#if 0
	//IS IT GOOD WE HAVE THIS HERE? maybe move some to KexiDataTableView?
	if(m_tableView->selectedItem() && !m_tableView->selectedItem()->isInsertItem())
	{
		m_tableView->recordSet()->deleteRecord(m_tableView->selectedItem()->getHint().toInt());
		m_tableView->recordSet()->database()->watcher()->remove(this, m_tableView->recordSet()->fieldInfo(0)->table(), m_tableView->selectedItem()->getHint().toInt());
		m_tableView->remove(m_tableView->selectedItem());
	}
#endif
}

#ifndef KEXI_NO_PRINT
void
KexiDataTable::print(KPrinter &printer)
{
	m_tableView->print(printer);
}
#endif

KexiDataTable::~KexiDataTable()
{
	kdDebug()<<"KexiDataTable::~KexiDataTable()"<<endl;
//	if (!m_record) kdDebug()<<"m_record == 0"<<endl;
//	delete m_record;
}

/*! Sets focus on:
 - first row for read-only table
 - last (new) row for r-w table
*/
void
KexiDataTable::setFocus()
{
	m_tableView->setFocus();
	if (readOnly()) {
		m_tableView->selectRow(0);
	}
	else {
		m_tableView->selectRow( m_tableView->rows()-1 );
	}
}

void
KexiDataTable::slotSerachColChanged(int index)
{
//thanks to the german trains i
//could extend search function while driving to
//LinuxTag
// -- cosider that as a diary entry ;)
	m_tableView->setSorting(index);
#ifndef KEXI_NO_DATATABLE_SEARCH
	m_search->setText("");
	m_search->setFocus();
#endif
}

void
KexiDataTable::slotSearchChanged(const QString &findQuery)
{
#ifndef KEXI_NO_DATATABLE_SEARCH
	kdDebug() << "KexiDataTable::slotSearchChanged()" << endl;
	if(m_tableView->sorting() != m_searchCol->currentItem())
		m_tableView->setSorting(m_searchCol->currentItem());
	m_tableView->findString(findQuery);
#endif
}

void
KexiDataTable::slotTableSearchChanged(int col)
{
#ifndef KEXI_NO_DATATABLE_SEARCH
	//i have to say programming in trains is a joy!

	m_searchCol->setCurrentItem(col);
#endif
}

void
KexiDataTable::setSearchVisible(bool visible)
{
#ifndef KEXI_NO_DATATABLE_SEARCH
	if(visible)
	{
		m_lSearch->show();
		m_searchCol->show();
		m_search->show();
	}
	else
	{
		m_lSearch->hide();
		m_searchCol->hide();
		m_search->hide();
	}

	m_searchVisible = visible;
#endif
}

/*
KXMLGUIClient *
KexiDataTable::guiClient()
{
	return new TableGUIClient(this);
}

//GUI client implementation follows...
TableGUIClient::TableGUIClient(KexiDataTable *t)
{
	setXMLFile("kexitableview.rc");
#ifndef KEXI_NO_DATATABLE_SEARCH
	new KToggleAction(i18n("Show Incremental Search"), 0, t, SLOT(showISearch()), 0, "showISearch");
#endif
}

TableGUIClient::~TableGUIClient()
{
}*/

#include "kexidatatable.moc"
