/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexialtertabledialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>

#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/error.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include <kexipropertybuffer.h>
#include <kexiproperty.h>
#include "kexipropertyeditor.h"
#include "kexidialogbase.h"
#include "kexitableview.h"

#define MAX_FIELDS 101 //nice prime number

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
	KexiDB::TableSchema *table, const char *name)
 : KexiDataTable(win, parent, name, false/*not db-aware*/)
{
	m_table = table; //orig table
//	if (m_table) //deep copy of the original table
//		m_newTable = new KexiDB::TableSchema(*m_table); 
//	else //new, empty table
//		m_newTable = new KexiDB::TableSchema(m_dialog->partItem()->name());
//	m_currentBufferCleared = false;
	init();
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
	removeCurrentPropertyBuffer();
}

void KexiAlterTableDialog::init()
{

	m_data = new KexiTableViewData();
	m_data->setInsertingEnabled( false );
	KexiTableViewColumn *col = new KexiTableViewColumn(i18n("Field name"), KexiDB::Field::Text);
//		KexiDB::Field::PrimaryKey);
	KexiValidator *vd = new Kexi::IdentifierValidator();
	vd->setAcceptsEmptyValue(true);
	col->setValidator( vd );

	m_data->addColumn( col );
	KexiDB::Field *f = new KexiDB::Field(i18n("Data type"), KexiDB::Field::Enum);
//		KexiDB::Field::NotEmpty | KexiDB::Field::NotNull);
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup);
	int maxTypeNameTextWidth = 0;
	QFontMetrics fm(font());
	for (int i=1; i<=KexiDB::Field::LastTypeGroup; i++) {
		types[i-1] = KexiDB::Field::typeGroupName(i);
		maxTypeNameTextWidth = QMAX(maxTypeNameTextWidth, fm.width(types[i-1]));
	}
	f->setEnumHints(types);

	m_data->addColumn( new KexiTableViewColumn(*f) );
	m_data->addColumn( new KexiTableViewColumn(i18n("Comments"), KexiDB::Field::Text) );

/*	KexiTableItem *item = new KexiTableItem(0);
	item->push_back(QVariant("name"));
	item->push_back(QVariant("Text"));
	item->push_back(QVariant(""));
	data->append(item);
*/

//	m_view = new KexiTableView(data, this, "tableview");
//	QVBoxLayout *box = new QVBoxLayout(this);
//	box->addWidget(m_view);

	m_view->setSpreadSheetMode();
	m_view->adjustColumnWidthToContents(0); //adjust column width
//	m_view->adjustColumnWidthToContents(1); //adjust column width
	m_view->setColumnWidth(1, maxTypeNameTextWidth + 2*m_view->rowHeight());
	m_view->setColumnStretchEnabled( true, 2 ); //last column occupies the rest of the area

//	setFocusProxy(m_view);

	connect(m_view, SIGNAL(cellSelected(int,int)), 
		this, SLOT(slotCellSelected(int,int)));
	connect(m_data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	connect(m_data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*)),
		this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*)));
//	connect(data, SIGNAL(aboutToUpdateRow(KexiTableItem*,KexiDB::RowEditBuffer*,KexiDB::ResultInfo*)),
//		this, SLOT(slotAboutToUpdateRow(KexiTableItem*,KexiDB::RowEditBuffer*,KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowDeleted()), this, SLOT(slotRowDeleted()));
	connect(m_data, SIGNAL(rowInserted(KexiTableItem*,uint)), 
		this, SLOT(slotEmptyRowInserted(KexiTableItem*,uint)));
	

/*	//! before closing - we'are accepting editing
	connect(this,SIGNAL(closing()),m_view,SLOT(acceptRowEdit()));

	//! updating actions on start/stop editing
	connect(m_view, SIGNAL(rowEditStarted(int)), this, SLOT(slotUpdateRowActions(int)));
	connect(m_view, SIGNAL(rowEditTerminated(int)), this, SLOT(slotUpdateRowActions(int)));

*/
//	m_properties = new KexiPropertyEditor(splitter);
//	m_properties->setBuffer(m_constraints.at(0));

	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
