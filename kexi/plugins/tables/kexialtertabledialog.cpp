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
#include <kexitableviewpropertybuffer.h>
#include "kexipropertyeditor.h"
#include "kexidialogbase.h"
#include "kexitableview.h"

//#define MAX_FIELDS 101 //nice prime number

class KexiAlterTableDialogPrivate
{
	public:
		KexiAlterTableDialogPrivate()
		 : buffers(0)
		 , dontAskOnStoreData(false)
		 , slotTogglePrimaryKeyCalled(false)
		{}

		KexiTableViewData *data;

		KexiTableViewPropertyBuffer *buffers;

		int row; //!< used to know if a new row is selected in slotCellSelected()

		KToggleAction *action_toggle_pkey;

		//! internal
		int maxTypeNameTextWidth;
		//! Set to true in beforeSwitchTo() to avoid asking again in storeData()
		bool dontAskOnStoreData : 1;

		bool slotTogglePrimaryKeyCalled : 1;
};

//----------------------------------------------

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
	KexiDB::TableSchema *table, const char *name)
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

	col = new KexiTableViewColumn(i18n("Field name"), KexiDB::Field::Text);
//		KexiDB::Field::PrimaryKey);
	KexiValidator *vd = new Kexi::IdentifierValidator();
	vd->setAcceptsEmptyValue(true);
	col->setValidator( vd );

	d->data->addColumn( col );
	KexiDB::Field *f = new KexiDB::Field(i18n("Data type"), KexiDB::Field::Enum);
//		KexiDB::Field::NotEmpty | KexiDB::Field::NotNull);
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup);
	d->maxTypeNameTextWidth = 0;
	QFontMetrics fm(font());
	for (int i=1; i<=KexiDB::Field::LastTypeGroup; i++) {
		types[i-1] = KexiDB::Field::typeGroupName(i);
		d->maxTypeNameTextWidth = QMAX(d->maxTypeNameTextWidth, fm.width(types[i-1]));
	}
	f->setEnumHints(types);

	d->data->addColumn( new KexiTableViewColumn(*f) );
	d->data->addColumn( new KexiTableViewColumn(i18n("Comments"), KexiDB::Field::Text) );

	m_view->setSpreadSheetMode();
//	setFocusProxy(m_view);

	connect(d->data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)));
	connect(d->data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	connect(d->data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)),
		this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*,bool)));

	setMinimumSize(m_view->minimumSizeHint().width(),m_view->minimumSizeHint().height());
	m_view->setFocus();

	d->buffers = new KexiTableViewPropertyBuffer( this, m_view );
	connect(d->buffers, SIGNAL(rowDeleted()), this, SLOT(updateActions()));
	connect(d->buffers, SIGNAL(rowInserted()), this, SLOT(updateActions()));
	
	plugSharedAction("tablepart_toggle_pkey", this, SLOT(slotTogglePrimaryKey()));
	d->action_toggle_pkey = static_cast<KToggleAction*>( sharedAction("tablepart_toggle_pkey") );
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
//	removeCurrentPropertyBuffer();
	delete d;
}

void KexiAlterTableDialog::initData()
{
//	d->buffers->clear();

//	d->buffers.resize(MAX_FIELDS);
//	d->buffers.setAutoDelete(true);
//	m_row = -99;

	//add column data
	d->data->clear();
	int tableFieldCount = 0;
	if (tempData()->table) {
		tableFieldCount = tempData()->table->fieldCount();
		d->buffers->clear(tableFieldCount);

		for(int i=0; i < tableFieldCount; i++)
		{
			KexiDB::Field *field = tempData()->table->field(i);
			KexiTableItem *item = new KexiTableItem(0);
			item->push_back(QVariant(field->isPrimaryKey() ? "key" : ""));
			item->push_back(QVariant(field->name()));
			item->push_back(QVariant(field->typeGroup()-1)); //-1 because type groups are counted from 1
			item->push_back(QVariant(field->description()));
			d->data->append(item);
	
			createPropertyBuffer( i, field );
		}
	}
	else {
		d->buffers->clear();//default size
	}
	//add empty space
//	for (int i=tableFieldCount; i<MAX_FIELDS; i++) {
	for (int i=tableFieldCount; i<(int)d->buffers->size(); i++) {
//		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
//		buff->insert("primaryKey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
//		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
//		m_fields.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(3);//3 empty fields
		d->data->append(item);
	}

//	QSplitter *splitter = new QSplitter(Vertical, this);

	kdDebug() << "KexiAlterTableDialog::init(): vector contains " << d->buffers->size() << " items" << endl;

	m_view->setData(d->data);

	m_view->setColumnWidth(0, IconSize( KIcon::Small ) + 10);
	m_view->adjustColumnWidthToContents(1); //adjust column width
	m_view->setColumnWidth(2, d->maxTypeNameTextWidth + 2*m_view->rowHeight());
	m_view->setColumnStretchEnabled( true, 3 ); //last column occupies the rest of the area

	setDirty(false);
}

