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
#include <qpainter.h>
#include <qdom.h>

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
#include "kexiquerypart.h"
#include "kexidialogbase.h"
#include "kexidatatable.h"
#include "kexi_utils.h"
#include "kexisectionheader.h"
#include "kexitableviewpropertybuffer.h"
#include "widget/relations/kexirelationwidget.h"
#include "widget/relations/kexirelationviewtable.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(
	KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiViewBase(mainWin, parent, name)
{
	m_conn = mainWin->project()->dbConnection();
//	m_doc = doc;
	m_droppedNewItem = 0;

	m_spl = new QSplitter(Vertical, this);
#if (QT_VERSION >= 0x030200) //TMP 
	m_spl->setChildrenCollapsible(false);
#endif
//	KexiInternalPart::createWidgetInstance("relation", win, s, "relation");
	m_relations = new KexiRelationWidget(mainWin, m_spl, "relations");
	connect(m_relations, SIGNAL(tableAdded(KexiDB::TableSchema&)),
		this, SLOT(slotTableAdded(KexiDB::TableSchema&)));
	connect(m_relations, SIGNAL(tableHidden(KexiDB::TableSchema&)),
		this, SLOT(slotTableHidden(KexiDB::TableSchema&)));
	connect(m_relations, SIGNAL(tableFieldDoubleClicked(KexiDB::TableSchema*,const QString&)),
		this, SLOT(slotTableFieldDoubleClicked(KexiDB::TableSchema*,const QString&)));

//	addActionProxyChild( m_view->relationView() );
/*	KexiRelationPart *p = win->relationPart();
	if(p)
		p->createWidget(s, win);*/

	m_head = new KexiSectionHeader(i18n("Query columns"), Vertical, m_spl);
//	s->setResizeMode(m_head, QSplitter::KeepSize);
	m_dataTable = new KexiDataTable(mainWin, m_head, "guieditor_dataTable", false);
	m_dataTable->tableView()->setSpreadSheetMode();
//	m_dataTable->tableView()->addDropFilter("kexi/field");
//	setFocusProxy(m_dataTable);

	m_data = new KexiTableViewData(); //just empty data
	m_buffers = new KexiTableViewPropertyBuffer( this, m_dataTable->tableView() );
	initTableColumns();
	initTableRows();
	m_dataTable->tableView()->setData(m_data);

//	m_buffers = new KexiTableViewPropertyBuffer( this, m_dataTable->tableView() );

//	//last column occupies the rest of the area
//	m_dataTable->tableView()->setColumnStretchEnabled( true, m_data->columnsCount()-1 ); 
//	m_dataTable->tableView()->setColumnStretchEnabled(true, 0);
//	m_dataTable->tableView()->setColumnStretchEnabled(true, 1);
//	m_dataTable->tableView()->adjustHorizontalHeaderSize();
	QValueList<int> c;
	c << 0 << 1 << 4;
	m_dataTable->tableView()->maximizeColumnsWidth( c ); 
//	m_dataTable->tableView()->adjustColumnWidthToContents(-1);
	m_dataTable->tableView()->adjustColumnWidthToContents(2);//'visible'
	m_dataTable->tableView()->setDropsAtRowEnabled(true);
	connect(m_dataTable->tableView(), SIGNAL(dragOverRow(KexiTableItem*,int,QDragMoveEvent*)),
		this, SLOT(slotDragOverTableRow(KexiTableItem*,int,QDragMoveEvent*)));
	connect(m_dataTable->tableView(), SIGNAL(droppedAtRow(KexiTableItem*,int,QDropEvent*,KexiTableItem*&)),
		this, SLOT(slotDroppedAtRow(KexiTableItem*,int,QDropEvent*,KexiTableItem*&)));
	connect(m_data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowInserted(KexiTableItem*,uint)), 
		this, SLOT(slotRowInserted(KexiTableItem*,uint)));
	connect(m_relations, SIGNAL(tablePositionChanged(KexiRelationViewTableContainer*)),
		this, SLOT(slotTablePositionChanged(KexiRelationViewTableContainer*)));
	connect(m_relations, SIGNAL(aboutConnectionRemove(KexiRelationViewConnection*)),
		this, SLOT(slotAboutConnectionRemove(KexiRelationViewConnection*)));

//	m_table = new KexiTableView(m_data, s, "designer");
	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(m_spl);

	addChildView(m_relations);