//	resize( preferredSizeHint( m_view->sizeHint() ) );
	m_view->setFocus();
	initActions();
}

void KexiAlterTableDialog::initData()
{
	m_buffers.clear();

	m_buffers.resize(MAX_FIELDS);
	m_buffers.setAutoDelete(true);
	m_row = -99;

	//add column data
	m_data->clear();
	int tableFieldCount = 0;
	if (m_table) {
		tableFieldCount = m_table->fieldCount();
		for(int i=0; i < tableFieldCount; i++)
		{
			KexiDB::Field *field = m_table->field(i);
			KexiTableItem *item = new KexiTableItem(0);
			item->push_back(QVariant(field->name()));
			item->push_back(QVariant(field->typeGroup()-1)); //-1 because type groups are counted from 1
			item->push_back(QVariant(field->description()));
			m_data->append(item);
	
			createPropertyBuffer( i, field );
		}
	}
	//add empty space
	for (int i=tableFieldCount; i<MAX_FIELDS; i++) {
//		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
//		buff->insert("primaryKey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
//		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
//		m_fields.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(3);//3 empty fields
		m_data->append(item);
	}

//	QSplitter *splitter = new QSplitter(Vertical, this);

	kdDebug() << "KexiAlterTableDialog::init(): vector contains " << m_buffers.size() << " items" << endl;

	m_view->setData(m_data);
}

bool updatePropertiesVisibility(KexiDB::Field::Type fieldType, KexiPropertyBuffer& buf)
{
	bool changed = false;
	KexiProperty *prop;
	bool visible;
	//if there is no more than 1 subType name: hide the property
	prop = buf["subType"];
	visible = prop->keys() && prop->keys()->count()>1;
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = buf["unsigned"];
	visible = KexiDB::Field::isNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = buf["length"];
	visible = KexiDB::Field::isTextType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = buf["precision"];
	visible = KexiDB::Field::isFPNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = buf["notEmpty"];
	visible = KexiDB::Field::hasEmptyProperty(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	return changed;
}

KexiPropertyBuffer *
KexiAlterTableDialog::createPropertyBuffer( int row, KexiDB::Field *field )
{
	QString typeName = "KexiDB::Field::" + field->typeGroupString();
	KexiPropertyBuffer *buff = new KexiPropertyBuffer(this, typeName);
	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));
	//name
	KexiProperty *prop;
	buff->add(prop = new KexiProperty("name", QVariant(field->name()), i18n("Name")) );
	prop->setVisible(false);//always hidden

	//type
	buff->add( prop = new KexiProperty("type", QVariant(field->type()), i18n("Type")) );
	prop->setVisible(false);//always hidden

	//subtype
	const QStringList slist = KexiDB::typeStringsForGroup(field->typeGroup());
	const QStringList nlist = KexiDB::typeNamesForGroup(field->typeGroup());
	kdDebug() << "KexiAlterTableDialog::init(): subType strings: " << 
		slist.join("|") << "\nnames: " << nlist.join("|") << endl;
	buff->add(prop = new KexiProperty("subType", field->typeString(), slist, nlist, i18n("Subtype")));

	buff->add( new KexiProperty("caption", QVariant(field->caption()), i18n("Caption") ) );

	buff->add( prop = new KexiProperty("description", QVariant(field->description())) );
	prop->setVisible(false);//always hidden

	buff->add(prop = new KexiProperty("unsigned", QVariant(field->isUnsigned(), 4), i18n("Unsigned number")));

	buff->add( prop = new KexiProperty("length", (int)field->length()/*200?*/, i18n("Length")));

	buff->add( prop = new KexiProperty("precision", (int)field->precision()/*200?*/, i18n("Precision")));

//TODO: set reasonable default for column width...
	buff->add( new KexiProperty("width", (int)field->width()/*200?*/, i18n("Column width")));

	buff->add( prop = new KexiProperty("defaultValue", field->defaultValue()/*200?*/, i18n("Default value")));
