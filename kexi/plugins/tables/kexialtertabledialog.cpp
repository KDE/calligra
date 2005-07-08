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

#include <kexiutils/identifier.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include <koproperty/set.h>
#include <koproperty/property.h>
#include <widget/tableview/kexidataawarepropertyset.h>

#include <kexidialogbase.h>
#include <kexitableview.h>

//#define MAX_FIELDS 101 //nice prime number

//! indexes for table columns
#define COLUMN_ID_PK 0
#define COLUMN_ID_NAME 1
#define COLUMN_ID_TYPE 2
#define COLUMN_ID_DESC 3

class KexiAlterTableDialogPrivate
{
	public:
		KexiAlterTableDialogPrivate()
		 : sets(0)
		 , dontAskOnStoreData(false)
		 , slotTogglePrimaryKeyCalled(false)
		 , primaryKeyExists(false)
		{}

		~KexiAlterTableDialogPrivate() {
			delete sets;
		}

		KexiTableViewData *data;

		KexiDataAwarePropertySet *sets;

		int row; //!< used to know if a new row is selected in slotCellSelected()

		KToggleAction *action_toggle_pkey;

		//! internal
		int maxTypeNameTextWidth;
		//! Set to true in beforeSwitchTo() to avoid asking again in storeData()
		bool dontAskOnStoreData : 1;

		bool slotTogglePrimaryKeyCalled : 1;

		bool primaryKeyExists : 1;
};

//----------------------------------------------

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent,
	const char *name)
 : KexiDataTable(win, parent, name, false/*not db-aware*/)
 , d( new KexiAlterTableDialogPrivate() )
{
	d->data = new KexiTableViewData();
	d->data->setInsertingEnabled( false );

	KexiTableViewColumn *col = new KexiTableViewColumn(i18n("Primary Key", "PK"), KexiDB::Field::Text);
	col->field()->setDescription(i18n("Primary Key"));
	col->field()->setSubType("KIcon");
	col->setReadOnly(true);
	d->data->addColumn( col );

	col = new KexiTableViewColumn(i18n("Field Name"), KexiDB::Field::Text);
	KexiUtils::Validator *vd = new KexiUtils::IdentifierValidator();
	vd->setAcceptsEmptyValue(true);
	col->setValidator( vd );

	d->data->addColumn( col );
	KexiDB::Field *f = new KexiDB::Field(i18n("Data Type"), KexiDB::Field::Enum);

#ifdef KEXI_SHOW_UNIMPLEMENTED
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup);
#else
//TODO: remove this later
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup-1); //don't show last (BLOB) type
#endif
	d->maxTypeNameTextWidth = 0;
	QFontMetrics fm(font());
	for (uint i=1; i<=types.count(); i++) {
		types[i-1] = KexiDB::Field::typeGroupName(i);
		d->maxTypeNameTextWidth = QMAX(d->maxTypeNameTextWidth, fm.width(types[i-1]));
	}
	f->setEnumHints(types);

	d->data->addColumn( new KexiTableViewColumn(*f) );
	d->data->addColumn( new KexiTableViewColumn(i18n("Comments"), KexiDB::Field::Text) );

	m_view = dynamic_cast<KexiTableView*>(mainWidget());

	m_view->setSpreadSheetMode();
//	setFocusProxy(m_view);

	connect(d->data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	connect(d->data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
		this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));

	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	m_view->setFocus();

	d->sets = new KexiDataAwarePropertySet( this, m_view );
	connect(d->sets, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
	connect(d->sets, SIGNAL(rowInserted()), this, SLOT(updateActions()));

	plugSharedAction("tablepart_toggle_pkey", this, SLOT(slotTogglePrimaryKey()));
	d->action_toggle_pkey = static_cast<KToggleAction*>( sharedAction("tablepart_toggle_pkey") );
	d->action_toggle_pkey->plug(m_view->contextMenu(), 0); //add at the beg.
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
//	removeCurrentPropertySet();
	delete d;
}

