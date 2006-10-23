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

#include "kexitabledesignerview.h"
#include "kexitabledesignerview_p.h"
#include "kexilookupcolumnpage.h"
#include "kexitabledesignercommands.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kiconeffect.h>

#include <koproperty/set.h>
#include <koproperty/utils.h>

#include <kexidb/cursor.h>
#include <kexidb/tableschema.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/error.h>
#include <kexidb/lookupfieldschema.h>
#include <kexiutils/identifier.h>
#include <kexiproject.h>
#include <keximainwindow.h>
#include <widget/tableview/kexidataawarepropertyset.h>
#include <widget/kexicustompropertyfactory.h>
#include <kexiutils/utils.h>
#include <kexidialogbase.h>
#include <kexitableview.h>

//#define MAX_FIELDS 101 //nice prime number

//! used only for BLOBs
#define DEFAULT_OBJECT_TYPE_VALUE "image"

//#define KexiTableDesignerView_DEBUG

//! @todo remove this when BLOBs are implemented
//#define KEXI_NO_BLOB_FIELDS

using namespace KexiTableDesignerCommands;

//! @internal Used in tryCastQVariant() anf canCastQVariant()
static bool isIntegerQVariant(QVariant::Type t)
{
	return t==QVariant::LongLong 
		|| t==QVariant::ULongLong
		|| t==QVariant::Int
		|| t==QVariant::UInt;
}

//! @internal Used in tryCastQVariant()
static bool canCastQVariant(QVariant::Type fromType, QVariant::Type toType)
{
	return (fromType==QVariant::Int && toType==QVariant::UInt)
   || (fromType==QVariant::CString && toType==QVariant::String)
   || (fromType==QVariant::LongLong && toType==QVariant::ULongLong)
   || ((fromType==QVariant::String || fromType==QVariant::CString) 
	      && (isIntegerQVariant(toType) || toType==QVariant::Double));
}

/*! @internal 
 \return a variant value converted from \a fromVal to \a toType type.
 Null QVariant is returned if \a fromVal's type and \a toType type 
 are incompatible. */
static QVariant tryCastQVariant( const QVariant& fromVal, QVariant::Type toType )
{
	const QVariant::Type fromType = fromVal.type();
	if (fromType == toType)
		return fromVal;
	if (canCastQVariant(fromType, toType) || canCastQVariant(toType, fromType)
	    || (isIntegerQVariant(fromType) && toType==QVariant::Double))
	{
		QVariant res( fromVal );
		if (res.cast(toType))
			return res;
	}
	return QVariant();
}


KexiTableDesignerView::KexiTableDesignerView(KexiMainWindow *win, QWidget *parent)
 : KexiDataTable(win, parent, "KexiTableDesignerView", false/*not db-aware*/)
 , KexiTableDesignerInterface()
 , d( new KexiTableDesignerViewPrivate(this) )
{
	//needed for custom "identifier" property editor widget
	KexiCustomPropertyFactory::init();

	KexiDB::Connection *conn = mainWin()->project()->dbConnection();
	d->view = dynamic_cast<KexiTableView*>(mainWidget());

	d->data = new KexiTableViewData();
	if (conn->isReadOnly())
		d->data->setReadOnly(true);
	d->data->setInsertingEnabled( false );

	KexiTableViewColumn *col = new KexiTableViewColumn("pk", KexiDB::Field::Text, QString::null,
		i18n("Additional information about the field"));
	col->setIcon( KexiUtils::colorizeIconToTextColor( SmallIcon("info"), d->view->palette() ) );
	col->setHeaderTextVisible(false);
	col->field()->setSubType("KIcon");
	col->setReadOnly(true);
	d->data->addColumn( col );

//	col = new KexiTableViewColumn("name", KexiDB::Field::Text, i18n("Field Name"),
	col = new KexiTableViewColumn("caption", KexiDB::Field::Text, i18n("Field Caption"),
		i18n("Describes caption for the field"));
//	KexiUtils::Validator *vd = new KexiUtils::IdentifierValidator();
//	vd->setAcceptsEmptyValue(true);
//	col->setValidator( vd );
	d->data->addColumn( col );

	col = new KexiTableViewColumn("type", KexiDB::Field::Enum, i18n("Data Type"),
		i18n("Describes data type for the field"));
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
		d->maxTypeNameTextWidth = QMAX(d->maxTypeNameTextWidth, fm.width(types[i-1]));
	}
	col->field()->setEnumHints(types);

	d->data->addColumn( col = new KexiTableViewColumn("comments", KexiDB::Field::Text, i18n("Comments"),
		i18n("Describes additional comments for the field")) );

	d->view->setSpreadSheetMode();

	connect(d->data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant&,KexiDB::ResultInfo*)));
	connect(d->data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	//connect(d->data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
	//	this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));
	connect(d->data, SIGNAL(aboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)),
		this, SLOT(slotAboutToDeleteRow(KexiTableItem&,KexiDB::ResultInfo*,bool)));

	setMinimumSize(d->view->minimumSizeHint().width(), d->view->minimumSizeHint().height());
	d->view->setFocus();

	d->sets = new KexiDataAwarePropertySet( this, d->view );
	connect(d->sets, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
	connect(d->sets, SIGNAL(rowInserted()), this, SLOT(slotRowInserted()));

	d->contextMenuTitle = new KPopupTitle(d->view->contextMenu());
	d->view->contextMenu()->insertItem(d->contextMenuTitle, -1, 0);
	connect(d->view->contextMenu(), SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowContextMenu()));

	plugSharedAction("tablepart_toggle_pkey", this, SLOT(slotTogglePrimaryKey()));
	d->action_toggle_pkey = static_cast<KToggleAction*>( sharedAction("tablepart_toggle_pkey") );
	d->action_toggle_pkey->plug(d->view->contextMenu(), 1); //add at the beginning
	setAvailable("tablepart_toggle_pkey", !conn->isReadOnly());

#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
	plugSharedAction("edit_undo", this, SLOT(slotUndo()));
	plugSharedAction("edit_redo", this, SLOT(slotRedo()));
	setAvailable("edit_undo", false);
	setAvailable("edit_redo", false);
	connect(d->history, SIGNAL(commandExecuted(KCommand*)), this, SLOT(slotCommandExecuted(KCommand*)));
#endif

#ifdef KEXI_DEBUG_GUI
	KexiUtils::addAlterTableActionDebug(QString::null); //to create the tab
	KexiUtils::connectPushButtonActionForDebugWindow( 
		"simulateAlterTableExecution", this, SLOT(slotSimulateAlterTableExecution()));
	KexiUtils::connectPushButtonActionForDebugWindow( 
		"executeRealAlterTable", this, SLOT(executeRealAlterTable()));
#endif
}

KexiTableDesignerView::~KexiTableDesignerView()
{
//	removeCurrentPropertySet();
	delete d;
}

void KexiTableDesignerView::initData()
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
			if (field->isPrimaryKey()) {
				(*item)[COLUMN_ID_ICON] = "key";
				d->primaryKeyExists = true;
			}
			else {
				KexiDB::LookupFieldSchema *lookupFieldSchema
					= field->table() ? field->table()->lookupFieldSchema(*field) : 0;
				if (lookupFieldSchema && lookupFieldSchema->rowSource().type()!=KexiDB::LookupFieldSchema::RowSource::NoType
					&& !lookupFieldSchema->rowSource().name().isEmpty())
				{
					(*item)[COLUMN_ID_ICON] = "combo";
				}
			}
			(*item)[COLUMN_ID_CAPTION] = field->captionOrName();
			(*item)[COLUMN_ID_TYPE] = field->typeGroup()-1; //-1 because type groups are counted from 1
			(*item)[COLUMN_ID_DESC] = field->description();
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
			createPropertySet( i, *field );
		}
	}

	//column widths
	d->view->setColumnWidth(COLUMN_ID_ICON, IconSize( KIcon::Small ) + 10);
	d->view->adjustColumnWidthToContents(COLUMN_ID_CAPTION); //adjust column width
	d->view->setColumnWidth(COLUMN_ID_TYPE, d->maxTypeNameTextWidth + 2 * d->view->rowHeight());
	d->view->setColumnStretchEnabled( true, COLUMN_ID_DESC ); //last column occupies the rest of the area
	const int minCaptionColumnWidth = d->view->fontMetrics().width("wwwwwwwwwww");
	if (minCaptionColumnWidth > d->view->columnWidth(COLUMN_ID_CAPTION))
		d->view->setColumnWidth(COLUMN_ID_CAPTION, minCaptionColumnWidth);

	setDirty(false);
	d->view->setCursorPosition(0, COLUMN_ID_CAPTION); //set @ name column
	propertySetSwitched();
}

