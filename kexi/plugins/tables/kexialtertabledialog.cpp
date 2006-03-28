/* This file is part of the KDE project
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexialtertabledialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kmenu.h>
#include <kmessagebox.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/error.h>
#include <kexiutils/identifier.h>
#include <kexiproject.h>
#include <keximainwindow.h>
#include <widget/tableview/kexidataawarepropertyset.h>
#include <widget/kexicustompropertyfactory.h>
#include <kexidialogbase.h>
#include <kexitableview.h>

//#define MAX_FIELDS 101 //nice prime number

//! indices for table columns
#define COLUMN_ID_PK 0
//#define COLUMN_ID_NAME 1
#define COLUMN_ID_CAPTION 1
#define COLUMN_ID_TYPE 2
#define COLUMN_ID_DESC 3

//#define KexiAlterTableDialog_DEBUG

//! @todo remove this when BLOBs are implemented
#define KEXI_NO_BLOB_FIELDS

//! @internal
class KexiAlterTableDialogPrivate
{
	public:
		KexiAlterTableDialogPrivate()
		 : sets(0)
		 , dontAskOnStoreData(false)
		 , slotTogglePrimaryKeyCalled(false)
		 , primaryKeyExists(false)
		 , slotPropertyChanged_primaryQt::Key_enabled(true)
		 , slotPropertyChanged_subType_enabled(true)
		{
		}

		~KexiAlterTableDialogPrivate() {
			delete sets;
		}
		
		KexiTableView *view; //!< helper

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
		//! Used in slotPropertyChanged() to avoid infinite recursion
		bool slotPropertyChanged_primaryQt::Key_enabled : 1;
		//! Used in slotPropertyChanged() to avoid infinite recursion
		bool slotPropertyChanged_subType_enabled : 1;
};

//----------------------------------------------

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent,
	const char *name)
 : KexiDataTable(win, parent, name, false/*not db-aware*/)
 , d( new KexiAlterTableDialogPrivate() )
{
	//needed for custom "identifier" property editor widget
	KexiCustomPropertyFactory::init();

	KexiDB::Connection *conn = mainWin()->project()->dbConnection();

	d->data = new KexiTableViewData();
	if (conn->isReadOnly())
		d->data->setReadOnly(true);
	d->data->setInsertingEnabled( false );

	KexiTableViewColumn *col = new KexiTableViewColumn("pk", KexiDB::Field::Text, i18n("Primary Key", "PK"),
		i18n("Describes primary key for the field."));
	col->field()->setSubType("K3Icon");
	col->setReadOnly(true);
	d->data->addColumn( col );

//	col = new KexiTableViewColumn("name", KexiDB::Field::Text, i18n("Field Name"),
	col = new KexiTableViewColumn("caption", KexiDB::Field::Text, i18n("Field Caption"),
		i18n("Describes name for the field."));
//	KexiUtils::Validator *vd = new KexiUtils::IdentifierValidator();
//	vd->setAcceptsEmptyValue(true);
//	col->setValidator( vd );
	d->data->addColumn( col );

	col = new KexiTableViewColumn("type", KexiDB::Field::Enum, i18n("Data Type"),
		i18n("Describes data type for the field."));
	d->data->addColumn( col );

#ifdef KEXI_NO_BLOB_FIELDS
//! @todo remove this later
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup-1); //don't show last type (BLOB)
#else
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup);
#endif
	d->maxTypeNameTextWidth = 0;
	QFontMetrics fm(font());
	for (uint i=1; i<=types.count(); i++) {
		types[i-1] = KexiDB::Field::typeGroupName(i);
		d->maxTypeNameTextWidth = qMax(d->maxTypeNameTextWidth, fm.width(types[i-1]));
	}
	col->field()->setEnumHints(types);

	d->data->addColumn( col = new KexiTableViewColumn("comments", KexiDB::Field::Text, i18n("Comments"),
		i18n("Describes additional comments for the field.")) );

	d->view = dynamic_cast<KexiTableView*>(mainWidget());

	d->view->setSpreadSheetMode();