void KexiAlterTableDialog::initData()
{
	//add column data
//	d->data->clear();
	d->data->deleteAllRows();
	int tableFieldCount = 0;
	d->primaryKeyExists = false;

	if (tempData()->table) {
		tableFieldCount = tempData()->table->fieldCount();
//not needed		d->sets->clear(tableFieldCount);

		//recreate table data rows
		for(int i=0; i < tableFieldCount; i++) {
			KexiDB::Field *field = tempData()->table->field(i);
			KexiTableItem *item = d->data->createItem(); //new KexiTableItem(0);
			(*item)[0] = field->isPrimaryKey() ? "key" : "";
			if (field->isPrimaryKey())
				d->primaryKeyExists = true;
			(*item)[1] = field->name();
			(*item)[2] = field->typeGroup()-1; //-1 because type groups are counted from 1
			(*item)[3] = field->description();
			d->data->append(item);

//later!			createPropertySet( i, field );
		}
	}
//	else {
//		d->sets->clear();//default size
//	}

	//add empty space
	const int columnsCount = d->data->columnsCount();
	for (int i=tableFieldCount; i<(int)d->sets->size(); i++) {
//		KexiTableItem *item = new KexiTableItem(columnsCount);//3 empty fields
		d->data->append(d->data->createItem());
	}

	//set data for our spreadsheet: this will clear our sets
	m_view->setData(d->data);

	//now recreate property sets
	if (tempData()->table) {
		for(int i=0; i < tableFieldCount; i++) {
			KexiDB::Field *field = tempData()->table->field(i);
			createPropertySet( i, field );
		}
	}

	//column widths
	m_view->setColumnWidth(COLUMN_ID_PK, IconSize( KIcon::Small ) + 10);
	m_view->adjustColumnWidthToContents(COLUMN_ID_NAME); //adjust column width
	m_view->setColumnWidth(COLUMN_ID_TYPE, d->maxTypeNameTextWidth + 2*m_view->rowHeight());
	m_view->setColumnStretchEnabled( true, COLUMN_ID_DESC ); //last column occupies the rest of the area

	setDirty(false);
	m_view->setCursorPosition(0, COLUMN_ID_NAME); //set @ name column
}

static bool updatePropertiesVisibility(KexiDB::Field::Type fieldType, KoProperty::Set &set)
{
	bool changed = false;
	KoProperty::Property *prop;
	bool visible;
	//if there is no more than 1 subType name or it's a PK: hide the property
	prop = &set["subType"];
	visible = prop->listData() && prop->listData()->keys.count() > 1
		&& set["primaryKey"].value().toBool()==false;
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &set["unsigned"];
	visible = KexiDB::Field::isNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &set["length"];
	visible = (fieldType == KexiDB::Field::Text);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		prop->setValue( visible ? KexiDB::Field::defaultTextLength() : 0, false );
		changed = true;
	}
#ifndef KEXI_NO_UNFINISHED
	prop = &set["precision"];
	visible = KexiDB::Field::isFPNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
