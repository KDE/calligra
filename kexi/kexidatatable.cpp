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

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexiapplication.h"

#include "kexidatatable.h" 
 
KexiDataTable::KexiDataTable(QWidget *parent, QString caption, const char *name)
	: KexiDialogBase(parent, name)
{
	QGridLayout *g = new QGridLayout(this);
	m_tableView = new KexiTableView(this);
	m_tableView->m_editOnDubleClick = true;
	m_statusBar = new QStatusBar(this);

	QLabel *lSearch = new QLabel("Search: ", this);
	m_search = new QLineEdit("", this);
	connect(m_search, SIGNAL(textChanged(const QString &)), this, SLOT(slotSearchChanged(const QString &)));

	setCaption(i18n(caption + " - table"));

	g->addWidget(lSearch,			0,	0);
	g->addWidget(m_search,			0,	1);
	g->addMultiCellWidget(m_tableView,	1,	1,	0,	1);
	g->addMultiCellWidget(m_statusBar,	2,	2,	0,	1);

	connect(m_tableView, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
}

bool
KexiDataTable::executeQuery(QString queryStatement)
{
	kdDebug() << "KexiDataTable::executeQuery(): executing query..." << endl;
	m_record = kexi->project()->db()->queryRecord(queryStatement, false);
	
	for(uint i = 0; i < m_record->fieldCount(); i++)
	{
		if(!m_record->fieldInfo(i)->auto_increment())
		{
			m_tableView->addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly());
			kdDebug() << "KexiDataTable::executeQuery(): adding usual column" << endl;
		}
		else
		{
			m_tableView->addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly(), 100, true);
			kdDebug() << "KexiDataTable::executeQuery(): adding auto-inc columns" << endl;
		}
	}

	int record = 0;
	while(m_record->next())
	{
		kdDebug() << "KexiDataTable::executeQuery(): next()" << endl;
		KexiTableItem *it = new KexiTableItem(m_tableView);
		for(uint i = 0; i < m_record->fieldCount(); i++)
		{
//			it->setInsertItem(false);
			kdDebug() << "KexiDataTable::executeQuery(): value()" << endl;
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
		
		for(uint f=0; f < m_record->fieldCount(); f++)
		{
			if(m_record->fieldInfo(f)->primary_key())
			{
				i->setValue(f, QVariant((unsigned int)m_record->last_id()));
			}
		}

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
		}
	}
	
}

void
KexiDataTable::slotSearchChanged(const QString &findQuery)
{
	kdDebug() << "KexiDataTable::slotSearchChanged()" << endl;
	m_tableView->findString(findQuery);
}

KexiDataTable::~KexiDataTable()
{
}

#include "kexidatatable.moc"