//TODO: show this after we get properly working editor for QVariant:
	prop->setVisible(false);

	buff->add(new KexiProperty("primaryKey", QVariant(field->isPrimaryKey(), 4), i18n("Primary Key")));

	buff->add(new KexiProperty("unique", QVariant(field->isUniqueKey(), 4), i18n("Unique")));

	buff->add(new KexiProperty("notNull", QVariant(field->isNotNull(), 4), i18n("Required")));
	
	buff->add(prop = new KexiProperty("notEmpty", QVariant(field->isNotEmpty(), 4), i18n("Not Empty")));

	buff->add(new KexiProperty("indexed", QVariant(field->isIndexed(), 4), i18n("Indexed")));

	updatePropertiesVisibility(field->type(), *buff);

	m_buffers.insert(row, buff);
	return buff;
}

void
KexiAlterTableDialog::initActions()
{

}

void KexiAlterTableDialog::slotCellSelected(int, int row)
{
	kdDebug() << "KexiAlterTableDialog::slotCellSelected()" << endl;
	if(row == m_row)
		return;
	m_row = row;
	propertyBufferSwitched();
}

bool KexiAlterTableDialog::beforeSwitchTo(int mode, bool &cancelled)
{
	if (mode==Kexi::DesignViewMode) {
		initData();
		return true;
	}
	else if (mode==Kexi::DataViewMode) {
		if (!dirty() && parentDialog()->neverSaved()) {
			cancelled=true;
			KMessageBox::information(this, i18n("Cannot switch to data view, because table design is empty.\n"
				"First, please create your design.") );
			return true;
		}

#if 0
		//todo
		KexiDB::TableSchema *nt = new KexiDB::TableSchema(m_newTable->name());
		nt->setCaption(m_newTable->caption());

		KexiTableViewData *data = m_view->data();
		int i=0;
		for(KexiTableItem *it = data->first(); it; it = data->next(), i++)
		{
			if (!(*it)[0].toString().isEmpty()) {//for nonempty names:
				KexiProperty *prop = (*m_buffers.at(i))["pkey"];
				if (prop && !it->at(0).toString().isEmpty())
				{
					KexiDB::Field *f = new KexiDB::Field(nt);
					f->setName(it->at(0).toString());
					f->setType((KexiDB::Field::Type)it->at(1).toInt());
					f->setPrimaryKey(prop->value().toBool());

					nt->addField(f);
				}
			}
		}

		KexiDB::TableSchema *s = mainWin()->project()->dbConnection()->tableSchema(m_newTable->name());
		if(!s)
		{
			KexiDB::TableSchema *ts = mainWin()->project()->dbConnection()->tableSchema("kexi__objects");
			mainWin()->project()->dbConnection()->dropTable(m_newTable->name());
			mainWin()->project()->dbConnection()->createTable(nt);
		}
#endif 
	/*	else
		{
			KexiDB::Cursor *cursor = mainWin()->project()->dbConnection()->executeQuery(*s, KexiDB::Cursor::Buffered);
			mainWin()->project()->dbConnection()->dropTable(m_newTable->name());
			mainWin()->project()->dbConnection()->createTable(nt);
			for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
			{
				
			}
			mainWin()->project()->dbConnection()->deleteCursor(cursor);
		}*/
		return true;
	}
	else if (mode==Kexi::TextViewMode) {
		//todo
	}
	return false;
}

KexiPropertyBuffer *KexiAlterTableDialog::propertyBuffer()
{
	return (m_view->currentRow() >= 0) ? 
		m_buffers.at( m_view->currentRow() ) : 0;
//	return m_currentBufferCleared ? 0 : m_buffers.at(m_view->currentRow());
}

void KexiAlterTableDialog::removeCurrentPropertyBuffer()
{
	const int r = m_view->currentRow();
	KexiPropertyBuffer *buf = m_buffers.at(r);
	if (!buf)
		return;
	buf->debug();
//	m_currentBufferCleared = true;
	m_buffers.remove(r);
	propertyBufferSwitched();
//	delete buf;
//	m_currentBufferCleared = false;
}