//! Gets subtype strings and names for type \a fieldType
void
KexiTableDesignerView::getSubTypeListData(KexiDB::Field::TypeGroup fieldTypeGroup, 
	QStringList& stringsList, QStringList& namesList)
{
/* disabled - "mime" is moved from subType to "objectType" custom property
	if (fieldTypeGroup==KexiDB::Field::BLOBGroup) {
		// special case: BLOB type uses "mime-based" subtypes
//! @todo hardcoded!
		stringsList << "image";
		namesList << i18n("Image object type", "Image");
	}
	else {*/
	stringsList = KexiDB::typeStringsForGroup(fieldTypeGroup);
	namesList = KexiDB::typeNamesForGroup(fieldTypeGroup);
//	}
	kexipluginsdbg << "KexiTableDesignerView::getSubTypeListData(): subType strings: " << 
		stringsList.join("|") << "\nnames: " << namesList.join("|") << endl;
}

KoProperty::Set *
KexiTableDesignerView::createPropertySet( int row, const KexiDB::Field& field, bool newOne )
{
	QString typeName = "KexiDB::Field::" + field.typeGroupString();
	KoProperty::Set *set = new KoProperty::Set(d->sets, typeName);
	if (mainWin()->project()->dbConnection()->isReadOnly())
		set->setReadOnly( true );
//	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
//		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));

	KoProperty::Property *prop;

	set->addProperty(prop = new KoProperty::Property("uid", d->generateUniqueId(), ""));
	prop->setVisible(false);

	//meta-info for property editor
	set->addProperty(prop = new KoProperty::Property("this:classString", i18n("Table field")) );
	prop->setVisible(false);
	set->addProperty(prop = new KoProperty::Property("this:iconName", 
//! \todo add table_field icon 
		"lineedit" //"table_field"
	));
	prop->setVisible(false);
	set->addProperty(prop = new KoProperty::Property("this:useCaptionAsObjectName", 
		QVariant(true, 1), QString::null)); //we want "caption" to be displayed in the header, not name
	prop->setVisible(false);

	//name
	set->addProperty( prop 
		= new KoProperty::Property("name", QVariant(field.name()), i18n("Name"), 
		QString::null, KexiCustomPropertyFactory::Identifier) );

	//type
	set->addProperty( prop 
		= new KoProperty::Property("type", QVariant(field.type()), i18n("Type")) );
#ifndef KexiTableDesignerView_DEBUG
	prop->setVisible(false);//always hidden
#endif

	//subtype
	QStringList typeStringList, typeNameList;
	getSubTypeListData(field.typeGroup(), typeStringList, typeNameList);
/* disabled - "mime" is moved from subType to "objectType" custom property
	QString subTypeValue;
	if (field.typeGroup()==KexiDB::Field::BLOBGroup) {
// special case: BLOB type uses "mime-based" subtypes
//! @todo this should be retrieved from KexiDB::Field when BLOB supports many different mimetypes
		subTypeValue = slist.first();
	}
	else {*/
	QString subTypeValue = field.typeString();
	//}
	set->addProperty(prop = new KoProperty::Property("subType", 
		typeStringList, typeNameList, subTypeValue, i18n("Subtype")));

	// objectType
	QStringList objectTypeStringList, objectTypeNameList;
//! @todo this should be retrieved from KexiDB::Field when BLOB supports many different mimetypes
	objectTypeStringList << "image";
	objectTypeNameList << i18n("Image object type", "Image");
	QString objectTypeValue( field.customProperty("objectType").toString() );
	if (objectTypeValue.isEmpty())
		objectTypeValue = DEFAULT_OBJECT_TYPE_VALUE;
	set->addProperty(prop = new KoProperty::Property("objectType", 
		objectTypeStringList, objectTypeNameList, objectTypeValue, i18n("Subtype")/*todo other i18n string?*/));
	
	set->addProperty( prop 
		= new KoProperty::Property("caption", QVariant(field.caption()), i18n("Caption") ) );
	prop->setVisible(false);//always hidden

	set->addProperty( prop 
		= new KoProperty::Property("description", QVariant(field.description())) );
	prop->setVisible(false);//always hidden

	set->addProperty( prop 
		= new KoProperty::Property("unsigned", QVariant(field.isUnsigned(), 4), i18n("Unsigned Number")));

	set->addProperty( prop 
		= new KoProperty::Property("length", (int)field.length()/*200?*/, i18n("Length")));

	set->addProperty( prop 
		= new KoProperty::Property("precision", (int)field.precision()/*200?*/, i18n("Precision")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif
	set->addProperty( prop 
		= new KoProperty::Property("visibleDecimalPlaces", field.visibleDecimalPlaces(), i18n("Visible Decimal Places")));
	prop->setOption("min", -1);
	prop->setOption("minValueText", i18n("Auto Decimal Places","Auto"));

//! @todo set reasonable default for column width
	set->addProperty( prop 
		= new KoProperty::Property("width", (int)field.width()/*200?*/, i18n("Column Width")));
#ifdef KEXI_NO_UNFINISHED
	prop->setVisible(false);
#endif

	set->addProperty( prop 
		= new KoProperty::Property("defaultValue", field.defaultValue(), i18n("Default Value"),
			QString::null, (KoProperty::PropertyType)field.variantType()) );
	prop->setOption("3rdState", i18n("None"));
//	prop->setVisible(false);

	set->addProperty( prop 
		= new KoProperty::Property("primaryKey", QVariant(field.isPrimaryKey(), 4), i18n("Primary Key")));
	prop->setIcon("key");

	set->addProperty( prop
		= new KoProperty::Property("unique", QVariant(field.isUniqueKey(), 4), i18n("Unique")));

	set->addProperty( prop
		= new KoProperty::Property("notNull", QVariant(field.isNotNull(), 4), i18n("Required")));
	
	set->addProperty( prop
		= new KoProperty::Property("allowEmpty", QVariant(!field.isNotEmpty(), 4), i18n("Allow Zero\nSize")));

	set->addProperty( prop
		= new KoProperty::Property("autoIncrement", QVariant(field.isAutoIncrement(), 4), i18n("Autonumber")));
	prop->setIcon("autonumber");

	set->addProperty( prop
		= new KoProperty::Property("indexed", QVariant(field.isIndexed(), 4), i18n("Indexed")));

	//- properties related to lookup columns (used and set by the "lookup column" tab in the property pane)
	KexiDB::LookupFieldSchema *lookupFieldSchema = field.table() ? field.table()->lookupFieldSchema(field) : 0;
	set->addProperty( prop = new KoProperty::Property("rowSource", 
		lookupFieldSchema ? lookupFieldSchema->rowSource().name() : QString::null, i18n("Row Source")));
	prop->setVisible(false);

	set->addProperty( prop = new KoProperty::Property("rowSourceType", 
		lookupFieldSchema ? lookupFieldSchema->rowSource().typeName() : QString::null, i18n("Row Source\nType")));
	prop->setVisible(false);

	set->addProperty( prop
		= new KoProperty::Property("boundColumn", 
		lookupFieldSchema ? lookupFieldSchema->boundColumn() : -1, i18n("Bound Column")));
	prop->setVisible(false);
	
	set->addProperty( prop
		= new KoProperty::Property("visibleColumn", 
		lookupFieldSchema ? lookupFieldSchema->visibleColumn() : -1, i18n("Visible Column")));
	prop->setVisible(false);

//! @todo support columnWidths(), columnHeadersVisible(), maximumListRows(), limitToList(), displayWidget()

	//----
	d->updatePropertiesVisibility(field.type(), *set);

	connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
		this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));

	d->sets->insert(row, set, newOne);
	return set;
}

void KexiTableDesignerView::updateActions(bool activated)
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

void KexiTableDesignerView::slotUpdateRowActions(int row)
{
	KexiDataTable::slotUpdateRowActions(row);
	updateActions();
}

