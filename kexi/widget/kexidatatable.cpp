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

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include "koApplication.h"


#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecordset.h"
#include "kexiDB/kexidbupdaterecord.h"
#include "kexiDB/kexidberror.h"
#include "kexiDB/kexidbwatcher.h"

#include "kexidatatable.h"
#include "kexitableview.h"
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
	m_tableView = new KexiTableView(this);
	m_tableView->m_editOnDubleClick = true;
	m_statusBar = new QStatusBar(this);
#ifndef KEXI_NO_DATATABLE_SEARCH
	QLabel *lSearch = new QLabel(i18n("Search:"), this);
	m_search = new QLineEdit("", this);
	connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(slotSearchChanged(const QString &)));
	g->addWidget(lSearch,			0,	0);
	g->addWidget(m_search,			0,	1);
#endif
	g->addMultiCellWidget(m_tableView,	1,	1,	0,	1);
	g->addMultiCellWidget(m_statusBar,	2,	2,	0,	1);

	connect(m_tableView, SIGNAL(itemChanged(KexiTableItem *, int,QVariant)), this, SLOT(slotItemChanged(KexiTableItem *, int,QVariant)));
//	connect(m_tableView, SIGNAL(contextMenuRequested(KexiTableItem *, int, const QPoint &)), this,
//	 SLOT(slotContextMenu(KexiTableItem *, int, const QPoint &)));
	m_tableView->setDeletionPolicy( KexiTableView::SignalDelete );
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
			KexiDBRecordSet *ftr = m_db->queryRecord("SELECT * FROM `" + m_record->fieldInfo(i)->table() + "`");
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

	m_first = false;
}

bool
KexiDataTable::readOnly()
{
	return m_record ? m_record->readOnly() : true;
}

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
}

void
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
}

void
KexiDataTable::slotSearchChanged(const QString &findQuery)
{
	kdDebug() << "KexiDataTable::slotSearchChanged()" << endl;
	m_tableView->findString(findQuery);
}

void
KexiDataTable::slotContextMenu(KexiTableItem *i, int col, const QPoint &pos)
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

KexiDataTable::~KexiDataTable()
{
	kdDebug()<<"KexiDataTable::~KexiDataTable()"<<endl;
	if (!m_record) kdDebug()<<"m_record == 0"<<endl;
	delete m_record;
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

#include "kexidatatable.moc"
