/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <kmessagebox.h>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>
#include <kexidb/connection.h>

#include <kexiproject.h>
#include <keximainwindow.h>
#include <kexiinternalpart.h>
#include <kexitableview.h>
#include <kexitableitem.h>
#include <kexitableviewdata.h>
#include <kexidragobjects.h>
#include "kexiquerydocument.h"
#include "kexidialogbase.h"
#include "kexidatatable.h"

#include "widget/relations/kexirelationwidget.h"

#define MAX_FIELDS 101 //nice prime number

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiMainWindow *mainWin, QWidget *parent, KexiQueryDocument *doc, const char *name)
 : KexiViewBase(mainWin, parent, name)
{
//	setDirty(true);
	m_conn = mainWin->project()->dbConnection();
	m_doc = doc;

	QSplitter *s = new QSplitter(Vertical, this);
//	KexiInternalPart::createWidgetInstance("relation", win, s, "relation");
	m_relations = new KexiRelationWidget(mainWin, s, "relations");
//	addActionProxyChild( m_view->relationView() );
/*	KexiRelationPart *p = win->relationPart();
	if(p)
		p->createWidget(s, win);*/

	m_dataTable = new KexiDataTable(mainWin, s, "guieditor_dataTable", false);
	m_dataTable->tableView()->setSpreadSheetMode();
	m_dataTable->tableView()->addDropFilter("kexi/field");
	addActionProxyChild(m_dataTable);

	m_data = new KexiTableViewData();
	initTable();
	m_dataTable->tableView()->setData(m_data);
	m_dataTable->tableView()->adjustColumnWidthToContents(-1);

	kdDebug() << "KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor() data = " << m_data << endl;
//	m_table = new KexiTableView(m_data, s, "designer");
	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(s);

	connect(m_dataTable->tableView(), SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));
	restore();
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}

void
KexiQueryDesignerGuiEditor::initTable()
{
	KexiTableViewColumn *col1 = new KexiTableViewColumn(i18n("Field"), KexiDB::Field::Enum);
	m_data->addColumn(col1);

	KexiTableViewColumn *col2 = new KexiTableViewColumn(i18n("Table"), KexiDB::Field::Enum);
	m_data->addColumn(col2);

	KexiTableViewColumn *col3 = new KexiTableViewColumn(i18n("Visible"), KexiDB::Field::Boolean);
	m_data->addColumn(col3);

	KexiDB::Field *f = new KexiDB::Field(i18n("Totals"), KexiDB::Field::Enum);
	QValueVector<QString> totalsTypes;
	totalsTypes.append( i18n("Group by") );
	totalsTypes.append( i18n("Sum") );
	totalsTypes.append( i18n("Average") );
	totalsTypes.append( i18n("Min") );
	totalsTypes.append( i18n("Max") );
	//todo: more like this
	f->setEnumHints(totalsTypes);
	KexiTableViewColumn *col4 = new KexiTableViewColumn(*f);
	m_data->addColumn(col4);

	f= new KexiDB::Field(i18n("Sort"), KexiDB::Field::Enum);
	QValueVector<QString> sortTypes;
	sortTypes.append( i18n("Ascending") );
	sortTypes.append( i18n("Descending") );
	sortTypes.append( i18n("No sorting") );
	f->setEnumHints(sortTypes);
	KexiTableViewColumn *col5 = new KexiTableViewColumn(*f);
	m_data->addColumn(col5);

	KexiTableViewColumn *col6 = new KexiTableViewColumn(i18n("Criteria"), KexiDB::Field::Text);
	m_data->addColumn(col6);

//	KexiTableViewColumn *col7 = new KexiTableViewColumn(i18n("Or"), KexiDB::Field::Text);
//	m_data->addColumn(col7);

	const int columns = m_data->columnsCount();
	for (int i=0; i<MAX_FIELDS; i++) {
//		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
//		buff->insert("primaryKey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
//		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
//		m_fields.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(columns);//3 empty fields
		m_data->append(item);
	}

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
	item->push_back(QVariant(true));
	item->push_back(QVariant());
	m_data->append(item);

	//TODO: this should deffinitly not go here :)
//	m_table->updateContents();

	setDirty(true);
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
	if (!m_doc)
		return 0;
	if(m_doc->schema())
		m_doc->schema()->clear();
	else
		m_doc->setSchema(new KexiDB::QuerySchema());

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
			m_doc->schema()->addTable(t);
		}

		KexiDB::Field *f = new KexiDB::Field();
		f->setName(it->at(1).toString());
		f->setTable(t);

		m_doc->schema()->addField(f);
	}

//	containsRef

	//this is temporary and will later be replaced by a intelligent master-table-finder in
	//KexiDB::Connection::selectStatement()
	m_doc->schema()->setParentTable(m_doc->schema()->tables()->first());
	m_doc->schema()->setStatement("");

	m_doc->setSchema(m_doc->schema());
	return m_doc->schema();
}

void
KexiQueryDesignerGuiEditor::restore()
{
	if(!m_doc || !m_doc->schema())
		return;

	m_dataTable->tableView()->clearData();
	KexiDB::Field::Vector flist = m_doc->schema()->fieldsExpanded();
	for(unsigned int i=0; i < flist.count(); i++)
	{
//		m_dataTable->tableView()->addRow(flist.at(i)->table()->name(), flist.at(i)->name());
	}
}

bool
KexiQueryDesignerGuiEditor::beforeSwitchTo(int mode, bool &cancelled)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::beforeSwitch()" << mode << endl;
	//update the pointer :)
	if (mode==Kexi::DesignViewMode) {
		//todo
	}
	else if (mode==Kexi::DataViewMode) {
		if (!dirty() && parentDialog()->neverSaved()) {
			cancelled=true;
			KMessageBox::information(this, i18n("Cannot switch to data view, because query design is empty.\n"
				"First, please create your design.") );
			return true;
		}
		return true;
	}
	else if (mode==Kexi::TextViewMode) {
		//todo
		return true;
	}

//	schema();
	return false;
}

bool
KexiQueryDesignerGuiEditor::afterSwitchFrom(int /*mode*/, bool &/*cancelled*/)
{
//	restore();
	return true;
}


KexiDB::SchemaData*
KexiQueryDesignerGuiEditor::storeNewData(const KexiDB::SchemaData& sdata)
{
	return 0;
}

bool KexiQueryDesignerGuiEditor::storeData()
{
	return true;
}

#include "kexiquerydesignerguieditor.moc"