//	addActionProxyChild(m_relations);
	setViewWidget(m_relations);
	addChildView(m_dataTable);
//	addActionProxyChild(m_dataTable);
//	restore();
	m_relations->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_head->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	updateGeometry();
//	m_spl->setResizeMode(m_relations, QSplitter::KeepSize);
//	m_spl->setResizeMode(m_head, QSplitter::FollowSizeHint);
	m_spl->setSizes(QValueList<int>()<< 800<<400);

/* moved to afterSwitchFrom
	if (!m_dialog->neverSaved()) {
		QTimer::singleShot(1,this,SLOT(loadLayout()));
	}
	*/
//	setDirty(false);
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}

void
KexiQueryDesignerGuiEditor::initTableColumns()
{
	KexiTableViewColumn *col1 = new KexiTableViewColumn(i18n("Field"), KexiDB::Field::Enum);

	QValueList<QVariant> empty_list;
	m_fieldColumnData = new KexiTableViewData( empty_list, empty_list,
		KexiDB::Field::Text, KexiDB::Field::Text);
	col1->setRelatedData( m_fieldColumnData );
	m_data->addColumn(col1);

	KexiTableViewColumn *col2 = new KexiTableViewColumn(i18n("Table"), KexiDB::Field::Enum);
	m_tablesColumnData = new KexiTableViewData( empty_list, empty_list,
		KexiDB::Field::Text, KexiDB::Field::Text);
	col2->setRelatedData( m_tablesColumnData );
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

/*TODO
f= new KexiDB::Field(i18n("Sort"), KexiDB::Field::Enum);
	QValueVector<QString> sortTypes;
	sortTypes.append( i18n("Ascending") );
	sortTypes.append( i18n("Descending") );
	sortTypes.append( i18n("No sorting") );
	f->setEnumHints(sortTypes);
	KexiTableViewColumn *col5 = new KexiTableViewColumn(*f);
	m_data->addColumn(col5);*/

	KexiTableViewColumn *col6 = new KexiTableViewColumn(i18n("Criteria"), KexiDB::Field::Text);
	m_data->addColumn(col6);

//	KexiTableViewColumn *col7 = new KexiTableViewColumn(i18n("Or"), KexiDB::Field::Text);
//	m_data->addColumn(col7);
}

void KexiQueryDesignerGuiEditor::initTableRows()
{
	m_data->clear();
	const int columns = m_data->columnsCount();
	for (int i=0; i<(int)m_buffers->size(); i++) {
//		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
//		buff->insert("primaryKey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
//		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
//		m_fields.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(columns);
		m_data->append(item);
	}

	updateColumsData();
}

