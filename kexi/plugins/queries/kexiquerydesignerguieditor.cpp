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

#include "kexiquerydesignerguieditor.h"

#include <qlayout.h>
#include <qsplitter.h>

#include <kdebug.h>
#include <klocale.h>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>
#include <kexidb/connection.h>

#include <kexiproject.h>
#include <keximainwindow.h>
#include <kexiinternalpart.h>
#include <kexitableview.h>
#include <kexitableitem.h>
#include <kexitableviewdata.h>
#include "kexidragobjects.h"

#include "widget/relations/kexirelationwidget.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
{
	m_conn = mainWin->project()->dbConnection();

	QSplitter *s = new QSplitter(Vertical, this);
//	KexiInternalPart::createWidgetInstance("relation", win, s, "relation");
	m_relations = new KexiRelationWidget(mainWin, s, "relations");
//	addActionProxyChild( m_view->relationView() );
/*	KexiRelationPart *p = win->relationPart();
	if(p)
		p->createWidget(s, win);*/

	m_data = new KexiTableViewData();
	initTable();
	kdDebug() << "KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor() data = " << m_data << endl;
	m_table = new KexiTableView(m_data, s, "designer");
	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(s);
	m_table->addDropFilter("kexi/field");

	connect(m_table, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));
	m_table->setNavigatorEnabled(false);
}

void
KexiQueryDesignerGuiEditor::initTable()
{
	KexiDB::Field *f = new KexiDB::Field(i18n("Table"), KexiDB::Field::Text);
	KexiTableViewColumn *col = new KexiTableViewColumn(*f);
	m_data->addColumn(col);

	KexiDB::Field *f2 = new KexiDB::Field(i18n("Field"), KexiDB::Field::Text);
	KexiTableViewColumn *col2 = new KexiTableViewColumn(*f2);
	m_data->addColumn(col2);

	KexiDB::Field *f3 = new KexiDB::Field(i18n("Shown"), KexiDB::Field::Boolean);
	f3->setDefaultValue(QVariant(true));
	KexiTableViewColumn *col3 = new KexiTableViewColumn(*f3);
	m_data->addColumn(col3);

	KexiDB::Field *f4 = new KexiDB::Field(i18n("Condition"), KexiDB::Field::Text);
	KexiTableViewColumn *col4 = new KexiTableViewColumn(*f4);
	m_data->addColumn(col4);
}

KexiRelationWidget *KexiQueryDesignerGuiEditor::relationView() const
{
	return m_relations;
}

void
KexiQueryDesignerGuiEditor::addRow(const QString &tbl, const QString &field)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::addRow(" << tbl << ", " << field << ")" << endl;
	KexiTableItem *item = new KexiTableItem(0);

//	 = QVariant(tbl);
	item->push_back(QVariant(tbl));
	item->push_back(QVariant(field));
	item->push_back(QVariant(false));
	item->push_back(QVariant());
	m_data->append(item);

	//TODO: this should deffinitly not go here :)
	m_table->updateContents();
}

void
KexiQueryDesignerGuiEditor::slotDropped(QDropEvent *ev)
{
	//TODO: better check later if the source is really a table
	QString srcTable;
	QString srcField;
	QString dummy;

	KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
	addRow(srcTable, srcField);
}

KexiDB::QuerySchema *
KexiQueryDesignerGuiEditor::schema()
{
	KexiDB::QuerySchema *s = new KexiDB::QuerySchema();
	QDict<KexiDB::TableSchema> tables;
	for(KexiTableItem *it = m_data->first(); it; it = m_data->next())
	{
		QString tableName = it->at(0).toString();

		if(tableName.isEmpty() || it->at(1).toString().isEmpty())
			continue;

		KexiDB::TableSchema *t = tables[it->at(0).toString()];
		if(!t)
		{
			t = m_conn->tableSchema(tableName);
			tables.insert(tableName, t);
			s->addTable(t);
		}

		KexiDB::Field *f = new KexiDB::Field();
		f->setName(it->at(1).toString());
		f->setTable(t);

		s->addField(f);
	}

//	containsRef

	//this is temporary and will later be replaced by a intelligent master-table-finder in
	//KexiDB::Connection::selectStatement()
	s->setParentTable(s->tables()->first());
	return s;
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}

#include "kexiquerydesignerguieditor.moc"