static bool updatePropertiesVisibility(KexiDB::Field::Type fieldType, KexiPropertyBuffer& buf)
{
	bool changed = false;
	KexiProperty *prop;
	bool visible;
	//if there is no more than 1 subType name: hide the property
	prop = &buf["subType"];
	visible = prop->keys() && prop->keys()->count()>1;
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &buf["unsigned"];
	visible = KexiDB::Field::isNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &buf["length"];
	visible = (fieldType == KexiDB::Field::Text);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		prop->setValue( visible ? KexiDB::Field::defaultTextLength() : 0, false );
		changed = true;
	}
	prop = &buf["precision"];
	visible = KexiDB::Field::isFPNumericType(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	prop = &buf["allowEmpty"];
	visible = KexiDB::Field::hasEmptyProperty(fieldType);
	if (prop->isVisible()!=visible) {
		prop->setVisible( visible );
		changed = true;
	}
	return changed;
}

KexiPropertyBuffer *
KexiAlterTableDialog::createPropertyBuffer( int row, KexiDB::Field *field, bool newOne )
{
	QString typeName = "KexiDB::Field::" + field->typeGroupString();
	KexiPropertyBuffer *buff = new KexiPropertyBuffer(this, typeName);
//	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
//		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));
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
	
	buff->add(prop = new KexiProperty("allowEmpty", QVariant(!field->isNotEmpty(), 4), i18n("Allow Zero\nSize")));

	buff->add(new KexiProperty("indexed", QVariant(field->isIndexed(), 4), i18n("Indexed")));

	updatePropertiesVisibility(field->type(), *buff);

	connect(buff, SIGNAL(propertyChanged(KexiPropertyBuffer&, KexiProperty&)),
		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&, KexiProperty&)));

	d->buffers->insert(row, buff, newOne);
	return buff;
}

void KexiAlterTableDialog::updateActions(bool activated)
{
/*! \todo check if we can set pkey for this column type (eg. BLOB?) */
	setAvailable("tablepart_toggle_pkey", propertyBuffer()!=0);
	if (!propertyBuffer())
		return;
	KexiPropertyBuffer &buf = *propertyBuffer();
	d->slotTogglePrimaryKeyCalled = true;
	 d->action_toggle_pkey->setChecked(buf["primaryKey"].value().toBool());
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
	if (!propertyBuffer())
		return;
	KexiPropertyBuffer &buf = *propertyBuffer();
	bool set = !buf["primaryKey"].value().toBool();
	buf["primaryKey"] = QVariant(set, 1);
	d->action_toggle_pkey->setChecked(set);

	if (m_view->selectedItem()) {
		//show key in the table
		m_view->data()->clearRowEditBuffer();
		m_view->data()->updateRowEditBuffer(m_view->selectedItem(), 0, QVariant(set ? "key" : ""));
		m_view->data()->saveRowChanges(*m_view->selectedItem(), true);
	}

	if (set) {
		//primary key is set, remove old pkey if exists
		KexiPropertyBuffer *b = 0;
		int i;
		const int count = (int)d->buffers->size();
		for (i=0; i<count; i++) {
			b = d->buffers->at(i);
			if (b && b!=&buf && (*b)["primaryKey"].value().toBool())
				break;
		}
		if (i<count) {//remove
			(*b)["primaryKey"] = QVariant(false, 0);
			//remove key from table
			m_view->data()->clearRowEditBuffer();
			KexiTableItem *item = m_view->itemAt(i);
			if (item) {
				m_view->data()->updateRowEditBuffer(item, 0, QVariant());
				m_view->data()->saveRowChanges(*item, true);
			}
		}
	}
	updateActions();
	d->slotTogglePrimaryKeyCalled = false;
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
	+ ( emptyTable ? QString::null : QString("\n\n") + i18n("Note: This table is already filled with data which will be removed.") );
}