void KexiQueryDesignerGuiEditor::updateColumsData()
{
//	m_fieldColumnData
	m_dataTable->tableView()->acceptRowEdit();

	QStringList sortedTableNames;
	for (TablesDictIterator it(*m_relations->tables());it.current();++it)
		sortedTableNames += it.current()->table()->name();
	qHeapSort( sortedTableNames );

	//several tables can be hidden now, so remove rows for these tables
	QValueList<int> rowsToDelete;
	for (int r = 0; r<(int)m_buffers->size(); r++) {
		KexiPropertyBuffer *buf = m_buffers->at(r);
		if (buf) {
			QString tableName = (*buf)["table"]->value().toString();
			if (sortedTableNames.end() == qFind( sortedTableNames.begin(), sortedTableNames.end(), tableName )) {
				//table not found: mark this line for later remove
				rowsToDelete += r;
//				buf->add(new KexiProperty("_rm", 0) );
			}
		}
	}
	m_data->deleteRows( rowsToDelete );

	//update 'table' and 'field' columns
	m_tablesColumnData->clear();
	m_fieldColumnData->clear();

	for (QStringList::Iterator it = sortedTableNames.begin(); it!=sortedTableNames.end(); ++it) {
		//table
		KexiDB::TableSchema *table = m_relations->tables()->find(*it)->table();
//	for (TablesDictIterator it(*m_relations->tables());it.current();++it) {
		KexiTableItem *item = new KexiTableItem(2);
		(*item)[0]=table->name();
		(*item)[1]=(*item)[0];
		m_tablesColumnData->append( item );
		//field
		item = new KexiTableItem(2);
		(*item)[0]=table->name()+".*";
		(*item)[1]=(*item)[0];
		m_fieldColumnData->append( item );
		for (KexiDB::Field::ListIterator t_it = table->fieldsIterator();t_it.current();++t_it) {
			item = new KexiTableItem(2);
			(*item)[0]=table->name()+"."+t_it.current()->name();
			(*item)[1]=QString("  ") + t_it.current()->name();
			m_fieldColumnData->append( item );
		}
	}
//TODO
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

/*KexiDB::QuerySchema *
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

	//this is temporary and will later be replaced by a intelligent master-table-finder in
	//KexiDB::Connection::selectStatement()
	m_doc->schema()->setParentTable(m_doc->schema()->tables()->first());
	m_doc->schema()->setStatement("");

	m_doc->setSchema(m_doc->schema());
	return m_doc->schema();
}*/

/*void
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
}*/

KexiQueryPart::TempData *
KexiQueryDesignerGuiEditor::tempData() const
{	
	return static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
}

static QString msgCannotSwitch_EmptyDesign() { 
	return i18n("Cannot switch to data view, because query design is empty.\n"
		"First, please create your design.");
}

bool
KexiQueryDesignerGuiEditor::buildSchema(QString *errMsg)
{
	//build query schema
	KexiQueryPart::TempData * temp = tempData();
	if (temp->query)
		temp->query->clear();
	else
		temp->query = new KexiDB::QuerySchema();

	//add tables
	for (TablesDictIterator it(*m_relations->tables()); it.current(); ++it) {
		temp->query->addTable( it.current()->table() );
	}

	//add relations (looking for connections)
	for (ConnectionListIterator it(*m_relations->connections()); it.current(); ++it) {
		KexiRelationViewTableContainer *masterTable = it.current()->masterTable();
		KexiRelationViewTableContainer *detailsTable = it.current()->detailsTable();

		temp->query->addRelationship(
			masterTable->table()->field(it.current()->masterField()),
			detailsTable->table()->field(it.current()->detailsField()) );
	}

	//add fields
	bool fieldsFound = false;
	for (int i=0; i<(int)m_buffers->size(); i++) {
		KexiPropertyBuffer *buf = m_buffers->at(i);
		if (buf) {
			KexiDB::TableSchema *t = m_conn->tableSchema((*buf)["table"]->value().toString());
			if (!t) {
				kdWarning() << "query designer: NO TABLE '" << (*buf)["table"]->value().toString() << "'" << endl;
				continue;
			}
			QString fieldName = (*buf)["field"]->value().toString();
			bool fieldVisible = (*buf)["visible"]->value().toBool();
			if (fieldName=="*") {
				temp->query->addAsterisk( new KexiDB::QueryAsterisk( temp->query, t ), fieldVisible );
				fieldsFound = true;
			}
			else {
				KexiDB::Field *f = t->field( fieldName );
				if (!f) {
					kdWarning() << "query designer: NO FIELD '" << fieldName << "'" << endl;
					continue;
				}
				temp->query->addField(f, fieldVisible);
				fieldsFound = true;
			}
		}
	}
	if (!fieldsFound) {
		if (errMsg)
			*errMsg = msgCannotSwitch_EmptyDesign();
		return false;
	}
	//TODO
	return true;
}

bool
KexiQueryDesignerGuiEditor::beforeSwitchTo(int mode, bool &cancelled, bool &dontStore)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::beforeSwitch()" << mode << endl;
	if (mode==Kexi::DesignViewMode) {
		//todo
		return true;
	}
	else if (mode==Kexi::DataViewMode) {
		if (!dirty() && parentDialog()->neverSaved()) {
			cancelled=true;
			KMessageBox::information(this, msgCannotSwitch_EmptyDesign());
			return true;
		}
		//remember current design in a temporary structure
		dontStore=true;
		QString errMsg;
		//build schema; problems are not allowed
		if (!buildSchema(&errMsg)) {
			cancelled=true;
			KMessageBox::information(this, errMsg);
			return false;
		}
		//TODO
		return true;
	}
	else if (mode==Kexi::TextViewMode) {
		dontStore=true;
		//build schema; ignore problems
		buildSchema();
		if (tempData()->query && tempData()->query->fieldCount()==0) {
			//no fields selected: let's add "*" (all-tables asterisk), 
			// otherwise SQL statement will be invalid
			tempData()->query->addAsterisk( new KexiDB::QueryAsterisk( tempData()->query ) );
		}
		//todo
		return true;
	}

	return false;
}

