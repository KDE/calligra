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

#include <qlayout.h>
#include <qstringlist.h>

#include <kstatusbar.h>
#include <klocale.h>
#include <kdebug.h>

#include "kexialtertable.h"
#include "kexitableview.h"
#include "kexiDB/kexidbrecord.h"
#include "kexiapplication.h"

KexiAlterTable::KexiAlterTable(QWidget *parent, QString table, const char *name)
 : KexiDialogBase(parent, name)
{
	m_table = table;
	QVBoxLayout* l = new QVBoxLayout(this);
	m_view = new KexiTableView(this);
	m_view->m_editOnDubleClick = true;
	m_statusbar = new KStatusBar(this);
	l->addWidget(m_view);
	l->addWidget(m_statusbar);
	
	//Cols
	m_view->addColumn(i18n("Field Name"), QVariant::String, true);
	m_view->addColumn(i18n("Datatype"), QVariant::String, true);
	m_view->addColumn(i18n("Length"), QVariant::Int, true);
	m_view->addColumn(i18n("Required"), QVariant::Bool, true);
	m_view->addColumn(i18n("Default Value"), QVariant::String, true);
	m_view->addColumn(i18n("Auto Increment"), QVariant::Bool, true);

	connect(m_view, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
	setCaption(i18n(m_table + " - Table Editor"));
	registerAs(DocumentWindow);
	initTable();
}

void KexiAlterTable::initTable()
{
	KexiDBRecord* record = kexi->project()->db()->queryRecord("select * from " + m_table + " limit 1 ", false);
	record->next();
	m_fieldnames.clear();
	int fc = 0;
	
	for(uint i = 0; i < record->fieldCount(); i++)
	{
		KexiTableItem *it = new KexiTableItem(m_view);
		it->setValue(0, record->fieldInfo(i)->name());
		m_fieldnames.append(record->fieldInfo(i)->name());
		it->setValue(1, record->fieldInfo(i)->sqlType());
		it->setValue(2, record->fieldInfo(i)->length());
		it->setValue(3, record->fieldInfo(i)->not_null());
		it->setValue(4, record->fieldInfo(i)->defaultValue());
		it->setValue(5, record->fieldInfo(i)->auto_increment());
		it->setHint(QVariant(fc++));
	}

	// Insert item
	KexiTableItem *insert = new KexiTableItem(m_view);
	insert->setHint(QVariant(fc));
	insert->setInsertItem(true);
	
	delete record;
}

void KexiAlterTable::slotItemChanged(KexiTableItem *i, int /*col*/)
{
	if(i->isInsertItem())
	{
		i->setInsertItem(false);
	}
	else
	{
		int field = i->getHint().toInt();
		kdDebug() << "KexiAlterTable::slotItemChanged(" << field << ")" << endl;
		bool ok = kexi->project()->db()->alterField(m_table, m_fieldnames[field], i->getValue(0).toString(),
		 static_cast<KexiDBField::ColumnType> (i->getValue(1).toInt()), i->getValue(2).toInt(), i->getValue(3).toBool(),
		 i->getValue(4).toString(), i->getValue(5).toBool());
		
		if(ok)
		{
			m_fieldnames[field] = i->getValue(0).toString();
		}
		else
		{
			// If the query faild revert
			KexiDBRecord* record = kexi->project()->db()->queryRecord("select * from " + m_table + " limit 1 ", false);
			record->next();
			i->setValue(0, record->fieldInfo(field)->name());
			i->setValue(1, record->fieldInfo(field)->sqlType());
			i->setValue(2, record->fieldInfo(field)->length());
			i->setValue(3, record->fieldInfo(field)->not_null());
			i->setValue(4, record->fieldInfo(field)->defaultValue());
			i->setValue(5, record->fieldInfo(field)->auto_increment());
			delete record;
		}
	}
}

#include "kexialtertable.moc"
