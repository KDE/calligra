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

/*#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlcursor.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qsqlindex.h> */
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
#include "kexiDB/kexidbrecord.h"
#include "kexiDB/kexidberror.h"
#include "kexiDB/kexidbwatcher.h"

#include "kexidatatable.h"
#include "kexitableview.h"
#include "kexiproject.h"
#include "kexiview.h"

KexiDataTable::KexiDataTable(KexiView *view,QWidget *parent, QString caption, const char *name, bool embedd)
	: KexiDialogBase(view,parent, name),m_record(0)
{
	QGridLayout *g = new QGridLayout(this);
	m_tableView = new KexiTableView(this);
	m_tableView->m_editOnDubleClick = true;
	m_statusBar = new QStatusBar(this);

	QLabel *lSearch = new QLabel(i18n("Search:"), this);
	m_search = new QLineEdit("", this);
	connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(slotSearchChanged(const QString &)));

	setCaption(i18n("%1 - Table").arg(caption));

	g->addWidget(lSearch,			0,	0);
	g->addWidget(m_search,			0,	1);
	g->addMultiCellWidget(m_tableView,	1,	1,	0,	1);
	g->addMultiCellWidget(m_statusBar,	2,	2,	0,	1);

	connect(m_tableView, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
	connect(m_tableView, SIGNAL(contextMenuRequested(KexiTableItem *, int, const QPoint &)), this,
	 SLOT(slotContextMenu(KexiTableItem *, int, const QPoint &)));

	m_db = view->project()->db();
	connect(m_db->watcher(), SIGNAL(updated(QObject *, const QString &, const QString &, uint, QVariant &)), this,
	 SLOT(slotUpdated(QObject *, const QString &, const QString &, uint, QVariant &)));
	connect(m_db->watcher(), SIGNAL(removed(QObject *, const QString &, uint)), this,
	 SLOT(slotRemoved(QObject *, const QString &, uint)));

	m_first = true;

	if(!embedd)
	{
		registerAs(DocumentWindow);
//		showMaximized();
	}
}

void KexiDataTable::setDataSet(KexiDBRecord *rec)
{
	if(!m_first)
		m_tableView->clearAll();

	m_record=rec;

	if(!m_record)
		kdDebug() << "KexiDataTable::setDataSet(): record doesn't exist" << endl;

	for(uint i = 0; i < m_record->fieldCount(); i++)
	{
		if(!m_record->fieldInfo(i)->auto_increment())
		{
			m_tableView->addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly());
			kdDebug() << "KexiDataTable::setDataSet(): adding usual column" << endl;
		}
		else
		{
			m_tableView->addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly(), QVariant(""), 100, true);
			kdDebug() << "KexiDataTable::setDataSet(): adding auto-inc columns" << endl;
		}
	}

	int record = 0;
	while(m_record->next())
	{
		kdDebug() << "KexiDataTable::setDataSet(): next()" << endl;
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

	if(!m_record->readOnly())
	{
		KexiTableItem *insert = new KexiTableItem(m_tableView);
		insert->setHint(QVariant(record));
		insert->setInsertItem(true);
	}

	m_first = false;

}

bool
KexiDataTable::executeQuery(const QString &queryStatement)
{


	kdDebug() << "KexiDataTable::executeQuery(): executing query..." << endl;
//	m_record = kexiProject()->db()->queryRecord(queryStatement, false);

	try
	{
		m_record = kexiProject()->db()->queryRecord(queryStatement, false);
	}
	catch(KexiDBError &err)
	{
		kdDebug() << "KexiDataTable::executeQuery(): db-error" << endl;
		err.toUser(this);
		return false;
	}

	setDataSet(m_record);
	return true;
}

void
KexiDataTable::slotItemChanged(KexiTableItem *i, int col)
{
	if(i->isInsertItem())
	{
		i->setInsertItem(false);
		i->setHint(QVariant(m_record->insert()));
		m_record->update(i->getHint().toInt(), col, i->getValue(col));
		m_record->commit(i->getHint().toInt(), true);
		KexiDBField *fi = m_record->fieldInfo(col);
		m_db->watcher()->update(this, fi->table(), fi->name(), i->getHint().toUInt(),
		 i->getValue(col));

		/*
		for(uint f=0; f < m_record->fieldCount(); f++)
		{
			if(m_record->fieldInfo(f)->primary_key())
			{
				i->setValue(f, QVariant((unsigned int)m_record->last_id()));
			}
		}
		*/

		KexiTableItem *newinsert = new KexiTableItem(m_tableView);
		newinsert->setHint(QVariant(i->getHint().toInt() + 1));
		newinsert->setInsertItem(true);
	}
	else
	{
		int record = i->getHint().toInt();
		kdDebug() << "KexiDataTable::slotItemChanged(" << record << ")" << endl;
		if(m_record->update(record, col, i->getValue(col)))
		{
			m_record->commit(i->getHint().toInt(), false);
			KexiDBField *fi = m_record->fieldInfo(col);
			m_db->watcher()->update(this, fi->table(), fi->name(), i->getHint().toUInt(),
			 i->getValue(col));
		}
	}

}

void
KexiDataTable::slotUpdated(QObject *sender, const QString &table, const QString &fieldName,
 uint record, QVariant &value)
{
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
				if(item->getHint().toInt() == record)
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
				if(item->getHint().toInt() == record)
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

void
KexiDataTable::print(KPrinter &printer)
{
	m_tableView->print(printer);
}

KexiDataTable::~KexiDataTable()
{
	kdDebug()<<"KexiDataTable::~KexiDataTable()"<<endl;
	if (!m_record) kdDebug()<<"m_record == 0"<<endl;
	delete m_record;
}

#include "kexidatatable.moc"