bool
KexiQueryDesignerGuiEditor::afterSwitchFrom(int mode, bool &cancelled)
{
//	restore();
	if (mode==Kexi::NoViewMode) {
		//this is not a SWITCH but fresh opening in this view mode
		if (!m_dialog->neverSaved()) {
			if (!loadLayout()) {
				//err msg
				parentDialog()->setStatus(i18n("Query definition loading failed."), i18n("Query data may be corrupted."));
				return false;
			}
			//todo: load columns
			//todo: load global query properties
		}
	}
	else if (mode==Kexi::TextViewMode) {
		if (tempData()->queryChangedInPreviousView) {
			//previous view changed query data
			//-clear and regenerate GUI items
			m_relations->clear();
			initTableRows();
			//todo
			if (tempData()->query) {
				//there is a query schema to show
				//-show tables:
				//(todo: instead of hiding all tables and showing some tables, 
				// show only these new and hide these unncecessary; the same for connections)
				KexiDB::QuerySchema *q = tempData()->query;
				for (KexiDB::TableSchema::ListIterator it(*q->tables()); it.current(); ++it) {
					m_relations->addTable( it.current() );
				}
				//-show relationships:
				KexiDB::Relationship *rel;
				for (KexiDB::Relationship::ListIterator it(*q->relationships()); (rel=it.current()); ++it) {
					SourceConnection conn;
//@todo: now only sigle-field relationships are implemented!
					KexiDB::Field *masterField = rel->masterIndex()->fields()->first();
					KexiDB::Field *detailsField = rel->detailsIndex()->fields()->first();
					conn.masterTable = masterField->table()->name(); //<<<TODO
					conn.masterField = masterField->name();
					conn.detailsTable = detailsField->table()->name();
					conn.detailsField = detailsField->name();
					m_relations->addConnection( conn );
				}
			}
		}

		//todo: generate layout
		//todo: load global query properties
	}
	else if (mode==Kexi::DataViewMode) {
	}
	tempData()->queryChangedInPreviousView = false;
	return true;
}


KexiDB::SchemaData*
KexiQueryDesignerGuiEditor::storeNewData(const KexiDB::SchemaData& sdata)
{
	KexiQueryPart::TempData * temp = tempData();
	buildSchema();
	KexiDB::QuerySchema *query = temp->query;
	temp->query = 0; //will be returend, so: don't keep it
	(KexiDB::SchemaData&)*query = sdata; //copy main attributes

	if (!m_mainWin->project()->dbConnection()
			->storeObjectSchemaData( *query, true /*newObject*/ ))
	{
		delete query;
		return 0;
	}
	m_dialog->setId( query->id() );

	//serialize detailed XML query definition
	QString xml = "<query_layout>", tmp;
	for (TablesDictIterator it(*m_relations->tables()); it.current(); ++it) {
		KexiRelationViewTableContainer *table_cont = it.current();
		tmp = QString("<table name=\"")+table_cont->table()->name()+"\" x=\""
		 +QString::number(table_cont->x())
		 +"\" y=\""+QString::number(table_cont->y())
		 +"\" width=\""+QString::number(table_cont->width())
		 +"\" height=\""+QString::number(table_cont->height())
		 +"\"/>";
		xml += tmp;
	}

	KexiRelationViewConnection *con; 
	for (ConnectionListIterator it(*m_relations->connections()); (con = it.current()); ++it) {
		tmp = QString("<conn mtable=\"") + con->masterTable()->table()->name() 
			+ "\" mfield=\"" + con->masterField() + "\" dtable=\"" + con->detailsTable()->table()->name() 
			+ "\" dfield=\"" + con->detailsField() + "\"/>";
		xml += tmp;
	}
	xml += "</query_layout>";
	if (!storeDataBlock( xml, "query_layout" )) {
		delete query;
		return 0;
	}
	
	return query;
}