/*
void KexiAlterTableDialog::slotUpdateRowActions(int row)
{
	setAvailable("edit_delete_row", !m_view->isReadOnly() && !(m_view->isInsertingEnabled() && row==m_view->rows()) );
	setAvailable("data_save_row", m_view->rowEditing());
}*/

void KexiAlterTableDialog::slotPropertyChanged(KexiPropertyBuffer& /*buf*/ ,KexiProperty& /*prop*/)
{
	setDirty();
	//TODO
}

KexiDB::Field::Type firstTypeForSelectedGroup( int typegroup )
{
	//take the 1st type for the group
	KexiDB::TypeGroupList tlst = KexiDB::typesForGroup( (KexiDB::Field::TypeGroup)typegroup );
	if (tlst.isEmpty()) {//this should not be!
		kdWarning() << "KexiAlterTableDialog::slotRowUpdated(): no types for group " 
		<< typegroup << endl;
		return KexiDB::Field::InvalidType;
	}
	return static_cast<KexiDB::Field::Type>(tlst.first());
}

void KexiAlterTableDialog::slotBeforeCellChanged(
	KexiTableItem *item, int colnum, QVariant newValue, KexiDB::ResultInfo* /*result*/)
{
	if (colnum==0) {//'name'
//		if (!item->at(1).toString().isEmpty() && item->at(1).isNull()) {
		//if 'type' is not filled yet
		if (item->at(1).isNull()) {
			//auto select 1st row of 'type' column
			m_view->data()->updateRowEditBuffer(item, 1, QVariant((int)0));
		}

		if (propertyBuffer()) {
			//update field name
			KexiPropertyBuffer &buf = *propertyBuffer();
			buf["name"]->setValue(newValue);
		}
	}
	else if (colnum==1) {//'type'
		if (newValue.isNull()) {
			//'type' col will be cleared: clear row 0 as well
			m_view->data()->updateRowEditBuffer(item, 0, QVariant(QString::null));
			return;
		}

		if (!propertyBuffer())
			return;

		KexiPropertyBuffer &buf = *propertyBuffer();
		//'type' col is changed (existed before)
		//-get type group number
		KexiDB::Field::TypeGroup fieldTypeGroup;
		int i_fieldTypeGroup = newValue.toInt()+1/*counting from 1*/;
		if (i_fieldTypeGroup < 1 || i_fieldTypeGroup > (int)KexiDB::Field::LastTypeGroup)
			return;
		fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>(i_fieldTypeGroup);

		//-get 1st type from this group, and update 'type' property
		KexiDB::Field::Type fieldType = firstTypeForSelectedGroup( i_fieldTypeGroup );
		if (fieldType==KexiDB::Field::InvalidType)
			fieldType = KexiDB::Field::Text;
		buf["type"]->setValue((int)fieldType);

		//-get subtypes for this type: keys (slist) and names (nlist)
		const QStringList slist = KexiDB::typeStringsForGroup(fieldTypeGroup);
		const QStringList nlist = KexiDB::typeNamesForGroup(fieldTypeGroup);
		KexiProperty *subTypeProperty = buf["subType"];

		//update subtype list and value
		subTypeProperty->setList(slist, nlist);
		subTypeProperty->setValue( KexiDB::Field::typeString(fieldType) );
		if (updatePropertiesVisibility(fieldType, buf)) {
			//properties' visiblility changed: refresh buffer
			propertyBufferReloaded();
		}
	}
	else if (colnum==2) {//'description'
		if (!propertyBuffer())
			return;

		//update field desc.
		KexiPropertyBuffer &buf = *propertyBuffer();
		buf["description"]->setValue(item->at(2));
	}
}