void KexiTableDesignerView::slotTogglePrimaryKey()
{
	if (d->slotTogglePrimaryKeyCalled)
		return;
	d->slotTogglePrimaryKeyCalled = true;
	if (!propertySet())
		return;
	KoProperty::Set &set = *propertySet();
	bool isSet = !set["primaryKey"].value().toBool();
	set.changeProperty("primaryKey", QVariant(isSet,1)); //this will update all related properties as well
/*	CommandGroup *setPrimaryKeyCommand;
	if (isSet) {
		setPrimaryKeyCommand = new CommandGroup(i18n("Set primary key for field \"%1\"")
			.arg(set["name"].value().toString()) );
	}
	else {
		setPrimaryKeyCommand = new CommandGroup(i18n("Unset primary key for field \"%1\"")
			.arg(set["name"].value().toString()) );
	}
	switchPrimaryKey(set, isSet, false, setPrimaryKeyCommand);*/
	//addHistoryCommand( setPrimaryKeyCommand, false /* !execute */ );
	d->slotTogglePrimaryKeyCalled = false;
}

void KexiTableDesignerView::switchPrimaryKey(KoProperty::Set &propertySet, 
	bool set, bool aWasPKey, CommandGroup* commandGroup)
{
	const bool was_pkey = aWasPKey || propertySet["primaryKey"].value().toBool();
//	propertySet["primaryKey"] = QVariant(set, 1);
	d->setPropertyValueIfNeeded( propertySet, "primaryKey", QVariant(set,1), commandGroup );
	if (&propertySet==this->propertySet()) {
		//update action and icon @ column 0 (only if we're changing current property set)
		d->action_toggle_pkey->setChecked(set);
		if (d->view->selectedItem()) {
			//show key in the table
			d->view->data()->clearRowEditBuffer();
			d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_ICON, 
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
			//(*s)["autoIncrement"] = QVariant(false, 0);
			d->setPropertyValueIfNeeded( *s, "autoIncrement", QVariant(false,0), commandGroup );
			//(*s)["primaryKey"] = QVariant(false, 0);
			d->setPropertyValueIfNeeded( *s, "primaryKey", QVariant(false,0), commandGroup );
			//remove key from table
			d->view->data()->clearRowEditBuffer();
			KexiTableItem *item = d->view->itemAt(i);
			if (item) {
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_ICON, QVariant());
				d->view->data()->saveRowChanges(*item, true);
			}
		}
		//set unsigned big-integer type
//		d->view->data()->saveRowChanges(*d->view->selectedItem());
		d->slotBeforeCellChanged_enabled = false;
			d->view->data()->clearRowEditBuffer();
			d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE,
			QVariant(KexiDB::Field::IntegerGroup-1/*counting from 0*/));
//			QVariant(KexiDB::Field::typeGroupName(KexiDB::Field::IntegerGroup)));
			d->view->data()->saveRowChanges(*d->view->selectedItem(), true);
		//propertySet["subType"] = KexiDB::Field::typeString(KexiDB::Field::BigInteger);
			d->setPropertyValueIfNeeded( propertySet, "subType", KexiDB::Field::typeString(KexiDB::Field::BigInteger), 
				commandGroup );
		//propertySet["unsigned"] = QVariant(true,4);
			d->setPropertyValueIfNeeded( propertySet, "unsigned", QVariant(true,4), commandGroup );
/*todo*/
		d->slotBeforeCellChanged_enabled = true;
	}
	updateActions();
}

/*void KexiTableDesignerView::slotCellSelected(int, int row)
{
	kDebug() << "KexiTableDesignerView::slotCellSelected()" << endl;
	if(row == m_row)
		return;
	m_row = row;
	propertyBufferSwitched();
}*/

tristate KexiTableDesignerView::beforeSwitchTo(int mode, bool &dontStore)
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
			int r = KMessageBox::warningYesNoCancel(this,
				i18n("Saving changes for existing table design is now required.")
				+ "\n" + d->messageForSavingChanges(emptyTable), QString::null,
				KStdGuiItem::save(), KStdGuiItem::discard(), QString::null, 
				KMessageBox::Notify|KMessageBox::Dangerous);
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

tristate KexiTableDesignerView::afterSwitchFrom(int mode)
{
	if (mode==Kexi::NoViewMode || mode==Kexi::DataViewMode) {
		initData();
	}
	return true;
}

KoProperty::Set *KexiTableDesignerView::propertySet()
{
	return d->sets ? d->sets->currentPropertySet() : 0;
}

/*
void KexiTableDesignerView::removeCurrentPropertySet()
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

void KexiTableDesignerView::slotBeforeCellChanged(
	KexiTableItem *item, int colnum, QVariant& newValue, KexiDB::ResultInfo* /*result*/)
{
	if (!d->slotBeforeCellChanged_enabled)
		return;
//	kDebug() << d->view->selectedItem() << " " << item 
		//<< " " << d->sets->at( d->view->currentRow() ) << " " << propertySet() << endl;
	if (colnum==COLUMN_ID_CAPTION) {//'caption'
//		if (!item->at(1).toString().isEmpty() && item->at(1).isNull()) {
		//if 'type' is not filled yet
		if (item->at(COLUMN_ID_TYPE).isNull()) {
			//auto select 1st row of 'type' column
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, QVariant((int)0));
		}

		KoProperty::Set *propertySetForItem = d->sets->findPropertySetForItem(*item);
		if (propertySetForItem) {
			d->addHistoryCommand_in_slotPropertyChanged_enabled = false; //because we'll add the two changes as one KMacroCommand
				QString oldName( propertySetForItem->property("name").value().toString() );
				QString oldCaption( propertySetForItem->property("caption").value().toString() );

				//we need to create the action now as set["name"] will be changed soon..
				ChangeFieldPropertyCommand *changeCaptionCommand
					= new ChangeFieldPropertyCommand( this, *propertySetForItem, "caption", oldCaption, newValue);

				//update field caption and name
				propertySetForItem->changeProperty("caption", newValue);
				propertySetForItem->changeProperty("name", newValue); // "name" prop. is of custom type Identifier, so this assignment 
										            // will automatically convert newValue to an valid identifier

				//remember this action containing 2 subactions
				CommandGroup *changeCaptionAndNameCommand = new CommandGroup(
					i18n("Change \"%1\" field's name to \"%2\" and caption from \"%3\" to \"%4\"")
						.arg(oldName).arg(propertySetForItem->property("name").value().toString())
						.arg(oldCaption).arg(newValue.toString() ));
				changeCaptionAndNameCommand->addCommand( changeCaptionCommand );
//					new ChangeFieldPropertyCommand( this, *propertySetForItem,
	//						"caption", oldCaption, newValue)
		//		);
				changeCaptionAndNameCommand->addCommand(
					new ChangeFieldPropertyCommand( this, *propertySetForItem,
							"name", oldName, propertySetForItem->property("name").value().toString())
				);
				addHistoryCommand( changeCaptionAndNameCommand, false /* !execute */ );

			d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
		}
	}
	else if (colnum==COLUMN_ID_TYPE) {//'type'
		if (newValue.isNull()) {
			//'type' col will be cleared: clear all other columns as well
			d->slotBeforeCellChanged_enabled = false;
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_ICON, QVariant());
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_CAPTION, QVariant(QString::null));
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_DESC, QVariant());
			d->slotBeforeCellChanged_enabled = true;
			return;
		}

		KoProperty::Set *propertySetForItem = d->sets->findPropertySetForItem(*item);
		if (!propertySetForItem)
			return;

		KoProperty::Set &set = *propertySetForItem; //propertySet();

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
//moved down		set["type"] = (int)fieldType;
//		set["subType"] = KexiDB::Field::typeName(fieldType);
		
		//-get subtypes for this type: keys (slist) and names (nlist)
		QStringList slist, nlist;
		getSubTypeListData(fieldTypeGroup, slist, nlist);

		QString subTypeValue;
