/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include <qlistbox.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include <kdebug.h>

#include <kexidb/field.h>
#include <kexidb/fieldlist.h>

#include <kexipartdatasource.h>

#include "kexidatasourcefields.h"

KexiDataSourceFields::KexiDataSourceFields(KexiProject *project, QWidget *parent, const char *name)
 : QWidget(parent, name)
{
	kdDebug() << "*** KexiDataSourceFields::KexiDataSourceFields() ***" << endl;
	m_project = project;
	m_ds = 0;
	m_fields = 0;

	m_avail = new QListBox(this);
	m_used = new QListBox(this);

	QPushButton *addOne = new QPushButton(">", this);
	connect(addOne, SIGNAL(clicked()), this, SLOT(addSelected()));
	QPushButton *addAll = new QPushButton(">>", this);
	connect(addAll, SIGNAL(clicked()), this, SLOT(addAll()));
	QPushButton *takeAll = new QPushButton("<<", this);
	connect(takeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
	QPushButton *takeOne = new QPushButton("<", this);
	connect(takeOne, SIGNAL(clicked()), this, SLOT(removeSelected()));

	QGridLayout *g = new QGridLayout(this);
	g->addMultiCellWidget(m_avail, 0, 5, 0, 0);
	g->addMultiCellWidget(m_used, 0, 5, 2, 2);
	g->addWidget(addAll, 1, 1);
	g->addWidget(addOne, 2, 1);
	g->addWidget(takeOne, 3, 1);
	g->addWidget(takeAll, 4, 1);


}

void
KexiDataSourceFields::setDataSource(KexiPart::DataSource *ds, const KexiPart::Item &it)
{
	kdDebug() << "KexiDataSourceFields::setDataSource()" << endl;
	m_avail->clear();
	m_used->clear();

	KexiDB::FieldList *fl = ds->fields(m_project, it);
	m_fields = fl;

	removeAll();
}

void
KexiDataSourceFields::addSelected()
{
	QListBoxItem *selected = m_avail->item(m_avail->currentItem());
	if(!selected)
		return;

	m_used->insertItem(selected->text());

	delete selected;
	emit listChanged();
}

void
KexiDataSourceFields::removeSelected()
{
	QListBoxItem *selected = m_used->item(m_used->currentItem());
	if(!selected)
		return;

	m_avail->insertItem(selected->text());

	delete selected;
	emit listChanged();
}

void
KexiDataSourceFields::addAll()
{
	m_avail->clear();
	m_used->clear();

	if(!m_fields)
		return;

	for(unsigned int i=0; i < m_fields->fieldCount(); i++)
	{
		m_used->insertItem(m_fields->field(i)->name());
	}

	emit listChanged();
}

void
KexiDataSourceFields::removeAll()
{
	m_avail->clear();
	m_used->clear();

	if(!m_fields)
		return;

	for(unsigned int i=0; i < m_fields->fieldCount(); i++)
	{
		m_avail->insertItem(m_fields->field(i)->name());
	}

	emit listChanged();
}

KexiDB::FieldList *
KexiDataSourceFields::usedFields()
{
	if(!m_fields)
		return 0;

	KexiDB::FieldList *f = new KexiDB::FieldList();
	for(unsigned int i=0; i < m_used->count(); i++)
	{
		f->addField(m_fields->field(m_used->item(i)->text()));
	}

	return f;
}

KexiDataSourceFields::~KexiDataSourceFields()
{
}

#include "kexidatasourcefields.moc"