bool KexiQueryDesignerGuiEditor::loadLayout()
{
	QString xml;
	if (!loadDataBlock( xml, "query_layout" )) {
		//TODO errmsg
		return false;
	}
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement doc_el = doc.documentElement(), el;
	if (doc_el.tagName()!="query_layout") {
		//TODO errmsg
		return false;
	}

	const bool was_dirty = dirty();

	//add tables
	for (el = doc_el.firstChild().toElement(); !el.isNull(); el=el.nextSibling().toElement()) {
		if (el.tagName()=="table") {
			KexiDB::TableSchema *t = m_conn->tableSchema(el.attribute("name"));
			QRect rect(el.attribute("x").toInt(),el.attribute("y").toInt(),
				el.attribute("width").toInt(),el.attribute("height").toInt());
			m_relations->addTable( t, rect );
		}
		else if (el.tagName()=="conn") {
			SourceConnection src_conn;
			src_conn.masterTable = el.attribute("mtable");
			src_conn.masterField = el.attribute("mfield");
			src_conn.detailsTable = el.attribute("dtable");
			src_conn.detailsField = el.attribute("dfield");
			m_relations->addConnection(src_conn);
		}
	}
	if (!was_dirty)
		setDirty(false);
	m_dataTable->tableView()->ensureCellVisible(0,0);
	return true;
}

bool KexiQueryDesignerGuiEditor::storeData()
{
	return true;
}

QSize KexiQueryDesignerGuiEditor::sizeHint() const
{
	QSize s1 = m_relations->sizeHint();
	QSize s2 = m_head->sizeHint();
	return QSize(QMAX(s1.width(),s2.width()), s1.height()+s2.height());
}

KexiTableItem* 
KexiQueryDesignerGuiEditor::createNewRow(const QString& tableName, const QString& fieldName) const
{
	KexiTableItem *newItem = new KexiTableItem(m_data->columnsCount());
	(*newItem)[0]=tableName+"."+fieldName;
	(*newItem)[1]=tableName;
	(*newItem)[2]=QVariant(true,1);//visible
	(*newItem)[3]=QVariant(0);//totals
	return newItem;
}

void KexiQueryDesignerGuiEditor::slotDragOverTableRow(KexiTableItem *item, int row, QDragMoveEvent* e)
{
	if (e->provides("kexi/field")) {
		e->acceptAction(true);
	}
}

void
KexiQueryDesignerGuiEditor::slotDroppedAtRow(KexiTableItem *item, int row, 
	QDropEvent *ev, KexiTableItem*& newItem)
{
	//TODO: better check later if the source is really a table
	QString srcTable;
	QString srcField;
	QString dummy;

	KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
	//insert new row at specific place
	newItem = createNewRow(srcTable, srcField);
	m_droppedNewItem = newItem;
	m_droppedNewTable = srcTable;
	m_droppedNewField = srcField;
	//TODO
}

void KexiQueryDesignerGuiEditor::slotRowInserted(KexiTableItem* item, uint row)
{
	if (m_droppedNewItem && m_droppedNewItem==item) {
		createPropertyBuffer( row, m_droppedNewTable, m_droppedNewField, true );
		propertyBufferSwitched();
		m_droppedNewItem=0;
	}
}

void KexiQueryDesignerGuiEditor::slotTableAdded(KexiDB::TableSchema &t)
{
	updateColumsData();
	setDirty();
}

void KexiQueryDesignerGuiEditor::slotTableHidden(KexiDB::TableSchema &t)
{
	updateColumsData();
	setDirty();
}