bool KexiAlterTableDialog::beforeSwitchTo(int mode, bool &cancelled, bool &dontStore)
{
/*	if (mode==Kexi::DesignViewMode) {
		initData();
		return true;
	}
	else */
	if (mode==Kexi::DataViewMode) {
		if (!dirty() && parentDialog()->neverSaved()) {
			cancelled=true;
			KMessageBox::information(this, i18n("Cannot switch to data view, because table design is empty.\n"
				"First, please create your design.") );
			return true;
		}
//<temporary>
		else if (dirty() && !parentDialog()->neverSaved()) {
//			cancelled = (KMessageBox::No == KMessageBox::questionYesNo(this, i18n("Saving changes for existing table design is not yet supported.\nDo you want to discard your changes now?")));

			KexiDB::Connection *conn = mainWin()->project()->dbConnection();
			bool emptyTable;
			cancelled = (KMessageBox::No == KMessageBox::questionYesNo(this, 
				i18n("Saving changes for existing table design is now required.")
				+"\n"+messageForSavingChanges(emptyTable)));
			dontStore = cancelled;
			if (!dontStore)
				d->dontAskOnStoreData = true;
//			if (dontStore)
//				setDirty(false);
		}
//</temporary>
		//todo
		return true;
	}
	else if (mode==Kexi::TextViewMode) {
		//todo
	}
	return true;
}

bool
KexiAlterTableDialog::afterSwitchFrom(int mode, bool &cancelled)
{
	if (mode==Kexi::NoViewMode || mode==Kexi::DataViewMode) {
		initData();
	}
	return true;
}

KexiPropertyBuffer *KexiAlterTableDialog::propertyBuffer()
{
	return d->buffers ? d->buffers->currentPropertyBuffer() : 0;
}

/*
void KexiAlterTableDialog::removeCurrentPropertyBuffer()
{
	const int r = m_view->currentRow();
	KexiPropertyBuffer *buf = d->buffers.at(r);
	if (!buf)
		return;
	buf->debug();
//	m_currentBufferCleared = true;
	d->buffers.remove(r);
	propertyBufferSwitched();
//	delete buf;
//	m_currentBufferCleared = false;
}
*/

static KexiDB::Field::Type firstTypeForSelectedGroup( int typegroup )
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
			buf["name"] = newValue;
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
		buf["type"] = (int)fieldType;

		//-get subtypes for this type: keys (slist) and names (nlist)
		const QStringList slist = KexiDB::typeStringsForGroup(fieldTypeGroup);
		const QStringList nlist = KexiDB::typeNamesForGroup(fieldTypeGroup);
		KexiProperty *subTypeProperty = &buf["subType"];

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
		buf["description"] = item->at(2);
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
		d->buffers->removeCurrentPropertyBuffer();

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
		KexiPropertyBuffer *newbuff = createPropertyBuffer( m_view->currentRow(), &field, true );
//moved
		//add a special property indicating that this is brand new buffer, 
		//not just changed
//		KexiProperty* prop = new KexiProperty("newrow", QVariant());
//		prop->setVisible(false);
//		newbuff->add( prop );

		//refresh property editor:
		propertyBufferSwitched();
	}
}

void KexiAlterTableDialog::updateActions()
{
	updateActions(false);
}

void KexiAlterTableDialog::slotPropertyChanged(KexiPropertyBuffer &buf, KexiProperty &property)
{
	if (property.name()=="primaryKey") {
		if (property.value().toBool()==true) {
			//primary key implies some rules
			buf["unique"] = QVariant(true,1);
			buf["notNull"] = QVariant(true,1);
			buf["allowEmpty"] = QVariant(false,1);
			buf["indexed"] = QVariant(true,1);
		}
	}
//TODO: perhaps show a hint in help panel telling what happens?
	else if (property.value().toBool()==false) {
		if (property.name()=="indexed" || property.name()=="unique" || property.name()=="notNull")
			buf["primaryKey"] = QVariant(false,1);
		if (property.name()=="notNull")
			buf["allowEmpty"] = QVariant(true,1);
	}
}

void KexiAlterTableDialog::slotAboutToInsertRow(KexiTableItem* item, 
	KexiDB::ResultInfo* /*result*/, bool /*repaint*/)
{
	setDirty();
	//TODO
}