/* disabled - "mime" is moved from subType to "objectType" custom property
		if (fieldType==KexiDB::Field::BLOB) {
			// special case: BLOB type uses "mime-based" subtypes
			subTypeValue = slist.first();
		}
		else {*/
			subTypeValue = KexiDB::Field::typeString(fieldType);
		//}
		KoProperty::Property *subTypeProperty = &set["subType"];
		kexipluginsdbg << subTypeProperty->value() << endl;
		
		// *** this action contains subactions ***
		CommandGroup *changeDataTypeCommand = new CommandGroup(
			i18n("Change data type for field \"%1\" to \"%2\"")
				.arg(set["name"].value().toString()).arg( KexiDB::Field::typeName( fieldType ) ) );

//kexipluginsdbg << "++++++++++" << slist << nlist << endl;

		//update subtype list and value
		const bool forcePropertySetReload 
			= KexiDB::Field::typeGroup( KexiDB::Field::typeForString(subTypeProperty->value().toString()) )
				!= fieldTypeGroup; //<-- ?????
//		const bool forcePropertySetReload = set["type"].value().toInt() != (int)fieldTypeGroup;
		const bool useListData = slist.count() > 1; //disabled-> || fieldType==KexiDB::Field::BLOB;

		if (!useListData) {
			slist.clear(); //empty list will be passed
			nlist.clear();
		}
		d->setPropertyValueIfNeeded( set, "type", (int)fieldType, changeDataTypeCommand,
			false /*!forceAddCommand*/, true /*rememberOldValue*/);

		// notNull and defaultValue=false is reasonable for boolean type
		if (fieldType == KexiDB::Field::Boolean) {
//! @todo maybe this is good for other data types as well?
			d->setPropertyValueIfNeeded( set, "notNull", QVariant(true, 1), changeDataTypeCommand,
				false /*!forceAddCommand*/, false /*!rememberOldValue*/);
			d->setPropertyValueIfNeeded( set, "defaultValue", QVariant(false, 1), changeDataTypeCommand,
				false /*!forceAddCommand*/, false /*!rememberOldValue*/);
		}

/*		if (useListData) {
		{
			subTypeProperty->setListData( slist, nlist );
		}
		else {
			subTypeProperty->setListData( 0 );
		}*/
		if (set["primaryKey"].value().toBool()==true) {
			//primary keys require big int, so if selected type is not integer- remove PK
			if (fieldTypeGroup != KexiDB::Field::IntegerGroup) {
				/*not needed, line below will do the work
				d->view->data()->updateRowEditBuffer(item, COLUMN_ID_ICON, QVariant());
				d->view->data()->saveRowChanges(*item); */
				//set["primaryKey"] = QVariant(false, 1);
				d->setPropertyValueIfNeeded( set, "primaryKey", QVariant(false, 1), changeDataTypeCommand );
//! @todo should we display (passive?) dialog informing about cleared pkey?
			}
		}
//		if (useListData)
//		subTypeProperty->setValue( subTypeValue, false/*!rememberOldValue*/ );
		d->setPropertyValueIfNeeded( set, "subType", subTypeValue, 
			changeDataTypeCommand, false, false /*!rememberOldValue*/,
			&slist, &nlist );

		if (d->updatePropertiesVisibility(fieldType, set, changeDataTypeCommand) || forcePropertySetReload) {
			//properties' visiblility changed: refresh prop. set
			propertySetReloaded(true);
		}

		addHistoryCommand( changeDataTypeCommand, false /* !execute */ );
	}
	else if (colnum==COLUMN_ID_DESC) {//'description'
		KoProperty::Set *propertySetForItem = d->sets->findPropertySetForItem(*item);
		if (!propertySetForItem)
			return;
		//update field desc.
		QVariant oldValue((*propertySetForItem)["description"].value());
		kexipluginsdbg << oldValue << endl;
		propertySetForItem->changeProperty("description", newValue);
		/*moved addHistoryCommand( 
			new ChangeFieldPropertyCommand( this, *propertySetForItem,
				"description", oldValue, newValue ), false);*/
	}
}

void KexiTableDesignerView::slotRowUpdated(KexiTableItem *item)
{
	const int row = d->view->data()->findRef(item);
	if (row < 0)
		return;
	
	setDirty();

	//-check if the row was empty before updating
	//if yes: we want to add a property set for this new row (field)
	QString fieldCaption( item->at(COLUMN_ID_CAPTION).toString() );
	const bool prop_set_allowed = !item->at(COLUMN_ID_TYPE).isNull();

	if (!prop_set_allowed && d->sets->at(row)/*propertySet()*/) {
		//there is a property set, but it's not allowed - remove it:
		d->sets->remove( row ); //d->sets->removeCurrentPropertySet();

		//clear 'type' column:
		d->view->data()->clearRowEditBuffer();
//		d->view->data()->updateRowEditBuffer(d->view->selectedItem(), COLUMN_ID_TYPE, QVariant());
		d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, QVariant());
		d->view->data()->saveRowChanges(*item);

	} else if (prop_set_allowed && !d->sets->at(row)/*propertySet()*/) {
		//-- create a new field:
		KexiDB::Field::TypeGroup fieldTypeGroup = static_cast<KexiDB::Field::TypeGroup>( 
			item->at(COLUMN_ID_TYPE).toInt()+1/*counting from 1*/ );
		int intFieldType = KexiDB::defaultTypeForGroup( fieldTypeGroup );
		if (intFieldType==0)
			return;

		QString description( item->at(COLUMN_ID_DESC).toString() );

//todo: check uniqueness:
		QString fieldName( KexiUtils::string2Identifier(fieldCaption) );

		KexiDB::Field::Type fieldType = KexiDB::intToFieldType( intFieldType );
		KexiDB::Field field( //tmp
			fieldName,
			fieldType,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			/*length*/0,
			/*precision*/0,
			/*defaultValue*/QVariant(),
			fieldCaption,
			description,
			/*width*/0);
//		m_newTable->addField( field );

		// reasonable case for boolean type: set notNull flag and "false" as default value
		if (fieldType == KexiDB::Field::Boolean) {
			field.setNotNull( true );
			field.setDefaultValue( QVariant(false, 0) );
		}

		kexipluginsdbg << "KexiTableDesignerView::slotRowUpdated(): " << field.debugString() << endl;

		//create a new property set:
		KoProperty::Set *newSet = createPropertySet( row, field, true );
//moved
		//add a special property indicating that this is brand new buffer,
		//not just changed
//		KoProperty::Property* prop = new KoProperty::Property("newrow", QVariant());
//		prop->setVisible(false);
//		newbuff->addProperty( prop );

		//refresh property editor:
		propertySetSwitched();

		if (row>=0) {
			if (d->addHistoryCommand_in_slotRowUpdated_enabled) {
				addHistoryCommand( new InsertFieldCommand( this, row, *newSet /*propertySet()*/ ), //, field /*will be copied*/ 
					false /* !execute */ );
			}
		}
		else {
			kexipluginswarn << "KexiTableDesignerView::slotRowUpdated() row # not found  !" << endl;
		}
	}
}

void KexiTableDesignerView::updateActions()
{
	updateActions(false);
}