#endif
	prop = &set["allowEmpty"];
	visible = KexiDB::Field::hasEmptyProperty(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &set["autoIncrement"];
	visible = KexiDB::Field::isAutoIncrementAllowed(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	return changed;
}

KoProperty::Set *
KexiAlterTableDialog::createPropertySet( int row, KexiDB::Field *field, bool newOne )
{
	QString typeName = "KexiDB::Field::" + field->typeGroupString();
	KoProperty::Set *set = new KoProperty::Set(d->sets, typeName);
//	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
//		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));

	KoProperty::Property *prop;

	//meta-info for property editor
	set->addProperty(prop = new KoProperty::Property("this:classString", i18n("Table field")) );
	prop->setVisible(false);
//! \todo add table_field icon (add	buff->addProperty(prop = new KexiProperty("this:iconName", "table_field") );
//	prop->setVisible(false);

	//name
	set->addProperty(prop = new KoProperty::Property("name", QVariant(field->name()), i18n("Name")) );
	prop->setVisible(false);//always hidden

	//type
	set->addProperty( prop = new KoProperty::Property("type", QVariant(field->type()), i18n("Type")) );
	prop->setVisible(false);//always hidden

	//subtype
	const QStringList slist = KexiDB::typeStringsForGroup(field->typeGroup());
	const QStringList nlist = KexiDB::typeNamesForGroup(field->typeGroup());
	kdDebug() << "KexiAlterTableDialog::init(): subType strings: " << 
		slist.join("|") << "\nnames: " << nlist.join("|") << endl;
	set->addProperty(prop = new KoProperty::Property("subType", nlist, slist, field->typeString(), i18n("Subtype")));

	set->addProperty( prop = new KoProperty::Property("caption", QVariant(field->caption()), i18n("Caption") ) );
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

	set->addProperty( prop = new KoProperty::Property("description", QVariant(field->description())) );
	prop->setVisible(false);//always hidden

	set->addProperty(prop = new KoProperty::Property("unsigned", QVariant(field->isUnsigned(), 4), i18n("Unsigned Number")));

	set->addProperty( prop = new KoProperty::Property("length", (int)field->length()/*200?*/, i18n("Length")));

	set->addProperty( prop = new KoProperty::Property("precision", (int)field->precision()/*200?*/, i18n("Precision")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

//TODO: set reasonable default for column width...
	set->addProperty( prop = new KoProperty::Property("width", (int)field->width()/*200?*/, i18n("Column width")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

	set->addProperty( prop = new KoProperty::Property("defaultValue", field->defaultValue(), i18n("Default value")));
//TODO: show this after we get properly working editor for QVariant:
	prop->setVisible(false);

	set->addProperty(prop = new KoProperty::Property("primaryKey", QVariant(field->isPrimaryKey(), 4), i18n("Primary Key")));
	prop->setIcon("key");

	set->addProperty(new KoProperty::Property("unique", QVariant(field->isUniqueKey(), 4), i18n("Unique")));

	set->addProperty(new KoProperty::Property("notNull", QVariant(field->isNotNull(), 4), i18n("Required")));
	
	set->addProperty(new KoProperty::Property("allowEmpty", QVariant(!field->isNotEmpty(), 4), i18n("Allow Zero\nSize")));

	set->addProperty(prop = new KoProperty::Property("autoIncrement", QVariant(field->isAutoIncrement(), 4), i18n("Autonumber")));
	prop->setIcon("autonumber");

	set->addProperty(new KoProperty::Property("indexed", QVariant(field->isIndexed(), 4), i18n("Indexed")));

	updatePropertiesVisibility(field->type(), *set);

	connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

	d->sets->insert(row, set, newOne);
	return set;
}

void KexiAlterTableDialog::updateActions(bool /*activated*/)
{
/*! \todo check if we can set pkey for this column type (eg. BLOB?) */
	setAvailable("tablepart_toggle_pkey", propertySet()!=0);
	if (!propertySet())
		return;
	KoProperty::Set &set = *propertySet();
	d->slotTogglePrimaryKeyCalled = true;
	 d->action_toggle_pkey->setChecked(set["primaryKey"].value().toBool());
	d->slotTogglePrimaryKeyCalled = false;
}

void KexiAlterTableDialog::slotUpdateRowActions(int row)
{
	KexiDataTable::slotUpdateRowActions(row);
	updateActions();
}

void KexiAlterTableDialog::slotTogglePrimaryKey()
{
	if (d->slotTogglePrimaryKeyCalled)
		return;
	d->slotTogglePrimaryKeyCalled = true;
	if (!propertySet())
		return;
	KoProperty::Set &set = *propertySet();
	bool isSet = !set["primaryKey"].value().toBool();
	setPrimaryKey(set, isSet);
	d->slotTogglePrimaryKeyCalled = false;
}

void KexiAlterTableDialog::setPrimaryKey(KoProperty::Set &propertySet, bool set)
{
	const bool was_pkey = propertySet["primaryKey"].value().toBool();
	propertySet["primaryKey"] = QVariant(set, 1);
	if (&propertySet==this->propertySet()) {
		//update action and icon @ column 0 (only if we're changing current property set)
		d->action_toggle_pkey->setChecked(set);
		if (m_view->selectedItem()) {
			//show key in the table
			m_view->data()->clearRowEditBuffer();
			m_view->data()->updateRowEditBuffer(m_view->selectedItem(), COLUMN_ID_PK, QVariant(set ? "key" : ""));
			m_view->data()->saveRowChanges(*m_view->selectedItem(), true);
		}
		if (was_pkey || set) //change flag only if we're setting pk or really clearing it
			d->primaryKeyExists = set;
	}

	if (set) {
		//primary key is set, remove old pkey if exists
		KoProperty::Set *s = 0;
		int i;
		const int count = (int)d->sets->size();
		for (i=0; i<count; i++) {
			s = d->sets->at(i);
			if (s && s!=&propertySet && (*s)["primaryKey"].value().toBool() && i!=m_view->currentRow())
				break;
		}
		if (i<count) {//remove
			(*s)["autoIncrement"] = QVariant(false, 0);
			(*s)["primaryKey"] = QVariant(false, 0);
			//remove key from table
			m_view->data()->clearRowEditBuffer();
			KexiTableItem *item = m_view->itemAt(i);
			if (item) {
				m_view->data()->updateRowEditBuffer(item, COLUMN_ID_PK, QVariant());
				m_view->data()->saveRowChanges(*item, true);
			}
		}
		//set unsigned big-integer type
//		m_view->data()->saveRowChanges(*m_view->selectedItem());
		m_view->data()->clearRowEditBuffer();
		m_view->data()->updateRowEditBuffer(m_view->selectedItem(), COLUMN_ID_TYPE,
			QVariant(KexiDB::Field::IntegerGroup-1/*counting from 0*/));
//			QVariant(KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup)));
		m_view->data()->saveRowChanges(*m_view->selectedItem(), true);
//		propertySet["type"] = KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup);
//		propertySet["type"] = (int)KexiDB::Field::IntegerGroup;
		propertySet["subType"] = KexiDB::Field::typeString(KexiDB::Field::BigInteger);
		propertySet["unsigned"] = QVariant(true,4);
	}
	updateActions();
}

/*void KexiAlterTableDialog::slotCellSelected(int, int row)
{
	kdDebug() << "KexiAlterTableDialog::slotCellSelected()" << endl;
	if(row == m_row)
		return;
	m_row = row;
	propertyBufferSwitched();
}*/

QString KexiAlterTableDialog::messageForSavingChanges(bool &emptyTable)
{
	KexiDB::Connection *conn = mainWin()->project()->dbConnection();
	bool ok;
	emptyTable = conn->isEmpty( *tempData()->table, ok ) && ok;
	return i18n("Do you want to save the design now?")
	+ ( emptyTable ? QString::null :
		QString("\n\n") + i18n("Note: This table is already filled with data which will be removed.") );
}

tristate KexiAlterTableDialog::beforeSwitchTo(int mode, bool &dontStore)
{
	if (!m_view->acceptRowEdit())
		return false;
/*	if (mode==Kexi::DesignViewMode) {
		initData();
		return true;
	}
	else */
	tristate res = true;
	if (mode==Kexi::DataViewMode) {
		if (!dirty() && parentDialog()->neverSaved()) {
			KMessageBox::sorry(this, i18n("Cannot switch to data view, because table design is empty.\n"
				"First, please create your design.") );
			return cancelled;
		}
//<temporary>
		else if (dirty() && !parentDialog()->neverSaved()) {
//			cancelled = (KMessageBox::No == KMessageBox::questionYesNo(this, i18n("Saving changes for existing table design is not yet supported.\nDo you want to discard your changes now?")));

//			KexiDB::Connection *conn = mainWin()->project()->dbConnection();
			bool emptyTable;
			int r = KMessageBox::questionYesNoCancel(this,
				i18n("Saving changes for existing table design is now required.")
				+"\n"+messageForSavingChanges(emptyTable), QString::null,
				KStdGuiItem::save(), KStdGuiItem::discard());
			if (r == KMessageBox::Cancel)
				res = cancelled;
			else
				res = true;
			dontStore = (r!=KMessageBox::Yes);
			if (!dontStore)
				d->dontAskOnStoreData = true;
//			if (dontStore)
//				setDirty(false);
		}
//</temporary>
		//todo
		return res;
	}
	else if (mode==Kexi::TextViewMode) {
		//todo
	}
	return res;
}

tristate KexiAlterTableDialog::afterSwitchFrom(int mode)
{
	if (mode==Kexi::NoViewMode || mode==Kexi::DataViewMode) {
		initData();
	}
	return true;
}

KoProperty::Set *KexiAlterTableDialog::propertySet()
{
	return d->sets ? d->sets->currentPropertySet() : 0;
}

/*
void KexiAlterTableDialog::removeCurrentPropertySet()
{
	const int r = m_view->currentRow();
	KoProperty::Set *buf = d->sets.at(r);
	if (!buf)
		return;
	buf->debug();
//	m_currentBufferCleared = true;
	d->sets.remove(r);
	propertysetswitched();
//	delete buf;
//	m_currentBufferCleared = false;
}
*/

void KexiAlterTableDialog::slotBeforeCellChanged(
	KexiTableItem *item, int colnum, QVariant& newValue, KexiDB::ResultInfo* /*result*/)
{
	if (colnum==COLUMN_ID_NAME) {//'name'
//		if (!item->at(1).toString().isEmpty() && item->at(1).isNull()) {
		//if 'type' is not filled yet
		if (item->at(COLUMN_ID_TYPE).isNull()) {
			//auto select 1st row of 'type' column
			m_view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, QVariant((int)0));
		}

		if (propertySet()) {
			//update field name
			KoProperty::Set &set = *propertySet();
			set["name"] = newValue;
		}
	}
	else if (colnum==COLUMN_ID_TYPE) {//'type'
		if (newValue.isNull()) {
			//'type' col will be cleared: clear 'name' column as well
			m_view->data()->updateRowEditBuffer(item, COLUMN_ID_NAME, QVariant(QString::null));
			return;
		}

		if (!propertySet())
			return;

		KoProperty::Set &set = *propertySet();
		//'type' col is changed (existed before)
		//-get type group number
		KexiDB::Field::TypeGroup fieldTypeGroup;
		int i_fieldTypeGroup = newValue.toInt()+1/*counting from 1*/;
		if (i_fieldTypeGroup < 1 || i_fieldTypeGroup >
#ifdef KEXI_SHOW_UNIMPLEMENTED
			(int)KexiDB::Field::LastTypeGroup)
#else
//TODO: remove this later
			(int)KexiDB::Field::LastTypeGroup-1) //don't show last (BLOB) type
#endif
			return;
		fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>(i_fieldTypeGroup);

		//-get 1st type from this group, and update 'type' property
		KexiDB::Field::Type fieldType = KexiDB::defaultTypeForGroup( fieldTypeGroup );
		if (fieldType==KexiDB::Field::InvalidType)
			fieldType = KexiDB::Field::Text;
//		set["type"] = (int)fieldType;
//		set["subType"] = KexiDB::Field::typeName(fieldType);

		//-get subtypes for this type: keys (slist) and names (nlist)
		const QStringList slist = KexiDB::typeStringsForGroup(fieldTypeGroup);
		const QStringList nlist = KexiDB::typeNamesForGroup(fieldTypeGroup);
		KoProperty::Property *subTypeProperty = &set["subType"];

		//update subtype list and value
		subTypeProperty->setListData( nlist, slist );
		if (set["primaryKey"].value().toBool()==true) //primary keys require big int
			fieldType = KexiDB::Field::BigInteger;
		subTypeProperty->setValue( KexiDB::Field::typeString(fieldType) );
		if (updatePropertiesVisibility(fieldType, set)) {
			//properties' visiblility changed: refresh prop. set
			propertySetReloaded(true);
		}
	}
	else if (colnum==COLUMN_ID_DESC) {//'description'
		if (!propertySet())
			return;

		//update field desc.
		KoProperty::Set &set = *propertySet();
		set["description"] = newValue; //item->at(COLUMN_ID_DESC);
	}
}

void KexiAlterTableDialog::slotRowUpdated(KexiTableItem *item)
{
	setDirty();

	//-check if the row was empty before updating
	//if yes: we want to add a property set for this new row (field)
	QString fieldName = item->at(COLUMN_ID_NAME).toString();
//	const bool buffer_allowed = !fieldName.isEmpty() && !item->at(1).isNull();
	const bool prop_set_allowed = !item->at(COLUMN_ID_TYPE).isNull();

	if (!prop_set_allowed && propertySet()) {
		//there is a property set, but it's not allowed - remove it:
		d->sets->removeCurrentPropertySet();

		//clear 'type' column:
		m_view->data()->clearRowEditBuffer();
		m_view->data()->updateRowEditBuffer(m_view->selectedItem(), COLUMN_ID_TYPE, QVariant());
		m_view->data()->saveRowChanges(*m_view->selectedItem());

	} else if (prop_set_allowed && !propertySet()) {
		//-- create a new field:
		KexiDB::Field::TypeGroup fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>( item->at(COLUMN_ID_TYPE).toInt()+1/*counting from 1*/ );
		int fieldType = KexiDB::defaultTypeForGroup( fieldTypeGroup );
		if (fieldType==0)
			return;

		QString description = item->at(COLUMN_ID_DESC).toString();

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

		//create a new property set:
		createPropertySet( m_view->currentRow(), &field, true );
//moved
		//add a special property indicating that this is brand new buffer,
		//not just changed
//		KoProperty::Property* prop = new KoProperty::Property("newrow", QVariant());
//		prop->setVisible(false);
//		newbuff->addProperty( prop );

		//refresh property editor:
		propertySetSwitched();
	}
}

void KexiAlterTableDialog::updateActions()
{
	updateActions(false);
}

void KexiAlterTableDialog::slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property)
{
	const QCString pname = property.name();
	if (pname=="primaryKey") {
		if (property.value().toBool()) {
			//primary key implies some rules
			set["unique"] = QVariant(true,1);
			set["notNull"] = QVariant(true,1);
			set["allowEmpty"] = QVariant(false,1);
			set["indexed"] = QVariant(true,1);
//! \todo: add setting for this: "Integer PKeys have autonumber set by default"
			set["autoIncrement"] = QVariant(true,1);
		}
		else {
			set["autoIncrement"] = QVariant(false,1);
		}
		setPrimaryKey(set, property.value().toBool());
		updatePropertiesVisibility(
			KexiDB::Field::typeForString( set["subType"].value().toString() ), set);
		//properties' visiblility changed: refresh prop. set
		propertySetReloaded(true/*preservePrevSelection*/);
	}
//TODO: perhaps show a hint in help panel telling what happens?
	else if (property.value().toBool()==false
		&& (pname=="indexed" || pname=="unique" || pname=="notNull"))
	{
//			set["primaryKey"] = QVariant(false,1);
		setPrimaryKey(set, false);
		if (pname=="notNull")
			set["allowEmpty"] = QVariant(true,1);
	}
	else if (pname=="subType") {
		if (set["primaryKey"].value().toBool()==true && property.value().toString()!=KexiDB::Field::typeString(KexiDB::Field::BigInteger)) {
			kdDebug() << "INVALID " << property.value().toString() << endl;
//			if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
//				i18n("This field has promary key assigned. Setting autonumber field"),
//				KGuiItem(i18n("Create &Primary Key"), "key"), KStdGuiItem::cancel() ))

		}
//		kdDebug() << property.value().toString() << endl;
//		kdDebug() << set["type"].value() << endl;
		if (KexiDB::Field::typeGroup( set["type"].value().toInt() ) == (int)KexiDB::Field::TextGroup) {
			updatePropertiesVisibility(KexiDB::Field::typeForString(property.value().toString()), set);
			//properties' visiblility changed: refresh prop. set
			propertySetReloaded(true);
		}
	}
	else {//prop==true:
		if (property.value().toBool()==true && pname=="autoIncrement") {
			if (set["primaryKey"].value().toBool()==false) {//we need PKEY here!
				QString msg = QString("<p>")
					+i18n("Setting autonumber requires primary key to be set for current field.")+"</p>";
				if (d->primaryKeyExists)
					msg += (QString("<p>")+ i18n("Previous primary key will be removed.")+"</p>");
				msg += (QString("<p>")
					+i18n("Do you want to create primary key for current field? "
					"Click \"Cancel\" to cancel setting autonumber.")+"</p>");

				if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
					i18n("Setting autonumber field"),
					KGuiItem(i18n("Create &Primary Key"), "key"), KStdGuiItem::cancel() ))
				{
					setPrimaryKey(set, true);
				}
				else {
					set["autoIncrement"].setValue( QVariant(false,1), false/*don't save old*/);
				}
			}
		}
	}
}