bool KexiAlterTableDialog::buildSchema(KexiDB::TableSchema &schema, bool &cancel)
{
	if (!m_view->acceptRowEdit()) {
		cancel = true;
		return false;
	}
	bool ok = true;
	//check for duplicates
	KexiPropertyBuffer *b = 0;
	bool no_fields = true;
	int i;
	QDict<char> names(101, false);
	char dummy;
	for (i=0;i<(int)d->buffers->size();i++) {
		b = d->buffers->at(i);
		if (b) {
			no_fields = false;
			const QString name = (*b)["name"].value().toString();
			if (name.isEmpty()) {
				m_view->setCursor(i, 0);
				m_view->startEditCurrentCell();
				KMessageBox::information(this, i18n("You should enter field name.") );
				cancel = true;
				ok = false;
				break;
			}
			if (names[name]) {
				break;
			}
			names.insert( name, &dummy ); //remember
		}
	}
	if (ok && no_fields) {//no fields added
		KMessageBox::information(this, i18n("You have added no fields.\nEvery table should have at least one field.") );
		cancel = true;
		ok = false;
	}
	if (ok && b && i<(int)d->buffers->size()) {//found a duplicate
		m_view->setCursor(i, 0);
		m_view->startEditCurrentCell();
		KMessageBox::information(this, i18n("You have added \"%1\" field name twice.\nField names cannot be repeated. Correct name of the field.")
			.arg((*b)["name"].value().toString()) );
		cancel = true;
		ok = false;
	}
	if (ok) {
		//for every field, create KexiDB::Field definition
		for (i=0;i<(int)d->buffers->size();i++) {
			KexiPropertyBuffer *b = d->buffers->at(i);
			if (!b)
				continue;
			KexiPropertyBuffer &buf = *b;
			uint constraints = 0;
			uint options = 0;
			if (buf["primaryKey"].value().toBool())
				constraints |= KexiDB::Field::PrimaryKey;
			if (buf["unique"].value().toBool())
				constraints |= KexiDB::Field::Unique;
			if (buf["notnull"].value().toBool())
				constraints |= KexiDB::Field::NotNull;
			if (!buf["allowEmpty"].value().toBool())
				constraints |= KexiDB::Field::NotEmpty;

			if (buf["unsigned"].value().toBool())
				options |= KexiDB::Field::Unsigned;
				
	//		int type = buf["type"].value().toInt();
	//		if (type < 0 || type > (int)KexiDB::Field::LastType)
	//			type = KexiDB::Field::Text;
			kdDebug() << buf["subType"].value().toString() << endl;
	//		int typeGroup = KexiDB::typeGroup(type);
			QString typeString = buf["subType"].value().toString();
			KexiDB::Field::Type type = KexiDB::Field::typeForString(typeString);
			if (type==KexiDB::Field::InvalidType)
				type = KexiDB::Field::Text;

			KexiDB::Field *f = new KexiDB::Field( 
				buf["name"].value().toString(),
				type,
				constraints,
				options,
				buf["length"].value().toInt(),
				buf["precision"].value().toInt(),
				buf["defaultValue"].value(),
				buf["caption"].value().toString(),
				buf["description"].value().toString(),
				buf["width"].value().toInt()
			);
			schema.addField(f);
		}
	}
	return ok;
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

	bool ok = buildSchema(*tempData()->table, cancel) && !cancel;

	//FINALLY: create table:
	if (ok) {
		//todo
		KexiDB::Connection *conn = mainWin()->project()->dbConnection();
		ok = conn->createTable(tempData()->table);
		//todo: show err...?
	}

	if (ok) {
		//we've current schema
		tempData()->tableSchemaChangedInPreviousView = true;
	}
	if (!ok) {
		delete tempData()->table;
		tempData()->table = 0;
	}
	return tempData()->table;
}

bool KexiAlterTableDialog::storeData(bool &cancel)
{
	if (!tempData()->table || !m_dialog->schemaData())
		return 0;

	if (!d->dontAskOnStoreData) {
		bool emptyTable;
		const QString msg = messageForSavingChanges(emptyTable);
		if (!emptyTable)
			cancel = (KMessageBox::No == KMessageBox::questionYesNo(this, msg));
	}
	d->dontAskOnStoreData = false; //one-time use
	if (cancel)
		return false;
//		KMessageBox::information(this, i18n("Saving changes for existing table design is not yet supported."));
//		cancel = true;

	KexiDB::TableSchema *newTable = new KexiDB::TableSchema(); 
	//copy schema data
	static_cast<KexiDB::SchemaData&>(*newTable) = static_cast<KexiDB::SchemaData&>(*tempData()->table);
	bool ok = buildSchema(*newTable, cancel) && !cancel;

	kdDebug() << "KexiAlterTableDialog::storeData() : BUILD SCHEMA:" << endl;
	newTable->debug();

	if (ok) {
		KexiDB::Connection *conn = mainWin()->project()->dbConnection();
		ok = conn->alterTable(*tempData()->table, *newTable);
	}
	if (ok) {
		//change current schema
		tempData()->table = newTable;
		tempData()->tableSchemaChangedInPreviousView = true;
	}
	else {
		delete newTable;
	}
	return ok;
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