void KexiTableDesignerView::slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property)
{
//	if (!d->slotPropertyChanged_enabled)
//		return;
	const QCString pname = property.name();
	kexipluginsdbg << "KexiTableDesignerView::slotPropertyChanged(): " << pname << " = " << property.value() 
		<< " (oldvalue = " << property.oldValue() << ")" << endl;

	// true is PK should be altered
	bool changePrimaryKey = false;
	// true is PK should be set to true, otherwise unset
	bool setPrimaryKey = false;

	if (pname=="primaryKey" && d->slotPropertyChanged_primaryKey_enabled) {
		changePrimaryKey = true;
		setPrimaryKey = property.value().toBool();
	}

	// update "lookup column" icon
	if (pname=="rowSource" || pname=="rowSourceType") {
//! @todo indicate invalid definitions of lookup columns as well using a special icon 
//!       (e.g. due to missing data source)
		const int row = d->sets->findRowForPropertyValue("uid", set["uid"].value().toInt());
		KexiTableItem *item = d->view->itemAt(row);
		if (item)
			d->updateIconForItem(*item, set);
	}

	//setting autonumber requires setting PK as well
	CommandGroup *setAutonumberCommand = 0;
	CommandGroup *toplevelCommand = 0;
	if (pname=="autoIncrement" && property.value().toBool()==true) {
		if (set["primaryKey"].value().toBool()==false) {//we need PKEY here!
			QString msg = QString("<p>")
				+i18n("Setting autonumber requires primary key to be set for current field.")+"</p>";
			if (d->primaryKeyExists)
				msg += (QString("<p>")+ i18n("Previous primary key will be removed.")+"</p>");
			msg += (QString("<p>")
				+i18n("Do you want to create primary key for current field? "
				"Click \"Cancel\" to cancel setting autonumber.")+"</p>");

			if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
				i18n("Setting Autonumber Field"),
				KGuiItem(i18n("Create &Primary Key"), "key"), KStdGuiItem::cancel() ))
			{
				changePrimaryKey = true;
				setPrimaryKey = true;
				//switchPrimaryKey(set, true);
				// this will be toplevel command 
				setAutonumberCommand = new CommandGroup(
					i18n("Assign autonumber for field \"%1\"").arg(set["name"].value().toString()) );
				toplevelCommand = setAutonumberCommand;
				d->setPropertyValueIfNeeded( set, "autoIncrement", QVariant(true,1), setAutonumberCommand );
			}
			else {
				setAutonumberCommand = new CommandGroup(
					i18n("Remove autonumber from field \"%1\"").arg(set["name"].value().toString()) );
				//d->slotPropertyChanged_enabled = false;
//					set["autoIncrement"].setValue( QVariant(false,1), false/*don't save old*/);
//					d->slotPropertyChanged_enabled = true;
				d->setPropertyValueIfNeeded( set, "autoIncrement", QVariant(false,1), setAutonumberCommand, 
					true /*forceAddCommand*/, false/*rememberOldValue*/ );
				addHistoryCommand( setAutonumberCommand, false /* !execute */ );
				return;
			}
		}
	}

	//clear PK when these properies were set to false:
	if ((pname=="indexed" || pname=="unique" || pname=="notNull") 
		&& set["primaryKey"].value().toBool() && property.value().toBool()==false)
	{
//! @todo perhaps show a hint in help panel telling what happens?
		changePrimaryKey = true;
		setPrimaryKey = false;
		// this will be toplevel command 
		CommandGroup *unsetIndexedOrUniquOrNotNullCommand = new CommandGroup(
			i18n("Set \"%1\" property for field \"%2\"").arg(property.caption()).arg(set["name"].value().toString()) );
		toplevelCommand = unsetIndexedOrUniquOrNotNullCommand;
		d->setPropertyValueIfNeeded( set, pname, QVariant(false,1), unsetIndexedOrUniquOrNotNullCommand );
		if (pname=="notNull") {
//?			d->setPropertyValueIfNeeded( set, "notNull", QVariant(true,1), unsetIndexedOrUniquOrNotNullCommand );
			d->setPropertyValueIfNeeded( set, "unique", QVariant(false,1), unsetIndexedOrUniquOrNotNullCommand );
		}
	}

	if (pname=="defaultValue") {
		KexiDB::Field::Type type = KexiDB::intToFieldType( set["type"].value().toInt() );
		set["defaultValue"].setType((KoProperty::PropertyType)KexiDB::Field::variantType(type));
	}

	if (pname=="subType" && d->slotPropertyChanged_subType_enabled) {
		d->slotPropertyChanged_subType_enabled = false;
		if (set["primaryKey"].value().toBool()==true 
			&& property.value().toString()!=KexiDB::Field::typeString(KexiDB::Field::BigInteger))
		{
			kexipluginsdbg << "INVALID " << property.value().toString() << endl;
//			if (KMessageBox::Yes == KMessageBox::questionYesNo(this, msg,
//				i18n("This field has promary key assigned. Setting autonumber field"),
//				KGuiItem(i18n("Create &Primary Key"), "key"), KStdGuiItem::cancel() ))

		}
		KexiDB::Field::Type type = KexiDB::intToFieldType( set["type"].value().toInt() );
		QString typeName;
/* disabled - "mime" is moved from subType to "objectType" custom property
		if (type==KexiDB::Field::BLOB) { //special case
			//find i18n'd text
			QStringList stringsList, namesList;
			getSubTypeListData(KexiDB::Field::BLOBGroup, stringsList, namesList);
			const int stringIndex = stringsList.findIndex( property.value().toString() );
			if (-1 == stringIndex || stringIndex>=(int)namesList.count())
				typeName = property.value().toString(); //for sanity
			else
				typeName = namesList[stringIndex];
		}
		else {*/
		typeName = KexiDB::Field::typeName( KexiDB::Field::typeForString(property.value().toString()) );
//		}
//		kDebug() << property.value().toString() << endl;
//		kDebug() << set["type"].value() << endl;
//		if (KexiDB::Field::typeGroup( set["type"].value().toInt() ) == (int)KexiDB::Field::TextGroup) {
		CommandGroup* changeFieldTypeCommand = new CommandGroup(
			i18n("Change type for field \"%1\" to \"%2\"").arg(set["name"].value().toString())
			.arg(typeName) );
		d->setPropertyValueIfNeeded( set, "subType", property.value(), property.oldValue(),
			changeFieldTypeCommand );

		kexipluginsdbg << set["type"].value() << endl;
		const KexiDB::Field::Type newType = KexiDB::Field::typeForString(property.value().toString());
		set["type"].setValue( newType );

		// cast "defaultValue" property value to a new type
		QVariant oldDefVal( set["defaultValue"].value() );
		QVariant newDefVal( tryCastQVariant(oldDefVal, KexiDB::Field::variantType(type)) );
		if (oldDefVal.type()!=newDefVal.type())
			set["defaultValue"].setType( newDefVal.type() );
		d->setPropertyValueIfNeeded( set, "defaultValue", newDefVal, newDefVal,
			changeFieldTypeCommand );

		d->updatePropertiesVisibility(newType, set);
		//properties' visiblility changed: refresh prop. set
		propertySetReloaded(true);
		d->slotPropertyChanged_subType_enabled = true;

		addHistoryCommand( changeFieldTypeCommand, false /* !execute */ );
		return;
//		}
//		d->slotPropertyChanged_subType_enabled = true;
//		return;
	}

	if (d->addHistoryCommand_in_slotPropertyChanged_enabled && !changePrimaryKey/*we'll add multiple commands for PK*/) {
		addHistoryCommand( new ChangeFieldPropertyCommand(this, set, 
				property.name(), property.oldValue() /* ??? */, property.value()), 
			false /* !execute */ );
	}

	if (changePrimaryKey) {
	  d->slotPropertyChanged_primaryKey_enabled = false;
		if (setPrimaryKey) {
			//primary key implies some rules
			//const bool prev_addHistoryCommand_in_slotPropertyChanged_enabled = d->addHistoryCommand_in_slotPropertyChanged_enabled;
//			d->addHistoryCommand_in_slotPropertyChanged_enabled = false;

			//this action contains subactions
			CommandGroup *setPrimaryKeyCommand = new CommandGroup(
				i18n("Set primary key for field \"%1\"")
					.arg(set["name"].value().toString()) );
			if (toplevelCommand)
				toplevelCommand->addCommand( setPrimaryKeyCommand );
			else
				toplevelCommand = setPrimaryKeyCommand;

			d->setPropertyValueIfNeeded( set, "primaryKey", QVariant(true,1), setPrimaryKeyCommand, true /*forceAddCommand*/ );
			d->setPropertyValueIfNeeded( set, "unique", QVariant(true,1), setPrimaryKeyCommand );
			d->setPropertyValueIfNeeded( set, "notNull", QVariant(true,1), setPrimaryKeyCommand );
			d->setPropertyValueIfNeeded( set, "allowEmpty", QVariant(false,1), setPrimaryKeyCommand );
			d->setPropertyValueIfNeeded( set, "indexed", QVariant(true,1), setPrimaryKeyCommand );
//! \todo: add setting for this: "Integer PKeys have autonumber set by default"
			d->setPropertyValueIfNeeded( set, "autoIncrement", QVariant(true,1), setPrimaryKeyCommand );

/*				set["unique"] = QVariant(true,1);
				set["notNull"] = QVariant(true,1);
				set["allowEmpty"] = QVariant(false,1);
				set["indexed"] = QVariant(true,1);
				set["autoIncrement"] = QVariant(true,1);*/
//			d->addHistoryCommand_in_slotPropertyChanged_enabled = prev_addHistoryCommand_in_slotPropertyChanged_enabled;
//down			addHistoryCommand( toplevelCommand, false /* !execute */ );
		}
		else {//! set PK to false
			//remember this action containing 2 subactions
			CommandGroup *setPrimaryKeyCommand = new CommandGroup(
				i18n("Unset primary key for field \"%1\"")
					.arg(set["name"].value().toString()) );
			if (toplevelCommand)
				toplevelCommand->addCommand( setPrimaryKeyCommand );
			else
				toplevelCommand = setPrimaryKeyCommand;

			d->setPropertyValueIfNeeded( set, "primaryKey", QVariant(false,1), setPrimaryKeyCommand, true /*forceAddCommand*/ );
			d->setPropertyValueIfNeeded( set, "autoIncrement", QVariant(false,1), setPrimaryKeyCommand );
//			set["autoIncrement"] = QVariant(false,1);

//down			addHistoryCommand( toplevelCommand, false /* !execute */ );
		}
		switchPrimaryKey(set, setPrimaryKey, true/*wasPKey*/, toplevelCommand);
		d->updatePropertiesVisibility(
			KexiDB::Field::typeForString( set["subType"].value().toString() ), set, toplevelCommand);
		addHistoryCommand( toplevelCommand, false /* !execute */ );
		//properties' visiblility changed: refresh prop. set
		propertySetReloaded(true/*preservePrevSelection*/);
	  d->slotPropertyChanged_primaryKey_enabled = true;
	}
}

