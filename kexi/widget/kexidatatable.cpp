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


#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecordset.h"
#include "kexiDB/kexidbupdaterecord.h"
#include "kexiDB/kexidberror.h"
#include "kexiDB/kexidbwatcher.h"

#include "kexidatatable.h"
#include "kexidatatableview.h"
#include "kexitableheader.h"
#include "kexiproject.h"
#include "kexiview.h"
#include "kexiprojecthandleritem.h"

KexiDataTable::KexiDataTable(KexiView *view,QWidget *parent, QString caption, QString identifier, bool embedd)
	: KexiDialogBase(view,parent, identifier.latin1())
{
	init(caption, identifier, embedd);
}

KexiDataTable::KexiDataTable(KexiView *view,QWidget *parent, KexiProjectHandlerItem *item, /*QString caption, const char *name,*/ bool embedd)
	: KexiDialogBase(view,parent, item->fullIdentifier().latin1())
{
	init(item->title(), item->fullIdentifier(), embedd);
}

void
KexiDataTable::init(QString caption, QString identifier, bool embedd)
{
	m_record=0;
	setCaption(i18n("%1 - Table").arg(caption));

	QGridLayout *g = new QGridLayout(this);
	m_tableView = new KexiDataTableView(this);
	m_tableView->m_editOnDubleClick = true;
	m_statusBar = new QStatusBar(this);
#ifndef KEXI_NO_DATATABLE_SEARCH
	// greate linux mess ;)
	m_searchVisible=true;
	m_lSearch = new QLabel(i18n("Search:"), this);
	m_searchCol = new QComboBox(this);
	m_search = new QLineEdit("", this);

	g->addWidget(m_lSearch,			0,	0);
	g->addWidget(m_searchCol,		0,	1);
	g->addWidget(m_search,			0,	2);

	connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(slotSearchChanged(const QString &)));
	connect(m_searchCol, SIGNAL(activated(int)), this, SLOT(slotSerachColChanged(int)));
	connect(m_tableView, SIGNAL(sortedColumnChanged(int)), this, SLOT(slotTableSearchChanged(int)));
#endif

	g->addMultiCellWidget(m_tableView,	1,	1,	0,	2);
	g->addMultiCellWidget(m_statusBar,	2,	2,	0,	2);

//	connect(m_tableView, SIGNAL(itemChanged(KexiTableItem *, int,QVariant)), this, SLOT(slotItemChanged(KexiTableItem *, int,QVariant)));
//	connect(m_tableView, SIGNAL(contextMenuRequested(KexiTableItem *, int, const QPoint &)), this,
//	 SLOT(slotContextMenu(KexiTableItem *, int, const QPoint &)));
	m_tableView->setDeletionPolicy( KexiDataTableView::SignalDelete );
	connect(m_tableView, SIGNAL(currentItemRemoveRequest()), this, SLOT(slotRemoveCurrentRecord()));

	m_db = m_view->project()->db();
	connect(m_db->watcher(), SIGNAL(updated(QObject *, const QString &, const QString &, uint, QVariant &)), this,
	 SLOT(slotUpdated(QObject *, const QString &, const QString &, uint, QVariant &)));
	connect(m_db->watcher(), SIGNAL(removed(QObject *, const QString &, uint)), this,
	 SLOT(slotRemoved(QObject *, const QString &, uint)));

	m_first = true;

	if(!embedd)
		registerAs(DocumentWindow, identifier);
	else
		m_statusBar->hide();

}