void KexiAlterTableDialog::slotAboutToInsertRow(KexiTableItem* /*item*/,
	KexiDB::ResultInfo* /*result*/, bool /*repaint*/)
{
	setDirty();
	//TODO
}

tristate KexiAlterTableDialog::buildSchema(KexiDB::TableSchema &schema)
{
	if (!m_view->acceptRowEdit())
		return cancelled;

	tristate res = true;
	//check for pkey; automatically add a pkey if user wanted
	if (!d->primaryKeyExists) {
		const int questionRes = KMessageBox::questionYesNoCancel(this,
			i18n("<p>Table \"%1\" has no <b>primary key</b> defined.</p>"
			"<p>Although a primary key is not required, it is needed "
			"for creating relations between database tables. "
			"Do you want to add primary key automatically now?</p>"
			"<p>If you want to add a primary key by hand, press \"Cancel\" "
			"to cancel saving table design.</p>").arg(schema.name()),
			QString::null, KGuiItem(i18n("&Add Primary Key"), "key"), KStdGuiItem::no(),
				"autogeneratePrimaryKeysOnTableDesignSaving");
		if (questionRes==KMessageBox::Cancel) {
			return cancelled;
		}
		else if (questionRes==KMessageBox::Yes) {
			m_view->insertEmptyRow(0);
			m_view->setCursorPosition(0, COLUMN_ID_NAME);
			//name and type
			m_view->data()->updateRowEditBuffer(m_view->selectedItem(), COLUMN_ID_NAME,
				QVariant("id"));
			m_view->data()->updateRowEditBuffer(m_view->selectedItem(), COLUMN_ID_TYPE,
				QVariant(KexiDB::Field::IntegerGroup-1/*counting from 0*/));
			if (!m_view->data()->saveRowChanges(*m_view->selectedItem(), true)) {
				return cancelled;
			}
			slotTogglePrimaryKey();
		}
	}

	//check for duplicates
	KoProperty::Set *b = 0;
	bool no_fields = true;
	int i;
	QDict<char> names(101, false);
	char dummy;
	for (i=0;i<(int)d->sets->size();i++) {
		b = d->sets->at(i);
		if (b) {
			no_fields = false;
			const QString name = (*b)["name"].value().toString();
			if (name.isEmpty()) {
				m_view->setCursorPosition(i, COLUMN_ID_NAME);
				m_view->startEditCurrentCell();
				KMessageBox::information(this, i18n("You should enter field name.") );
				res = cancelled;
				break;
			}
			if (names[name]) {
				break;
			}
			names.insert( name, &dummy ); //remember
		}
	}
	if (res && no_fields) {//no fields added
		KMessageBox::sorry(this,
			i18n("You have added no fields.\nEvery table should have at least one field.") );
		res = cancelled;
	}
	if (res && b && i<(int)d->sets->size()) {//found a duplicate
		m_view->setCursorPosition(i, COLUMN_ID_NAME);
		m_view->startEditCurrentCell();
		KMessageBox::sorry(this,
			i18n("You have added \"%1\" field name twice.\nField names cannot be repeated. "
			"Correct name of the field.")
			.arg((*b)["name"].value().toString()) );
		res = cancelled;
	}
	if (res) {
		//for every field, create KexiDB::Field definition
		for (i=0;i<(int)d->sets->size();i++) {
			KoProperty::Set *s = d->sets->at(i);
			if (!s)
				continue;
			KoProperty::Set &set = *s;

			kdDebug() << set["subType"].value().toString() << endl;
	//		int typeGroup = KexiDB::typeGroup(type);
			QString typeString = set["subType"].value().toString();
			KexiDB::Field::Type type = KexiDB::Field::typeForString(typeString);
			if (type==KexiDB::Field::InvalidType)
				type = KexiDB::Field::Text;

			uint constraints = 0;
			uint options = 0;
			if (set["primaryKey"].value().toBool())
				constraints |= KexiDB::Field::PrimaryKey;
			if (set["autoIncrement"].value().toBool() && KexiDB::Field::isAutoIncrementAllowed(type))
				constraints |= KexiDB::Field::AutoInc;
			if (set["unique"].value().toBool())
				constraints |= KexiDB::Field::Unique;
			if (set["notnull"].value().toBool())
				constraints |= KexiDB::Field::NotNull;
			if (!set["allowEmpty"].value().toBool())
				constraints |= KexiDB::Field::NotEmpty;

			if (set["unsigned"].value().toBool())
				options |= KexiDB::Field::Unsigned;

	//		int type = set["type"].value().toInt();
	//		if (type < 0 || type > (int)KexiDB::Field::LastType)
	//			type = KexiDB::Field::Text;

			KexiDB::Field *f = new KexiDB::Field(
				set["name"].value().toString(),
				type,
				constraints,
				options,
				set["length"].value().toInt(),
				set["precision"].value().toInt(),
				set["defaultValue"].value(),
				set["caption"].value().toString(),
				set["description"].value().toString(),
				set["width"].value().toInt()
			);
			schema.addField(f);
		}
	}
	return res;
}