void KexiTableDesignerView::slotRowInserted()
{
	updateActions();

	if (d->addHistoryCommand_in_slotRowInserted_enabled) {
		const int row = d->view->currentRow();
		if (row>=0) {
			addHistoryCommand( new InsertEmptyRowCommand( this, row ), false /* !execute */ );
		}
	}
	//TODO?
}

void KexiTableDesignerView::slotAboutToDeleteRow(
	KexiTableItem& item, KexiDB::ResultInfo* result, bool repaint)
{
	Q_UNUSED(result)
	Q_UNUSED(repaint)
	if (item[COLUMN_ID_ICON].toString()=="key")
		d->primaryKeyExists = false;

	if (d->addHistoryCommand_in_slotAboutToDeleteRow_enabled) {
		const int row = d->view->data()->findRef(&item);
		KoProperty::Set *set = row >=0 ? d->sets->at(row) : 0;
		//set can be 0 here, what means "removing empty row"
		addHistoryCommand( 
			new RemoveFieldCommand( this, row, set ),
			false /* !execute */
		);
	}
}

KexiDB::Field * KexiTableDesignerView::buildField( const KoProperty::Set &set ) const
{
	//create a map of property values
	kexipluginsdbg << set["type"].value() << endl;
	QMap<QCString, QVariant> values = KoProperty::propertyValues(set);
	//remove internal values, to avoid creating custom field's properties
	QMap<QCString, QVariant>::Iterator it = values.begin();
	KexiDB::Field *field = new KexiDB::Field();

	while (it!=values.end()) {
		const QString propName( it.key() );
		if (d->internalPropertyNames.find(propName.toLatin1()) || propName.startsWith("this:")
			|| (/*sanity*/propName=="objectType" && KexiDB::Field::BLOB != KexiDB::intToFieldType( set["type"].value().toInt() )))
		{
			QMap<QCString, QVariant>::Iterator it_tmp = it;
			++it;
			values.remove(it_tmp);
		}
		else
			++it;
	}
	//assign properties to the field
	// (note that "objectType" property will be saved as custom property)
	if (!KexiDB::setFieldProperties( *field, values )) {
		delete field;
		return 0;
	}
	return field;
}

tristate KexiTableDesignerView::buildSchema(KexiDB::TableSchema &schema, bool beSilent)
{
	if (!d->view->acceptRowEdit())
		return cancelled;

	tristate res = true;
	//check for pkey; automatically add a pkey if user wanted
	if (!d->primaryKeyExists) {
		if (beSilent) {
			kexipluginsdbg << "KexiTableDesignerView::buildSchema(): no primay key defined..." << endl;
		}
		else {
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
				if (beSilent) {
					kexipluginswarn << 
						QString("KexiTableDesignerView::buildSchema(): no field caption entered at row %1...")
						.arg(i+1) << endl;
				}
				else {
					d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
					d->view->startEditCurrentCell();
					KMessageBox::information(this, i18n("You should enter field caption.") );
				}
				res = cancelled;
				break;
			}
			if (names[name]) {
				break;
			}
			names.insert( name, &dummy ); //remember
		}
	}
	if (res == true && no_fields) {//no fields added
		if (beSilent) {
			kexipluginswarn << 
				"KexiTableDesignerView::buildSchema(): no field defined..." << endl;
		}
		else {
			KMessageBox::sorry(this,
				i18n("You have added no fields.\nEvery table should have at least one field.") );
		}
		res = cancelled;
	}
	if (res == true && b && i<(int)d->sets->size()) {//found a duplicate
		if (beSilent) {
			kexipluginswarn << 
				QString("KexiTableDesignerView::buildSchema(): duplicated field name '%1'")
				.arg((*b)["name"].value().toString()) << endl;
		}
		else {
			d->view->setCursorPosition(i, COLUMN_ID_CAPTION);
			d->view->startEditCurrentCell();
//! @todo for "names hidden" mode we won't get this error because user is unable to change names
			KMessageBox::sorry(this,
				i18n("You have added \"%1\" field name twice.\nField names cannot be repeated. "
				"Correct name of the field.")
				.arg((*b)["name"].value().toString()) );
		}
		res = cancelled;
	}
	if (res == true) {
		//for every field, create KexiDB::Field definition
		for (i=0;i<(int)d->sets->size();i++) {
			KoProperty::Set *s = d->sets->at(i);
			if (!s)
				continue;
			KexiDB::Field * f = buildField( *s );
			if (!f)
				continue; //hmm?
			schema.addField(f);
			if (!(*s)["rowSource"].value().toString().isEmpty() && !(*s)["rowSourceType"].value().toString().isEmpty()) {
				//add lookup column
				KexiDB::LookupFieldSchema *lookupFieldSchema = new KexiDB::LookupFieldSchema();
				lookupFieldSchema->rowSource().setTypeByName( (*s)["rowSourceType"].value().toString() );
				lookupFieldSchema->rowSource().setName( (*s)["rowSource"].value().toString() );
				lookupFieldSchema->setBoundColumn( (*s)["boundColumn"].value().toInt() );
				lookupFieldSchema->setVisibleColumn( (*s)["visibleColumn"].value().toInt() );
//! @todo support columnWidths(), columnHeadersVisible(), maximumListRows(), limitToList(), displayWidget()
				if (!schema.setLookupFieldSchema(f->name(), lookupFieldSchema)) {
					kexipluginswarn << 
						"KexiTableDesignerView::buildSchema(): !schema.setLookupFieldSchema()" << endl;
					delete lookupFieldSchema;
					return false;
				}
			}
		}
	}
	return res;
}

//! @internal
//! A recursive function for copying alter table actions from undo/redo commands.
static void copyAlterTableActions(KCommand* command, KexiDB::AlterTableHandler::ActionList &actions)
{
	CommandGroup* cmdGroup = dynamic_cast<CommandGroup*>( command );
	if (cmdGroup) {//command group: flatten it
		for (QPtrListIterator<KCommand> it(cmdGroup->commands()); it.current(); ++it)
			copyAlterTableActions(it.current(), actions);
		return;
	}
	Command* cmd = dynamic_cast<Command*>( command );
	if (!cmd) {
		kexipluginswarn << "KexiTableDesignerView::copyAlterTableActions(): cmd is not of type 'Command'!" << endl;
		return;
	}
	KexiDB::AlterTableHandler::ActionBase* action = cmd->createAction();
	//some commands can contain null actions, e.g. "set visibility" command
	if (action)
		actions.append( action );
}

tristate KexiTableDesignerView::buildAlterTableActions(KexiDB::AlterTableHandler::ActionList &actions)
{
	actions.clear();
	kexipluginsdbg << "KexiTableDesignerView::buildAlterTableActions(): " << d->history->commands().count()
		<< " top-level command(s) to process..." << endl;
	for (QPtrListIterator<KCommand> it(d->history->commands()); it.current(); ++it) {
		copyAlterTableActions(it.current(), actions);
	}
	return true;
}