//	setFocusProxy(d->view);

	connect(d->data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	connect(d->data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
		this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));
	connect(d->data, SIGNAL(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)),
		this, SLOT(slotAboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)));

	setMinimumSize(d->view->minimumSizeHint().width(), d->view->minimumSizeHint().height());
	d->view->setFocus();

	d->sets = new KexiDataAwarePropertySet( this, d->view );
	connect(d->sets, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
	connect(d->sets, SIGNAL(rowInserted()), this, SLOT(updateActions()));

	plugSharedAction("tablepart_toggle_pkey", this, SLOT(slotTogglePrimaryKey()));
	d->action_toggle_pkey = static_cast<KToggleAction*>( sharedAction("tablepart_toggle_pkey") );
	d->action_toggle_pkey->plug(d->view->contextMenu(), 0); //add at the beg.
	setAvailable("tablepart_toggle_pkey", !conn->isReadOnly());
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
			(*item)[1] = field->captionOrName();
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
//	const int columnsCount = d->data->columnsCount();
	for (int i=tableFieldCount; i<(int)d->sets->size(); i++) {
//		KexiTableItem *item = new KexiTableItem(columnsCount);//3 empty fields
		d->data->append(d->data->createItem());
	}

	//set data for our spreadsheet: this will clear our sets
	d->view->setData(d->data);

	//now recreate property sets
	if (tempData()->table) {
		for(int i=0; i < tableFieldCount; i++) {
			KexiDB::Field *field = tempData()->table->field(i);
			createPropertySet( i, field );
		}
	}

	//column widths
	d->view->setColumnWidth(COLUMN_ID_PK, IconSize( K3Icon::Small ) + 10);
	d->view->adjustColumnWidthToContents(COLUMN_ID_CAPTION); //adjust column width
	d->view->setColumnWidth(COLUMN_ID_TYPE, d->maxTypeNameTextWidth + 2 * d->view->rowHeight());
	d->view->setColumnStretchEnabled( true, COLUMN_ID_DESC ); //last column occupies the rest of the area

	setDirty(false);
	d->view->setCursorPosition(0, COLUMN_ID_CAPTION); //set @ name column
}