KexiDB::SchemaData* KexiAlterTableDialog::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	if (tempData()->table || m_dialog->schemaData()) //must not be
		return 0;

	//create table schema definition
	tempData()->table = new KexiDB::TableSchema(sdata.name());
	tempData()->table->setName( sdata.name() );
	tempData()->table->setCaption( sdata.caption() );
	tempData()->table->setDescription( sdata.description() );

	tristate res = buildSchema(*tempData()->table);
	cancel = ~res;

	//FINALLY: create table:
	if (res) {
		//todo
		KexiDB::Connection *conn = mainWin()->project()->dbConnection();
		res = conn->createTable(tempData()->table);
		if (res!=true)
			parentDialog()->setStatus(conn, "");
	}

	if (res) {
		//we've current schema
		tempData()->tableSchemaChangedInPreviousView = true;
//not needed; KexiProject emits newItemStored signal //let project know the table is created
//		mainWin()->project()->emitTableCreated(*tempData()->table);
	}
	else {
		delete tempData()->table;
		tempData()->table = 0;
	}
	return tempData()->table;
}

tristate KexiAlterTableDialog::storeData()
{
	if (!tempData()->table || !m_dialog->schemaData())
		return 0;

	tristate res = true;
	if (!d->dontAskOnStoreData) {
		bool emptyTable;
		const QString msg = messageForSavingChanges(emptyTable);
		if (!emptyTable) {
			if (KMessageBox::No == KMessageBox::questionYesNo(this, msg))
				res = cancelled;
		}
	}
	d->dontAskOnStoreData = false; //one-time use
	if (~res)
		return res;
//		KMessageBox::information(this, i18n("Saving changes for existing table design is not yet supported."));
//		cancel = true;

	KexiDB::TableSchema *newTable = new KexiDB::TableSchema();
	//copy schema data
	static_cast<KexiDB::SchemaData&>(*newTable) = static_cast<KexiDB::SchemaData&>(*tempData()->table);
	res = buildSchema(*newTable);
//	bool ok = buildSchema(*newTable, cancel) && !cancel;

	kdDebug() << "KexiAlterTableDialog::storeData() : BUILD SCHEMA:" << endl;
	newTable->debug();

	KexiDB::Connection *conn = mainWin()->project()->dbConnection();
	if (res) {
		res = KexiTablePart::askForClosingObjectsUsingTableSchema(
			this, *conn, *tempData()->table,
			i18n("You are about to change the design of table \"%1\" "
			"but following objects using this table are opened:")
			.arg(tempData()->table->name()));
	}
	if (res) {
		res = conn->alterTable(*tempData()->table, *newTable);
		if (!res)
			parentDialog()->setStatus(conn, "");
	}
	if (res) {
		//change current schema
		tempData()->table = newTable;
		tempData()->tableSchemaChangedInPreviousView = true;
	}
	else {
		delete newTable;
	}
	return res;
}

KexiTablePart::TempData* KexiAlterTableDialog::tempData() const
{
	return static_cast<KexiTablePart::TempData*>(parentDialog()->tempData());
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