KexiDB::SchemaData* KexiTableDesignerView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
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
	if (res == true) {
		//todo
		KexiDB::Connection *conn = mainWin()->project()->dbConnection();
		res = conn->createTable(tempData()->table);
		if (res!=true)
			parentDialog()->setStatus(conn, "");
	}

	if (res == true) {
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

tristate KexiTableDesignerView::storeData(bool dontAsk)
{
	if (!tempData()->table || !m_dialog->schemaData()) {
		d->recentResultOfStoreData = false;
		return false;
	}

	tristate res = true;
	if (!d->tempStoreDataUsingRealAlterTable && !d->dontAskOnStoreData && !dontAsk) {
		bool emptyTable;
		const QString msg = d->messageForSavingChanges(emptyTable);
		if (!emptyTable) {
			if (KMessageBox::No == KMessageBox::questionYesNo(this, msg))
				res = cancelled;
		}
	}
	d->dontAskOnStoreData = false; //one-time use
	if (~res) {
		d->recentResultOfStoreData = res;
		return res;
	}
//		KMessageBox::information(this, i18n("Saving changes for existing table design is not yet supported."));
//		cancel = true;

	KexiDB::Connection *conn = mainWin()->project()->dbConnection();

	KexiDB::AlterTableHandler *alterTableHandler = 0;
	KexiDB::TableSchema *newTable = 0;
	if (d->tempStoreDataUsingRealAlterTable) {
		KexiDB::AlterTableHandler::ActionList actions;
		res = buildAlterTableActions( actions );
	//todo: result?
		alterTableHandler = new KexiDB::AlterTableHandler( *conn );
		alterTableHandler->setActions(actions);
	}
	else {
//! @todo temp; remove this:
		//keep old behaviour
		newTable = new KexiDB::TableSchema();
		//copy schema data
		static_cast<KexiDB::SchemaData&>(*newTable) = static_cast<KexiDB::SchemaData&>(*tempData()->table);
		res = buildSchema(*newTable);
		kexipluginsdbg << "KexiTableDesignerView::storeData() : BUILD SCHEMA:" << endl;
		newTable->debug();
	}

	if (res == true) {
		res = KexiTablePart::askForClosingObjectsUsingTableSchema(
			this, *conn, *tempData()->table,
			i18n("You are about to change the design of table \"%1\" "
			"but following objects using this table are opened:")
			.arg(tempData()->table->name()));
	}
	if (res == true) {
		if (d->tempStoreDataUsingRealAlterTable) {
			newTable = alterTableHandler->execute(tempData()->table->name(), res);
			kexipluginsdbg << "KexiTableDesignerView::storeData() : ALTER TABLE EXECUTE: " 
				<< res.toString() << endl;
			if (!res) {
				alterTableHandler->debugError();
				parentDialog()->setStatus(alterTableHandler, "");
			}
//! @todo: result?
		}
		else {
//! @todo temp; remove this:
			//keep old behaviour
			res = conn->alterTable(*tempData()->table, *newTable);
			if (!res)
				parentDialog()->setStatus(conn, "");
		}
		if (res == true) {
			//change current schema
			tempData()->table = newTable;
			tempData()->tableSchemaChangedInPreviousView = true;
		}
		else {
			delete newTable;
		}
	}
	delete alterTableHandler;
	d->recentResultOfStoreData = res;
	return res;
}

tristate KexiTableDesignerView::simulateAlterTableExecution(QString *debugTarget)
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
	if (mainWin()->activeWindow() != parentDialog()) //to avoid executing for multiple alter table views
		return false;
	if (!tempData()->table || !m_dialog->schemaData())
		return false;
	KexiDB::Connection *conn = mainWin()->project()->dbConnection();
	KexiDB::AlterTableHandler::ActionList actions;
	tristate res = buildAlterTableActions( actions );
//todo: result?
	KexiDB::AlterTableHandler alterTableHandler( *conn );
	alterTableHandler.setActions(actions);

	if (debugTarget)
		return alterTableHandler.simulateExecution(tempData()->table->name(), *debugTarget);
	else
		return alterTableHandler.execute(tempData()->table->name(), res, /*simulate*/true);
# else
	return false;
# endif
#else
	return false;
#endif
}

void KexiTableDesignerView::slotSimulateAlterTableExecution()
{
	(void)simulateAlterTableExecution(0);
}

tristate KexiTableDesignerView::executeRealAlterTable()
{
	QSignal signal;
	signal.connect( mainWin(), SLOT(slotProjectSave()) );
	d->tempStoreDataUsingRealAlterTable = true;
	d->recentResultOfStoreData = false;
	signal.activate(); //will call KexiMainWindowImpl::slotProjectSaveAs() and thus storeData()
	d->tempStoreDataUsingRealAlterTable = false;
	return d->recentResultOfStoreData;
}

KexiTablePart::TempData* KexiTableDesignerView::tempData() const
{
	return static_cast<KexiTablePart::TempData*>(parentDialog()->tempData());
}

/*void KexiTableDesignerView::slotAboutToUpdateRow(
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

#ifdef KEXI_DEBUG_GUI
void KexiTableDesignerView::debugCommand( KCommand* command, int nestingLevel )
{
	if (dynamic_cast<Command*>(command))
		KexiUtils::addAlterTableActionDebug(dynamic_cast<Command*>(command)->debugString(), nestingLevel);
	else
		KexiUtils::addAlterTableActionDebug(command->name(), nestingLevel);
	//show subcommands
	if (dynamic_cast<CommandGroup*>(command)) {
		for (QPtrListIterator<KCommand> it(dynamic_cast<CommandGroup*>(command)->commands()); it.current(); ++it) {
			debugCommand(it.current(), nestingLevel + 1);
		}
	}
}
#endif

void KexiTableDesignerView::addHistoryCommand( KCommand* command, bool execute )
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
	debugCommand( command, 0 );
# endif
	d->history->addCommand( command, execute );
	updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::updateUndoRedoActions()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
	setAvailable("edit_undo", d->historyActionCollection->action("edit_undo")->isEnabled());
	setAvailable("edit_redo", d->historyActionCollection->action("edit_redo")->isEnabled());
#endif
}

void KexiTableDesignerView::slotUndo()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
	KexiUtils::addAlterTableActionDebug(QString("UNDO:"));
# endif
	d->history->undo();
	updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::slotRedo()
{
#ifndef KEXI_NO_UNDOREDO_ALTERTABLE
# ifdef KEXI_DEBUG_GUI
	KexiUtils::addAlterTableActionDebug(QString("REDO:"));
# endif
	d->history->redo();
	updateUndoRedoActions();
#endif
}

void KexiTableDesignerView::slotCommandExecuted(KCommand *command)
{
#ifdef KEXI_DEBUG_GUI
	debugCommand( command, 1 );
#endif
}

void KexiTableDesignerView::slotAboutToShowContextMenu()
{
	//update title
	if (propertySet()) {
		const KoProperty::Set &set = *propertySet();
		QString captionOrName(set["caption"].value().toString());
		if (captionOrName.isEmpty())
			captionOrName = set["name"].value().toString();
//! @todo show "field" icon
		d->contextMenuTitle->setTitle( i18n("Table field \"%1\"").arg(captionOrName) );
	}
	else {
		d->contextMenuTitle->setTitle( i18n("Empty table row", "Empty Row") );
	}
}

QString KexiTableDesignerView::debugStringForCurrentTableSchema(tristate& result)
{
	KexiDB::TableSchema tempTable;
	//copy schema data
	static_cast<KexiDB::SchemaData&>(tempTable) = static_cast<KexiDB::SchemaData&>(*tempData()->table);
	result = buildSchema(tempTable, true /*beSilent*/);
	if (true!=result)
		return QString::null;
	return tempTable.debugString(false /*without name*/);
}

// -- low-level actions used by undo/redo framework