static bool updatePropertiesVisibility(KexiDB::Field::Type fieldType, KoProperty::Set &set)
{
	bool changed = false;
	KoProperty::Property *prop;
	bool visible;
	
	prop = &set["subType"];
	const bool isObjectTypeGroup = set["type"].value().toInt() == (int)KexiDB::Field::BLOB;
	kDebug() << prop->value().toInt() << set["type"].value().toInt()<< endl;
	
	//if there is no more than 1 subType name or it's a PK: hide the property
	visible = (prop->listData() && prop->listData()->keys.count() > 1 || isObjectTypeGroup)
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
	prop = &set["unique"];
	visible = fieldType != KexiDB::Field::BLOB;
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &set["indexed"];
	visible = fieldType != KexiDB::Field::BLOB;
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
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

//! Gets subtype strings and names for type \a fieldType
void
KexiAlterTableDialog::getSubTypeListData(KexiDB::Field::TypeGroup fieldTypeGroup, 
	QStringList& stringsList, QStringList& namesList)
{
	if (fieldTypeGroup==KexiDB::Field::BLOBGroup) {
		// special case: BLOB type uses "mime-based" subtypes
//! @todo hardcoded!
		stringsList << "image";
		namesList << i18n("Image object type", "Image");
	}
	else {
		stringsList = KexiDB::typeStringsForGroup(fieldTypeGroup);
		namesList = KexiDB::typeNamesForGroup(fieldTypeGroup);
	}
	kDebug() << "KexiAlterTableDialog::getSubTypeListData(): subType strings: " << 
		stringsList.join("|") << "\nnames: " << namesList.join("|") << endl;
}

KoProperty::Set *
KexiAlterTableDialog::createPropertySet( int row, KexiDB::Field *field, bool newOne )
{
	QString typeName = "KexiDB::Field::" + field->typeGroupString();
	KoProperty::Set *set = new KoProperty::Set(d->sets, typeName);
	if (mainWin()->project()->dbConnection()->isReadOnly())
		set->setReadOnly( true );
//	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
//		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));

	KoProperty::Property *prop;

	//meta-info for property editor
	set->addProperty(prop = new KoProperty::Property("this:classString", i18n("Table field")) );
	prop->setVisible(false);
//! \todo add table_field icon (add	buff->addProperty(prop = new KexiProperty("this:iconName", "table_field") );
//	prop->setVisible(false);

	//name
	set->addProperty(prop 
		= new KoProperty::Property("name", QVariant(field->name()), i18n("Name"), 
		QString::null, KexiCustomPropertyFactory::Identifier) );

	//type
	set->addProperty( prop 
		= new KoProperty::Property("type", QVariant(field->type()), i18n("Type")) );
#ifndef KexiAlterTableDialog_DEBUG
	prop->setVisible(false);//always hidden
#endif

	//subtype
	QStringList slist, nlist;
	getSubTypeListData(field->typeGroup(), slist, nlist);
	QString subTypeValue;
	if (field->typeGroup()==KexiDB::Field::BLOBGroup) {
// special case: BLOB type uses "mime-based" subtypes
//! @todo this should be retrieved from KexiDB::Field when BLOB supports many different mimetypes
		subTypeValue = slist.first();
	}
	else {
		subTypeValue = field->typeString();
	}
	set->addProperty(prop 
		= new KoProperty::Property("subType", slist, nlist, subTypeValue, i18n("Subtype")));

	set->addProperty( prop 
		= new KoProperty::Property("caption", QVariant(field->caption()), i18n("Caption") ) );
	prop->setVisible(false);//always hidden

	set->addProperty( prop 
		= new KoProperty::Property("description", QVariant(field->description())) );
	prop->setVisible(false);//always hidden

	set->addProperty(prop 
		= new KoProperty::Property("unsigned", QVariant(field->isUnsigned(), 4), i18n("Unsigned Number")));

	set->addProperty( prop 
		= new KoProperty::Property("length", (int)field->length()/*200?*/, i18n("Length")));

	set->addProperty( prop 
		= new KoProperty::Property("precision", (int)field->precision()/*200?*/, i18n("Precision")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

//! @todo set reasonable default for column width
	set->addProperty( prop 
		= new KoProperty::Property("width", (int)field->width()/*200?*/, i18n("Column Width")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

	set->addProperty( prop 
		= new KoProperty::Property("defaultValue", field->defaultValue(), i18n("Default Value")));
//! @todo show this after we get properly working editor for QVariant
	prop->setVisible(false);

	set->addProperty(prop 
		= new KoProperty::Property("primaryKey", QVariant(field->isPrimaryKey(), 4), i18n("Primary Key")));
	prop->setIcon("key");

	set->addProperty(
		new KoProperty::Property("unique", QVariant(field->isUniqueKey(), 4), i18n("Unique")));

	set->addProperty(
		new KoProperty::Property("notNull", QVariant(field->isNotNull(), 4), i18n("Required")));
	
	set->addProperty(
		new KoProperty::Property("allowEmpty", QVariant(!field->isNotEmpty(), 4), i18n("Allow Zero\nSize")));

	set->addProperty(prop 
		= new KoProperty::Property("autoIncrement", QVariant(field->isAutoIncrement(), 4), i18n("Autonumber")));
	prop->setIcon("autonumber");

	set->addProperty(
		new KoProperty::Property("indexed", QVariant(field->isIndexed(), 4), i18n("Indexed")));

	updatePropertiesVisibility(field->type(), *set);

	connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

	d->sets->insert(row, set, newOne);
	return set;
}

void KexiAlterTableDialog::updateActions(bool activated)
{
	Q_UNUSED(activated);
/*! \todo check if we can set pkey for this column type (eg. BLOB?) */
	setAvailable("tablepart_toggle_pkey", propertySet()!=0 && !mainWin()->project()->dbConnection()->isReadOnly());
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

void KexiAlterTableDialog::setPrimaryKey(KoProperty::Set &propertySet, bool set, bool aWasPKey)
{
	const bool was_pkey = aWasPKey || propertySet["primaryKey"].value().toBool();
	propertySet["primaryKey"] = QVariant(set, 1);
	if (&propertySet==this->propertySet()) {
		//update action and icon @ column 0 (only if we're changing current property set)
		d->action_toggle_pkey->setChecked(set);
		if (d->view->selectedItem()) {
			//show key in the table
			d->view->data()->clearRowEditBuffer();
			d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_PK, 
				QVariant(set ? "key" : ""));
			d->view->data()->saveRowChanges(*d->view->selectedItem(), true);
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
			if (s && s!=&propertySet && (*s)["primaryKey"].value().toBool() && i!=d->view->currentRow())
				break;
		}
		if (i<count) {//remove
			(*s)["autoIncrement"] = QVariant(false, 0);
			(*s)["primaryKey"] = QVariant(false, 0);
			//remove key from table
			d->view->data()->clearRowEditBuffer();
			KexiTableItem *item = d->view->itemAt(i);
			if (item) {
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_PK, QVariant());
				d->view->data()->saveRowChanges(*item, true);
			}
		}
		//set unsigned big-integer type
//		d->view->data()->saveRowChanges(*d->view->selectedItem());
		d->view->data()->clearRowEditBuffer();
		d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE,
			QVariant(KexiDB::Field::IntegerGroup-1/*counting from 0*/));
//			QVariant(KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup)));
		d->view->data()->saveRowChanges(*d->view->selectedItem(), true);
//		propertySet["type"] = KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup);
//		propertySet["type"] = (int)KexiDB::Field::IntegerGroup;
		propertySet["subType"] = KexiDB::Field::typeString(KexiDB::Field::BigInteger);
		propertySet["unsigned"] = QVariant(true,4);
	}
	updateActions();
}

/*void KexiAlterTableDialog::slotCellSelected(int, int row)
{
	kDebug() << "KexiAlterTableDialog::slotCellSelected()" << endl;
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
		(QString("\n\n") + part()->i18nMessage(":additional message before saving design", parentDialog())) );
//		QString("\n\n") + i18n("Note: This table is already filled with data which will be removed.") );
}

tristate KexiAlterTableDialog::beforeSwitchTo(int mode, bool &dontStore)
{
	if (!d->view->acceptRowEdit())
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
	const int r = d->view->currentRow();
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
//	kDebug() << d->view->selectedItem() << " " << item 
		//<< " " << d->sets->at( d->view->currentRow() ) << " " << propertySet() << endl;
	if (colnum==COLUMN_ID_CAPTION) {//'caption'
//		if (!item->at(1).toString().isEmpty() && item->at(1).isNull()) {
		//if 'type' is not filled yet
		if (item->at(COLUMN_ID_TYPE).isNull()) {
			//auto select 1st row of 'type' column
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, QVariant((int)0));
		}

		if (propertySet()) {
			KoProperty::Set &set = *propertySet();
			//update field caption and name
			set["caption"] = newValue;
			set["name"] = newValue; // "name" prop. is of custom type Identifier, so this assignment 
			                        // will automatically convert newValue to an valid identifier
		}
	}
	else if (colnum==COLUMN_ID_TYPE) {//'type'
		if (newValue.isNull()) {
			//'type' col will be cleared: clear all other columns as well
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_PK, QVariant());
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_CAPTION, QVariant(QString::null));
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_DESC, QVariant());
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
#ifdef KEXI_NO_BLOB_FIELDS
//! @todo remove this later
			(int)KexiDB::Field::LastTypeGroup-1) //don't show last (BLOB) type
#else
			(int)KexiDB::Field::LastTypeGroup)
#endif
			return;
		fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>(i_fieldTypeGroup);

		//-get 1st type from this group, and update 'type' property
		KexiDB::Field::Type fieldType = KexiDB::defaultTypeForGroup( fieldTypeGroup );
		if (fieldType==KexiDB::Field::InvalidType)
			fieldType = KexiDB::Field::Text;
		set["type"] = (int)fieldType;
//		set["subType"] = KexiDB::Field::typeName(fieldType);
		
		//-get subtypes for this type: keys (slist) and names (nlist)
		QStringList slist, nlist;
		getSubTypeListData(fieldTypeGroup, slist, nlist);

		QString subTypeValue;
		if (fieldType==KexiDB::Field::BLOB) {
			// special case: BLOB type uses "mime-based" subtypes
			subTypeValue = slist.first();
		}
		else {
			subTypeValue = KexiDB::Field::typeString(fieldType);
		}
		KoProperty::Property *subTypeProperty = &set["subType"];
kDebug() << "++++++++++" << slist << nlist << endl;

		//update subtype list and value
		const bool forcePropertySetReload = set["type"].value().toInt() != (int)fieldTypeGroup;
		const bool useListData = slist.count() > 1 || fieldType==KexiDB::Field::BLOB;
		if (useListData) {
			subTypeProperty->setListData( slist, nlist );
		}
		else {
			subTypeProperty->setListData( 0 );
		}
		if (set["primaryKey"].value().toBool()==true) {
			//primary keys require big int, so if selected type is not integer- remove PK
			if (fieldTypeGroup != KexiDB::Field::IntegerGroup) {
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_PK, QVariant());
				set["primaryKey"] = QVariant(false, 1);
//! @todo should we display (passive?) dialog informing about cleared pkey?
			}
		}
		if (useListData)
			subTypeProperty->setValue( subTypeValue, false/*!rememberOldValue*/ );
		if (updatePropertiesVisibility(fieldType, set) || forcePropertySetReload) {
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
	QString fieldCaption( item->at(COLUMN_ID_CAPTION).toString() );
//	const bool buffer_allowed = !fieldName.isEmpty() && !item->at(1).isNull();
	const bool prop_set_allowed = !item->at(COLUMN_ID_TYPE).isNull();

	if (!prop_set_allowed && propertySet()) {
		//there is a property set, but it's not allowed - remove it:
		d->sets->removeCurrentPropertySet();

		//clear 'type' column:
		d->view->data()->clearRowEditBuffer();
		d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE, QVariant());
		d->view->data()->saveRowChanges(*d->view->selectedItem());

	} else if (prop_set_allowed && !propertySet()) {
		//-- create a new field:
		KexiDB::Field::TypeGroup fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>( 
			item->at(COLUMN_ID_TYPE).toInt()+1/*counting from 1*/ );
		int fieldType = KexiDB::defaultTypeForGroup( fieldTypeGroup );
		if (fieldType==0)
			return;

		QString description( item->at(COLUMN_ID_DESC).toString() );

//todo: check uniqueness:
		QString fieldName( KexiUtils::string2Identifier(fieldCaption) );

		KexiDB::Field field( //tmp
			fieldName,
			(KexiDB::Field::Type)fieldType,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			/*length*/0,
			/*precision*/0,
			/*defaultValue*/QVariant(),
			fieldCaption,
			description,
			/*width*/0);
//		m_newTable->addField( field );

		kDebug() << "KexiAlterTableDialog::slotRowUpdated(): " << field.debugString() << endl;

		//create a new property set:
		createPropertySet( d->view->currentRow(), &field, true );
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
	kexipluginsdbg << "KexiAlterTableDialog::slotPropertyChanged(): " << pname << " = " << property.value() << endl;
	if (pname=="primaryKey" && d->slotPropertyChanged_primaryQt::Key_enabled) {
		d->slotPropertyChanged_primaryQt::Key_enabled = false;
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
		setPrimaryKey(set, property.value().toBool(), true/*wasPKey*/);
		updatePropertiesVisibility(
			KexiDB::Field::typeForString( set["subType"].value().toString() ), set);
		//properties' visiblility changed: refresh prop. set
		propertySetReloaded(true/*preservePrevSelection*/);
		d->slotPropertyChanged_primaryQt::Key_enabled = true;
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
	else if (pname=="subType" && d->slotPropertyChanged_subType_enabled) {
		d->slotPropertyChanged_subType_enabled = false;
		if (set["primaryKey"].value().toBool()==true && property.value().toString()!=KexiDB::Field::typeString(KexiDB::Field::BigInteger)) {
			kDebug() << "INVALID " << property.value().toString() << endl;
//			if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
//				i18n("This field has promary key assigned. Setting autonumber field"),
//				KGuiItem(i18n("Create &Primary Key"), "key"), KStdGuiItem::cancel() ))

		}
//		kDebug() << property.value().toString() << endl;
//		kDebug() << set["type"].value() << endl;
		if (KexiDB::Field::typeGroup( set["type"].value().toInt() ) == (int)KexiDB::Field::TextGroup) {
			updatePropertiesVisibility(KexiDB::Field::typeForString(property.value().toString()), set);
			//properties' visiblility changed: refresh prop. set
			propertySetReloaded(true);
		}
		d->slotPropertyChanged_subType_enabled = true;
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

void KexiAlterTableDialog::slotAboutToDeleteRow(
	KexiTableItem& item, KexiDB::ResultInfo* result, bool repaint)
{
	Q_UNUSED(result)
	Q_UNUSED(repaint)
	if (item[COLUMN_ID_PK].toString()=="key")
		d->primaryKeyExists = false;
}

tristate KexiAlterTableDialog::buildSchema(KexiDB::TableSchema &schema)
{
	if (!d->view->acceptRowEdit())
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
			//-find unique name, starting with, "id", "id2", ....
			int i=0;
			int idIndex = 1; //means "id"
			QString pkFieldName("id%1");
			QString pkFieldCaption(i18n("Identifier%1", "Id%1"));
			while (i<(int)d->sets->size()) {
				KoProperty::Set *set = d->sets->at(i);
				if (set) {
					if ((*set)["name"].value().toString()
						== pkFieldName.arg(idIndex==1?QString::null : QString::number(idIndex))
					|| (*set)["caption"].value().toString()
					== pkFieldCaption.arg(idIndex==1?QString::null : QString::number(idIndex)))
					{
						//try next id index
						i = 0;
						idIndex++;
						continue;
					}
				}
				i++;
			}
			pkFieldName = pkFieldName.arg(idIndex==1?QString::null : QString::number(idIndex));
			pkFieldCaption = pkFieldCaption.arg(idIndex==1?QString::null : QString::number(idIndex));
			//ok, add PK with such unique name
			d->view->insertEmptyRow(0);
			d->view->setCursorPosition(0, COLUMN_ID_CAPTION);
			d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_CAPTION,
				QVariant(pkFieldCaption));
			d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE,
				QVariant(KexiDB::Field::IntegerGroup-1/*counting from 0*/));
			if (!d->view->data()->saveRowChanges(*d->view->selectedItem(), true)) {
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
				d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
				d->view->startEditCurrentCell();
				KMessageBox::information(this, i18n("You should enter field caption.") );
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
		d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
		d->view->startEditCurrentCell();
//! @todo for "names hidden" mode we won't get this error because user is unable to change names
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

			kexipluginsdbg << set["subType"].value().toString() << endl;
//			int i_type = set["type"].value().toInt();
			QString subTypeString( set["subType"].value().toString() );
/*			if (type == (int)KexiDB::Field::BLOB) {
//! @todo hardcoded! image is the only subtype for now
			}
			else {
				QString subTypeString = set["subType"].value().toString();
				KexiDB::Field::Type type = KexiDB::Field::typeForString(subTypeString);
			}*/
			KexiDB::Field::Type type = KexiDB::Field::typeForString(subTypeString);
//			KexiDB::Field::Type type;
			if (type <= (int)KexiDB::Field::InvalidType || type > (int)KexiDB::Field::LastType) {//for sanity
				type = KexiDB::Field::Text;
				kexipluginswarn << "KexiAlterTableDialog::buildSchema(): invalid type " << type 
					<< ", moving back to Text type" << endl;
			}
//			else 
//				type = (KexiDB::Field::Type)i_type;

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

tristate KexiAlterTableDialog::storeData(bool dontAsk)
{
	if (!tempData()->table || !m_dialog->schemaData())
		return 0;

	tristate res = true;
	if (!d->dontAskOnStoreData && !dontAsk) {
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

	kDebug() << "KexiAlterTableDialog::storeData() : BUILD SCHEMA:" << endl;
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
	QVariant *buf_type = buffer->at( d->view->field(1)->name() );

	//check if there is a type specified
//	if ((old_type.isNull() && !buf_type) || (buf_type && buf_type->isNull())) {
		//kDebug() << "err" << endl;
	//}
//	allow = true;
//	m_dirty = m_dirty | result->success;
}*/



#include "kexialtertabledialog.moc"

