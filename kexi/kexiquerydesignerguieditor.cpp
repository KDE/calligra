/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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
#include <qcombobox.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kdebug.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexiapplication.h"
#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexiquerydesignerguieditor.h"
#include "kexirelationview.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiQueryDesigner *parent, const char *name)
 : QWidget(parent, name)
{
	m_db = kexi->project()->db();

	m_tables = new KexiRelationView(this);

	m_tableCombo = new QComboBox(this);
	m_tableCombo->insertStringList(kexi->project()->db()->tables());

	m_addButton = new QPushButton(i18n("&add"), this);
	connect(m_addButton, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_designTable = new KexiTableView(this);
	m_designTable->m_editOnDubleClick = true;
	connect(m_designTable, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));

	m_designTable->viewport()->setAcceptDrops(true);
	m_designTable->addDropFilter("kexi/field");
	
	m_designTable->addColumn(i18n("source"), QVariant::StringList, true, QVariant(kexi->project()->db()->tables()));
	m_designTable->addColumn(i18n("field"), QVariant::String, true);
	m_designTable->addColumn(i18n("shown"), QVariant::Bool, true);
	m_designTable->addColumn(i18n("condition"), QVariant::String, true);
	
	KexiTableItem *i = new KexiTableItem(m_designTable);
	i->setValue(0, 1);
	i->setInsertItem(true);

	QGridLayout *g = new QGridLayout(this);
	g->addWidget(m_tableCombo,		0,	0);
	g->addWidget(m_addButton,		0,	1);
	g->addMultiCellWidget(m_tables,		1,	1,	0,	1);
	g->addMultiCellWidget(m_designTable,	2,	2,	0,	1);
}

void
KexiQueryDesignerGuiEditor::slotAddTable()
{
	KexiDBRecord *r = m_db->queryRecord("select * from " + m_tableCombo->currentText() + " limit 1");
	QStringList fields;
	for(uint i=0; i < r->fieldCount(); i++)
	{
		fields.append(r->fieldInfo(i)->name());
	}
	m_tables->addTable(m_tableCombo->currentText(), fields);
	
	delete r;
	m_tableCombo->removeItem(m_tableCombo->currentItem());
}

void
KexiQueryDesignerGuiEditor::slotDropped(QDropEvent *ev)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::slotDropped()" << endl;
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}


#include "kexiquerydesignerguieditor.moc"