void KexiTableDesignerView::clearRow(int row, bool addCommand)
{
	if (!d->view->acceptRowEdit())
		return;
	KexiTableItem *item = d->view->itemAt(row);
	if (!item)
		return;
	//remove from prop. set
	d->sets->remove( row );
	//clear row in table view (just clear value in COLUMN_ID_TYPE column)
//	for (int i=0; i < (int)d->view->data()->columnsCount(); i++) {
	if (!addCommand) {
		d->addHistoryCommand_in_slotRowUpdated_enabled = false;
		d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
		d->slotBeforeCellChanged_enabled = false;
	}
	d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, QVariant());
	if (!addCommand) {
		d->addHistoryCommand_in_slotRowUpdated_enabled = true;
		d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
		d->slotBeforeCellChanged_enabled = true;
	}
	d->view->data()->saveRowChanges(*item, true);
}

void KexiTableDesignerView::insertField(int row, const QString& caption, bool addCommand)
{
	insertFieldInternal(row, 0, caption, addCommand);
}

void KexiTableDesignerView::insertField(int row, KoProperty::Set& set, bool addCommand)
{
	insertFieldInternal(row, &set, QString::null, addCommand);
}

void KexiTableDesignerView::insertFieldInternal(int row, KoProperty::Set* set, //const KexiDB::Field& field, 
	const QString& caption, bool addCommand)
{
	if (set && (!set->contains("type") || !set->contains("caption"))) {
		kexipluginswarn << "KexiTableDesignerView::insertField(): no 'type' or 'caption' property in set!" << endl;
		return;
	}
	if (!d->view->acceptRowEdit())
		return;
	KexiTableItem *item = d->view->itemAt(row);
	if (!item)
		return;
	if (!addCommand) {
		d->addHistoryCommand_in_slotRowUpdated_enabled = false;
		d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
		d->slotBeforeCellChanged_enabled = false;
	}
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_CAPTION, 
				set ? (*set)["caption"].value() : QVariant(caption));//field.caption());
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, 
				set ? (int)KexiDB::Field::typeGroup( (*set)["type"].value().toInt() )-1/*counting from 0*/
				: (((int)KexiDB::Field::TextGroup)-1)/*default type, counting from 0*/
				);
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_DESC, 
				set ? (*set)["description"].value() : QVariant());//field.description());
	if (!addCommand) {
		d->slotBeforeCellChanged_enabled = true;
	}
		//this will create a new property set:
		d->view->data()->saveRowChanges(*item);
		if (set) {
			KoProperty::Set *newSet = d->sets->at(row);
			if (newSet) {
				*newSet = *set; //deep copy
			}
			else {
				kexipluginswarn << "KexiTableDesignerView::insertField() !newSet, row==" << row << endl;
			}
		}
	if (!addCommand) {
		d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
		d->addHistoryCommand_in_slotRowUpdated_enabled = true;
	}
	d->view->updateRow( row );
	propertySetReloaded(true);
}

void KexiTableDesignerView::insertEmptyRow( int row, bool addCommand )
{
	if (!addCommand) {
		d->addHistoryCommand_in_slotRowInserted_enabled = false;
	}
		d->view->insertEmptyRow( row );
	if (!addCommand) {
		d->addHistoryCommand_in_slotRowInserted_enabled = true;
	}
}

/*void KexiTableDesignerView::deleteRow( int row )
{
	d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = false;
		d->view->deleteItem( d->view->data()->at(row) );
	d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = true;
}*/

void KexiTableDesignerView::deleteRow( int row, bool addCommand )
{
	KexiTableItem *item = d->view->itemAt( row );
	if (!item)
		return;
	if (!addCommand) {
		d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = false;
	}
	const bool res = d->view->deleteItem(item);
	if (!addCommand) {
		d->addHistoryCommand_in_slotAboutToDeleteRow_enabled = true;
	}
	if (!res)
		return;
}

void KexiTableDesignerView::changeFieldPropertyForRow( int row,
 const QCString& propertyName, const QVariant& newValue, 
 KoProperty::Property::ListData* const listData, bool addCommand )
{
#ifdef KEXI_DEBUG_GUI
	KexiUtils::addAlterTableActionDebug(QString("** changeFieldProperty: \"")
		+ QString(propertyName) + "\" to \"" + newValue.toString() + "\"", 2/*nestingLevel*/);
#endif
	if (!d->view->acceptRowEdit())
		return;

	KoProperty::Set* set = d->sets->at( row );
	if (!set || !set->contains(propertyName))
		return;
	KoProperty::Property &property = set->property(propertyName);
	if (listData) {
		if (listData->keys.isEmpty())
			property.setListData( 0 );
		else
			property.setListData( new KoProperty::Property::ListData(*listData) );
	}
	if (propertyName != "type") //delayed type update (we need to have subtype set properly)
		property.setValue(newValue);
	KexiTableItem *item = d->view->itemAt(row);
	Q_ASSERT(item);

	if (propertyName == "type") {
	//	d->addHistoryCommand_in_slotRowUpdated_enabled = false;
//		d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
		d->slotPropertyChanged_subType_enabled = false;
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_TYPE, 
				int( KexiDB::Field::typeGroup( newValue.toInt() ) )-1);
			d->view->data()->saveRowChanges(*item);
		d->addHistoryCommand_in_slotRowUpdated_enabled = true;
//		d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
	//	d->slotPropertyChanged_subType_enabled = true;
		property.setValue(newValue); //delayed type update (we needed to have subtype set properly)
	}

	if (!addCommand) {
		d->addHistoryCommand_in_slotRowUpdated_enabled = false;
		d->addHistoryCommand_in_slotPropertyChanged_enabled = false;
		d->slotPropertyChanged_subType_enabled = false;
	}
		//special cases: properties displayed within the data grid:
		if (propertyName == "caption") {
			if (!addCommand) {
				d->slotBeforeCellChanged_enabled = false;
			}
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_CAPTION, newValue);
			d->view->data()->saveRowChanges(*item);
			if (!addCommand) {
				d->slotBeforeCellChanged_enabled = true;
			}
		}
		else if (propertyName == "description") {
			if (!addCommand) {
				d->slotBeforeCellChanged_enabled = false;
			}
			d->view->data()->updateRowEditBuffer(item, COLUMN_ID_DESC, newValue);
			if (!addCommand) {
				d->slotBeforeCellChanged_enabled = true;
			}
			d->view->data()->saveRowChanges(*item);
		}
	if (!addCommand) {
		d->addHistoryCommand_in_slotPropertyChanged_enabled = true;
		d->addHistoryCommand_in_slotRowUpdated_enabled = true;
		d->slotPropertyChanged_subType_enabled = true;
	}
	d->view->updateRow( row );
}

void KexiTableDesignerView::changeFieldProperty( int fieldUID,
 const QCString& propertyName, const QVariant& newValue, 
 KoProperty::Property::ListData* const listData, bool addCommand )
{
	//find a property by UID
	const int row = d->sets->findRowForPropertyValue("uid", fieldUID);
	if (row<0) {
		kexipluginswarn << "KexiTableDesignerView::changeFieldProperty(): field with uid="<<fieldUID<<" not found!"<<endl;
		return;
	}
	changeFieldPropertyForRow(row, propertyName, newValue, listData, addCommand);
}

void KexiTableDesignerView::changePropertyVisibility(
	int fieldUID, const QCString& propertyName, bool visible )
{
#ifdef KEXI_DEBUG_GUI
	KexiUtils::addAlterTableActionDebug(QString("** changePropertyVisibility: \"")
		+ QString(propertyName) + "\" to \"" + (visible ? "true" : "false") + "\"", 2/*nestingLevel*/);
#endif
	if (!d->view->acceptRowEdit())
		return;

	//find a property by name
	const int row = d->sets->findRowForPropertyValue("uid", fieldUID);
	if (row<0)
		return;
	KoProperty::Set* set = d->sets->at( row );
	if (!set || !set->contains(propertyName))
		return;

	KoProperty::Property &property = set->property(propertyName);
	if (property.isVisible() != visible) {
		property.setVisible(visible);
		propertySetReloaded(true);
	}
}

void KexiTableDesignerView::propertySetSwitched()
{
	KexiDataTable::propertySetSwitched();

	//if (parentDialog()!=parentDialog()->mainWin()->currentDialog())
	//	return; //this is not the current dialog's view
	
	static_cast<KexiTablePart*>(parentDialog()->part())->lookupColumnPage()
		->assignPropertySet(propertySet());
}

#include "kexitabledesignerview.moc"