void KexiQueryDesignerGuiEditor::slotBeforeCellChanged(KexiTableItem *item, int colnum, 
	QVariant newValue, KexiDB::ResultInfo* result)
{
	if (colnum==0) {//'field'
		if (newValue.isNull()) {
			m_data->updateRowEditBuffer(item, 1, QVariant(), false/*!allowSignals*/);
			m_data->updateRowEditBuffer(item, 2, QVariant(false,1));//invisible
			m_data->updateRowEditBuffer(item, 3, QVariant());//remove totals
			m_buffers->removeCurrentPropertyBuffer();
		}
		else {
			//auto fill 'table' column
			QString fieldName = newValue.toString();
			QString tableName = fieldName;
			int id = tableName.find('.');
			if (id>=0)
				tableName = tableName.left(id);
			m_data->updateRowEditBuffer(item, 1, tableName, false/*!allowSignals*/);
			m_data->updateRowEditBuffer(item, 2, QVariant(true,1));//visible
			m_data->updateRowEditBuffer(item, 3, QVariant(0));//totals
			if (!propertyBuffer()) {
				createPropertyBuffer( m_dataTable->tableView()->currentRow(), 
					tableName, fieldName, true );
				propertyBufferSwitched();
			}
			//update property
			if (propertyBuffer()) {
				KexiPropertyBuffer &buf = *propertyBuffer();
				buf["field"]->setValue(fieldName.mid(id+1));
				buf["alias"]->setValue(QString::null);
				buf["caption"]->setValue(QString::null);
				buf["table"]->setValue(tableName);
			}
		}
	}
	else if (colnum==1) {//'table'
		if (newValue.isNull()) {
			if (!item->at(0).toString().isEmpty())
				m_data->updateRowEditBuffer(item, 0, QVariant(), false/*!allowSignals*/);
			m_data->updateRowEditBuffer(item, 2, QVariant(false,1));//invisible
			m_data->updateRowEditBuffer(item, 3, QVariant());//remove totals
			m_buffers->removeCurrentPropertyBuffer();
		}
		//update property
		if (propertyBuffer()) {
			KexiPropertyBuffer &buf = *propertyBuffer();
			buf["table"]->setValue(newValue);
			buf["caption"]->setValue(QString::null);
		}
	}
	else if (colnum==2) {//'visible'
		if (!propertyBuffer()) {
			createPropertyBuffer( m_dataTable->tableView()->currentRow(), 
				item->at(1).toString(), item->at(0).toString(), true );
			m_data->updateRowEditBuffer(item, 3, QVariant(0));//totals
			propertyBufferSwitched();
		}
		if (propertyBuffer()) {
			KexiPropertyBuffer &buf = *propertyBuffer();
			buf["visible"]->setValue(newValue);
		}
	}
	else if (colnum==3) {//'criteria'
		//TODO:
		//unused yet
	}
	else if (colnum==4) {//'totals'
		//TODO:
		//unused yet
	}
}

void KexiQueryDesignerGuiEditor::slotTablePositionChanged(KexiRelationViewTableContainer*)
{
	setDirty(true);
}

void KexiQueryDesignerGuiEditor::slotAboutConnectionRemove(KexiRelationViewConnection*)
{
	setDirty(true);
}

void KexiQueryDesignerGuiEditor::slotTableFieldDoubleClicked( KexiDB::TableSchema* table, const QString& fieldName )
{
	if (!table || (!table->field(fieldName) && fieldName!="*"))
		return;
	int row_num;
	//find last filled row in the GUI table
	for (row_num=m_buffers->size()-1; !m_buffers->at(row_num) && row_num>=0; row_num--)
		;
	row_num++; //after
	//add row
	KexiTableItem *newItem = createNewRow(table->name(), fieldName);
	m_dataTable->tableView()->insertItem(newItem, row_num);
	m_dataTable->tableView()->setCursor(row_num, 0);
	//create buffer
	createPropertyBuffer( row_num, table->name(), fieldName, true/*new one*/ );
	propertyBufferSwitched();
}

KexiPropertyBuffer *KexiQueryDesignerGuiEditor::propertyBuffer()
{
	return m_buffers->currentPropertyBuffer();
}

KexiPropertyBuffer *
KexiQueryDesignerGuiEditor::createPropertyBuffer( int row, 
	const QString& tableName, const QString& fieldName, bool newOne )
{
	const bool asterisk = (tableName=="*");
	QString typeName = "KexiQueryDesignerGuiEditor::Column";
	KexiPropertyBuffer *buff = new KexiPropertyBuffer(this, typeName);

	KexiProperty *prop;
	buff->add(prop = new KexiProperty("table", QVariant(tableName)) );
	prop->setVisible(false);//always hidden

	buff->add(prop = new KexiProperty("field", QVariant(fieldName)) );
	prop->setVisible(false);//always hidden

	buff->add(prop = new KexiProperty("caption", QVariant(QString::null), i18n("Caption") ) );
	if (asterisk)
		prop->setVisible(false);

	buff->add(prop = new KexiProperty("alias", QVariant(QString::null), i18n("Alias")) );
	if (asterisk)
		prop->setVisible(false);

	buff->add(prop = new KexiProperty("visible", QVariant(true, 4)) );
	prop->setVisible(false);

/*TODO: 
	buff->add(prop = new KexiProperty("totals", QVariant(QString::null)) );
	prop->setVisible(false);*/

	//sorting
	QStringList slist, nlist;
	slist << "nosorting" << "ascending" << "descending";
	nlist << i18n("None") << i18n("Ascending") << i18n("Descending");
	buff->add(prop = new KexiProperty("sorting", slist[0], slist, nlist, i18n("Sorting")));

	m_buffers->insert(row, buff, newOne);
	return buff;
}

#include "kexiquerydesignerguieditor.moc"