void KexiAlterTableDialog::slotRowUpdated(KexiTableItem *item)
{
	setDirty();

	//-check if the row was empty before updating
	//if yes: we want to add a property buffer for this new row (field)
	QString fieldName = item->at(0).toString();
//	const bool buffer_allowed = !fieldName.isEmpty() && !item->at(1).isNull();
	const bool buffer_allowed = !item->at(1).isNull();

	if (!buffer_allowed && propertyBuffer()) {
		//there is a buffer, but it's not allowed - remove it:
		removeCurrentPropertyBuffer();

		//clear 'type' column:
		m_view->data()->clearRowEditBuffer();
		m_view->data()->updateRowEditBuffer(m_view->selectedItem(), 1, QVariant());
		m_view->data()->saveRowChanges(*m_view->selectedItem());
	
	} else if (buffer_allowed && !propertyBuffer()) {
		//-- create a new field:
		int fieldType = firstTypeForSelectedGroup( item->at(1).toInt()+1/*counting from 1*/ );
		if (fieldType==0)
			return;

		QString description = item->at(2).toString();

		KexiDB::Field field( //tmp
			fieldName,
			(KexiDB::Field::Type)fieldType,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			/*length*/0, 
			/*precision*/0,
			/*defaultValue*/QVariant(),
			/*caption*/QString::null,
			description,
			/*width*/0);
//		m_newTable->addField( field );

		kdDebug() << "KexiAlterTableDialog::slotRowUpdated(): " << field.debugString() << endl;

		//create new property buffer:
		KexiPropertyBuffer *newbuff = createPropertyBuffer( m_view->currentRow(), &field );
		//add a special property indicating that this is brand new buffer, 
		//not just changed
		KexiProperty* prop = new KexiProperty("newrow", QVariant());
		prop->setVisible(false);
		newbuff->add( prop );

		//refresh property editor:
		propertyBufferSwitched();
	}
}

void KexiAlterTableDialog::slotAboutToInsertRow(KexiTableItem* item, 
	KexiDB::ResultInfo* /*result*/)
{
	setDirty();
	//TODO
}

void KexiAlterTableDialog::slotRowDeleted()
{
	setDirty();
	//remove current prop. buffer
	removeCurrentPropertyBuffer();

	//let's move up all buffers that are below that deleted
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
	const int r = m_view->currentRow();
	for (int i=r;i<int(m_buffers.size()-1);i++) {
		KexiPropertyBuffer *b = m_buffers[i+1];
		m_buffers.insert( i , b );
	}
	m_buffers.insert( m_buffers.size()-1, 0 );
	m_buffers.setAutoDelete(true);//revert the flag

	propertyBufferSwitched();
}

void KexiAlterTableDialog::slotEmptyRowInserted(KexiTableItem*, uint /*index*/)
{
	setDirty();

	//let's move down all buffers that are below that deleted
	m_buffers.setAutoDelete(false);//to avoid auto deleting in insert()
	const int r = m_view->currentRow();
	m_buffers.resize(m_buffers.size()+1);
	for (int i=int(m_buffers.size()); i>r; i--) {
		KexiPropertyBuffer *b = m_buffers[i-1];
		m_buffers.insert( i , b );
	}
	m_buffers.insert( r, 0 );
	m_buffers.setAutoDelete(true);//revert the flag

	propertyBufferSwitched();
	
}

