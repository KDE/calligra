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
#include "kexiDB/kexidbfield.h"
#include "kexiDB/kexidb.h"
#include "kexiproject.h"

KexiAlterTable::KexiAlterTable(KexiView *view, QWidget *parent, const QString &table, const char *name)
 : KexiDialogBase(view,parent, name)
{
	m_table = table;
	QVBoxLayout* l = new QVBoxLayout(this);
	m_view = new KexiTableView(this);
	m_view->m_editOnDubleClick = true;
	m_statusbar = new KStatusBar(this);
	l->addWidget(m_view);
	l->addWidget(m_statusbar);

	//Cols
	kdDebug() << "KexiAlterTable::KexiAlterTable(...): Add the columns to the tableview" << endl;
	m_view->addColumn(i18n("Field Name"), QVariant::String, true);

	QStringList strings;

	for(int i = 1; i < 20; i++)
	{
		strings.append(KexiDBField::typeName(static_cast<KexiDBField::ColumnType>(i)));
	}

	m_view->addColumn(i18n("Datatype"), QVariant::StringList, true, QVariant(strings));
	m_view->addColumn(i18n("Length"), QVariant::Int, true);
	m_view->addColumn(i18n("Required"), QVariant::Bool, true);
	m_view->addColumn(i18n("Default Value"), QVariant::String, true);
	m_view->addColumn(i18n("Unsigned"), QVariant::Bool, true);
	m_view->addColumn(i18n("Decimals"), QVariant::Int, true);
	m_view->addColumn(i18n("Auto Increment"), QVariant::Bool, true);

	connect(m_view, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
	setCaption(i18n("%1 - Table Editor").arg(m_table));
	registerAs(DocumentWindow);
	kdDebug() << "Ready" << endl;
	initTable();
}

void KexiAlterTable::initTable()
{
	KexiDBTableStruct dbStruct = kexiProject()->db()->getStructure(m_table);
	m_fieldnames.clear();
	int fc = 0;
	
	for(KexiDBField* field = dbStruct.first(); field; field = dbStruct.next())
	{
		KexiTableItem *it = new KexiTableItem(m_view);
		it->setValue(0, field->name());
		m_fieldnames.append(field->name());
		it->setValue(1, field->sqlType() - 1);
		it->setValue(2, field->length());
		it->setValue(3, field->not_null());
		it->setValue(4, field->defaultValue());
		it->setValue(5, field->unsignedType());
		it->setValue(6, field->precision());
		it->setValue(7, field->auto_increment());
		it->setHint(QVariant(fc++));
	}

	// Insert item
	KexiTableItem *insert = new KexiTableItem(m_view);
	insert->setValue(1, KexiDBField::SQLVarchar - 1);
	insert->setValue(2, 1);
	insert->setHint(QVariant(fc));
	insert->setInsertItem(true);
}

void KexiAlterTable::slotItemChanged(KexiTableItem *i, int /*col*/)
{
	KexiDBField::ColumnConstraints constraints = KexiDBField::None;
	
	if(i->getValue(3).toBool())
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::NotNull);
	}
	
	if(i->getValue(7).toBool())
	{
		constraints = static_cast<KexiDBField::ColumnConstraints>(constraints + KexiDBField::AutoInc);
	}
	
	if(i->isInsertItem())
	{
		if(i->getValue(0).toString() != "" && i->getValue(1).toInt() != 0)
		{
			kdDebug() << "Create new field!" << endl;
			bool ok = kexiProject()->db()->createField(m_table, i->getValue(0).toString(),
				static_cast<KexiDBField::ColumnType>(i->getValue(1).toInt() + 1), i->getValue(2).toInt(),
				i->getValue(6).toInt(), constraints, i->getValue(5).toBool(), false, i->getValue(4).toString());

			if(ok)
			{
				kdDebug() << "New field created!" << endl;
				i->setInsertItem(false);
				m_fieldnames.append(i->getValue(0).toString());
				KexiTableItem *newinsert = new KexiTableItem(m_view);
				newinsert->setValue(1, KexiDBField::SQLVarchar - 1);
				newinsert->setValue(2, 1);
				newinsert->setHint(QVariant(i->getHint().toInt() + 1));
				newinsert->setInsertItem(true);
			}
		}
	}
	else
	{
		int field = i->getHint().toInt();
		kdDebug() << "KexiAlterTable::slotItemChanged(" << field << ")" << endl;
		bool ok = kexiProject()->db()->alterField(m_table, m_fieldnames[field],
			i->getValue(0).toString(), static_cast<KexiDBField::ColumnType> (i->getValue(1).toInt() + 1),
			i->getValue(2).toInt(), i->getValue(6).toInt(), constraints, i->getValue(5).toBool(), false,
			i->getValue(4).toString());

		if(ok)
		{
			m_fieldnames[field] = i->getValue(0).toString();
		}
		else
		{
			// If the query faild revert
			KexiDBTableStruct dbStruct = kexiProject()->db()->getStructure(m_table);
			KexiDBField* f = dbStruct.at(field);
			i->setValue(0, f->name());
			i->setValue(1, f->sqlType() - 1);
			i->setValue(2, f->length());
			i->setValue(3, f->not_null());
			i->setValue(4, f->defaultValue());
			i->setValue(5, f->unsignedType());
			i->setValue(6, f->auto_increment());
		}
	}
}

#include "kexialtertable.moc"