void KexiDataTable::setDataSet(KexiDBRecordSet *rec)
{
	m_tableView->setDataSet(rec);
	m_tableView->setDataBase(m_db);
	m_statusBar->message(i18n("%1 records.").arg(m_tableView->records()));

#if 0
	if(!m_first)
		m_tableView->clearAll();

	if (m_record) disconnect(m_record,0,this,0);
	m_record=rec;

	if(!m_record)
	{
		kdDebug() << "KexiDataTable::setDataSet(): record doesn't exist" << endl;
		return;
	}

	connect(m_record,SIGNAL(recordInserted(KexiDBUpdateRecord*)),this,SLOT(recordInsertFinished(KexiDBUpdateRecord*)));
	for(uint i = 0; i < m_record->fieldCount(); i++)
	{
		QVariant defaultval = QVariant("");
		if(m_record->isForignField(i))
		{
			QStringList fdata;
			KexiDBRecordSet *ftr = m_db->queryRecord("SELECT * FROM " + m_db->escapeName(m_record->fieldInfo(i)->table()));
			if(ftr)
			{
				while(ftr->next())
				{
					fdata.append(ftr->value(m_record->fieldName(i)).toString());
				}

				defaultval = QVariant(fdata);
				delete ftr;
			}
		}

		m_tableView->addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly(),
		 defaultval, 100, m_record->fieldInfo(i)->auto_increment());
#ifndef KEXI_NO_DATATABLE_SEARCH
		m_searchCol->insertItem(m_record->fieldName(i));
#endif
	}

	int record = 0;
	while(m_record->next())
	{
//		kdDebug() << "KexiDataTable::setDataSet(): next()" << endl;
		KexiTableItem *it = new KexiTableItem(m_tableView);
		for(uint i = 0; i < m_record->fieldCount(); i++)
		{
//			it->setInsertItem(false);
			it->setValue(i, m_record->value(i));
//			it->setInsertItem(false);
		}
		it->setHint(QVariant(record));
		record++;
	}

	if(!readOnly())
	{
		KexiTableItem *insert = new KexiTableItem(m_tableView);
		insert->setHint(QVariant(record));
		insert->setInsertItem(true);
	}

	//automatically set cursor on 0,0
	m_tableView->setFocus();
	m_tableView->setCursor(0,0);

	/*}
	else {
		m_tableView->selectRow( m_tableView->rows()-1 );
	}*/
	//m_tableView->update();
//rows
	m_first = false;
#endif

}

bool
KexiDataTable::readOnly()
{
	return m_record ? m_record->readOnly() : true;
}

void
KexiDataTable::slotContextMenu(KexiTableItem *i, int, const QPoint &pos)
{
	if (i->isInsertItem()) //avoid delete not inserted item
		return;
	QPopupMenu context;
	context.insertItem(i18n("Delete Record"), this, SLOT(slotRemoveCurrentRecord()));
	context.exec(pos);
}

void
KexiDataTable::slotRemoveCurrentRecord()
{
	if(m_tableView->selectedItem() && !m_tableView->selectedItem()->isInsertItem())
	{
		m_record->deleteRecord(m_tableView->selectedItem()->getHint().toInt());
		m_db->watcher()->remove(this, m_record->fieldInfo(0)->table(), m_tableView->selectedItem()->getHint().toInt());
		m_tableView->remove(m_tableView->selectedItem());
	}
}

#ifndef KEXI_NO_PRINT
void
KexiDataTable::print(KPrinter &printer)
{
	m_tableView->print(printer);
}
#endif


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


KXMLGUIClient *
KexiDataTable::guiClient()
{
	return new TableGUIClient(this);
}


KexiDataTable::~KexiDataTable()
{
	kdDebug()<<"KexiDataTable::~KexiDataTable()"<<endl;
	if (!m_record) kdDebug()<<"m_record == 0"<<endl;
	delete m_record;
}

//GUI client implementation follows...
TableGUIClient::TableGUIClient(KexiDataTable *t)
{
	setXMLFile("kexitableview.rc");
#ifndef KEXI_NO_DATATABLE_SEARCH
	new KToggleAction(i18n("Show incremental search"), 0, t, SLOT(showIS), 0, "showISearch");
#endif
}

TableGUIClient::~TableGUIClient()
{
}

#include "kexidatatable.moc"