KexiDB::SchemaData* KexiAlterTableDialog::storeNewData(const KexiDB::SchemaData& sdata)
{
	if (m_table || m_dialog->schemaData()) //must not be
		return 0;
	
	m_view->acceptRowEdit();

	//create table schema definition
	m_table = new KexiDB::TableSchema(sdata.name());
	m_table->setName( sdata.name() );
	m_table->setCaption( sdata.caption() );
	m_table->setDescription( sdata.description() );

	//check for duplicates
	KexiPropertyBuffer *b = 0;
	bool no_fields = true;
	int i;
	QDict<char> names(101, false);
	char dummy;
	for (i=0;i<(int)m_buffers.size();i++) {
		b = m_buffers[i];
		if (b) {
			no_fields = false;
			const QString name = (*b)["name"]->value().toString();
			if (name.isEmpty()) {
				m_view->setCursor(i, 0);
				m_view->startEditCurrentCell();
				KMessageBox::information(this, i18n("You should enter field name.") );
				return 0;
			}
			if (names[name]) {
				break;
			}
			names.insert( name, &dummy ); //remember
		}
	}
	if (no_fields) {//no fields added
		KMessageBox::information(this, i18n("You have added no fields.\nEvery table should have at least one field.") );
		return 0;
	}
	if (b && i<(int)m_buffers.size()) {//found a duplicate
		m_view->setCursor(i, 0);
		m_view->startEditCurrentCell();
		KMessageBox::information(this, i18n("You have added \"%1\" field name twice.\nField names cannot be repeated. Correct name of the field.")
			.arg((*b)["name"]->value().toString()) );
		return 0;
	}

	//for every field, create KexiDB::Field definition
	for (i=0;i<(int)m_buffers.size();i++) {
		KexiPropertyBuffer *b = m_buffers[i];
		if (!b)
			continue;
		KexiPropertyBuffer &buf = *b;
		uint constraints = 0;
		uint options = 0;
		if (buf["primaryKey"]->value().toBool())
			constraints |= KexiDB::Field::PrimaryKey;
		if (buf["unique"]->value().toBool())
			constraints |= KexiDB::Field::Unique;
		if (buf["notnull"]->value().toBool())
			constraints |= KexiDB::Field::NotNull;
		if (buf["notEmpty"]->value().toBool())
			constraints |= KexiDB::Field::NotEmpty;

		if (buf["unsigned"]->value().toBool())
			options |= KexiDB::Field::Unsigned;
			
//		int type = buf["type"]->value().toInt();
//		if (type < 0 || type > (int)KexiDB::Field::LastType)
//			type = KexiDB::Field::Text;
		kdDebug() << buf["subType"]->value().toString() << endl;
//		int typeGroup = KexiDB::typeGroup(type);
		QString typeString = buf["subType"]->value().toString();
		KexiDB::Field::Type type = KexiDB::Field::typeForString(typeString);
		if (type==KexiDB::Field::InvalidType)
			type = KexiDB::Field::Text;

		KexiDB::Field *f = new KexiDB::Field( 
			buf["name"]->value().toString(),
			type,
			constraints,
			options,
			buf["length"]->value().toInt(),
			buf["precision"]->value().toInt(),
			buf["defaultValue"]->value(),
			buf["caption"]->value().toString(),
			buf["description"]->value().toString(),
			buf["width"]->value().toInt()
		);
		m_table->addField(f);
	}

	//todo

	KexiDB::Connection *conn = mainWin()->project()->dbConnection();

	//FINALLY: create table:
	if (!conn->createTable(m_table)) {
		//todo: show err...
		delete m_table;
		m_table = 0;
	}
	else {
		//finally, we've got a table schema
		setDirty(false);
	}
	return m_table;
}

bool KexiAlterTableDialog::storeData()
{
//	KexiDB::TableSchema *ts = static_cast<KexiDB::TableSchema*>(m_dialog->schemaData());
	m_view->acceptRowEdit();

//<TODO: remove this in the future>
	if (!m_dialog->neverSaved()) {
		return KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Saving changes for existing table design are not yet supported.\nDo you want to discard your changes now?"));
	}
//</TODO>

/*** TODO: ALTER TABLE CODE IN KEXIDB!
	if (!ts || !mainWin()->project()->dbConnection()->alterTable(ts))
		return 0;
*/
	setDirty(false);
	return true;
}

/*void KexiAlterTableDialog::slotAboutToUpdateRow(
	KexiTableItem* item, KexiDB::RowEditBuffer* buffer, KexiDB::ResultInfo* result)
{
	KexiDB::RowEditBuffer::SimpleMap map = buffer->simpleBuffer();
	buffer->debug();

	QVariant old_type = item->at(1);
	QVariant *buf_type = buffer->at( m_view->field(1)->name() );

	//check if there is a type specified
//	if ((old_type.isNull() && !buf_type) || (buf_type && buf_type->isNull())) {
		//kdDebug() << "err" << endl;
	//}
//	allow = true;
//	m_dirty = m_dirty | result->success;
}*/



#include "kexialtertabledialog.moc"

