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

#include "kexitableview.h"

#define MAX_FIELDS 101 //nice prime number

KexiAlterTableDialog::KexiAlterTableDialog(KexiMainWindow *win, QWidget *parent, 
	KexiDB::TableSchema &table, const char *name)
 : KexiViewBase(win, parent, name)
{
	m_dirty = false;
	m_table = &table; //orig table
	m_newTable = new KexiDB::TableSchema(*m_table); //deep copy of the original table
	m_fields.resize(MAX_FIELDS);
	m_fields.setAutoDelete(true);
	m_row = -99;
	m_currentBufferCleared = false;
	init();
}

KexiAlterTableDialog::~KexiAlterTableDialog()
{
	removeCurrentPropertyBuffer();
}

void KexiAlterTableDialog::init()
{
	KexiTableViewData *data = new KexiTableViewData();
	data->setInsertingEnabled( false );
	KexiTableViewColumn *col = new KexiTableViewColumn(i18n("Field name"), KexiDB::Field::Text);
//		KexiDB::Field::PrimaryKey);
	KexiValidator *vd = new Kexi::IdentifierValidator();
	vd->setAcceptsEmptyValue(true);
	col->setValidator( vd );

	data->addColumn( col );
	KexiDB::Field *f = new KexiDB::Field(i18n("Data type"), KexiDB::Field::Enum);
//		KexiDB::Field::NotEmpty | KexiDB::Field::NotNull);
	QValueVector<QString> types(KexiDB::Field::LastTypeGroup);
	for (int i=1; i<=KexiDB::Field::LastTypeGroup; i++) {
		types[i-1] = KexiDB::Field::typeGroupName(i);
	}
	f->setEnumHints(types);

	data->addColumn( new KexiTableViewColumn(*f) );
	data->addColumn( new KexiTableViewColumn(i18n("Comments"), KexiDB::Field::Text) );

/*	KexiTableItem *item = new KexiTableItem(0);
	item->push_back(QVariant("name"));
	item->push_back(QVariant("Text"));
	item->push_back(QVariant(""));
	data->append(item);
*/
	
	//add column data
	for(unsigned int i=0; i < m_newTable->fieldCount(); i++)
	{
		KexiDB::Field *field = m_newTable->field(i);
		KexiTableItem *item = new KexiTableItem(0);
		item->push_back(QVariant(field->name()));
		item->push_back(QVariant(field->typeGroup()-1)); //-1 because type groups are counted from 1
		item->push_back(QVariant(field->helpText()));
		data->append(item);

		createPropertyBuffer( i, field );
	}

	//add empty space
	for (int i=m_newTable->fieldCount(); i<MAX_FIELDS; i++) {
//		KexiPropertyBuffer *buff = new KexiPropertyBuffer(this);
//		buff->insert("primaryKey", KexiProperty("pkey", QVariant(false, 4), i18n("Primary Key")));
//		buff->insert("len", KexiProperty("len", QVariant(200), i18n("Length")));
//		m_fields.insert(i, buff);
		KexiTableItem *item = new KexiTableItem(3);//3 empty fields
		data->append(item);
	}

//	QSplitter *splitter = new QSplitter(Vertical, this);

	kdDebug() << "KexiAlterTableDialog::init(): vector contains " << m_fields.size() << " items" << endl;

	m_view = new KexiTableView(data, this, "tableview");
	QVBoxLayout *box = new QVBoxLayout(this);
	box->addWidget(m_view);

	m_view->setNavigatorEnabled(false);
	m_view->setSortingEnabled(false);//no, sorting is not good idea here
	m_view->adjustColumnWidthToContents(0); //adjust column width
	m_view->adjustColumnWidthToContents(1); //adjust column width
	m_view->setColumnStretchEnabled( true, 2 ); //last column occupies the rest of the area
//	setFocusProxy(m_view);

	connect(m_view, SIGNAL(cellSelected(int,int)), 
		this, SLOT(slotCellSelected(int,int)));
	connect(data, SIGNAL(aboutToChangeCell(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)),
		this, SLOT(slotBeforeCellChanged(KexiTableItem*,int,QVariant,KexiDB::ResultInfo*)));
	connect(data, SIGNAL(rowUpdated(KexiTableItem*)),
		this, SLOT(slotRowUpdated(KexiTableItem*)));
	connect(data, SIGNAL(aboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*)),
		this, SLOT(slotAboutToInsertRow(KexiTableItem*,KexiDB::ResultInfo*)));
//	connect(data, SIGNAL(aboutToUpdateRow(KexiTableItem*,KexiDB::RowEditBuffer*,KexiDB::ResultInfo*)),
//		this, SLOT(slotAboutToUpdateRow(KexiTableItem*,KexiDB::RowEditBuffer*,KexiDB::ResultInfo*)));
	connect(data, SIGNAL(rowDeleted()), this, SLOT(slotRowDeleted()));
	

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

KexiPropertyBuffer *
KexiAlterTableDialog::createPropertyBuffer( int row, KexiDB::Field *field )
{
	QString typeName = "KexiDB::Field::" + field->typeGroupString();
	KexiPropertyBuffer *buff = new KexiPropertyBuffer(this, typeName);
	connect(buff,SIGNAL(propertyChanged(KexiPropertyBuffer&,KexiProperty&)),
		this, SLOT(slotPropertyChanged(KexiPropertyBuffer&,KexiProperty&)));
	//name
	KexiProperty *prop = new KexiProperty("name", QVariant(field->name()), i18n("Name"));
	prop->setVisible(false);
	buff->add(prop);
	//type
	prop = new KexiProperty("type", QVariant(field->type()), i18n("Type"));
	prop->setVisible(false);
	buff->add(prop);
	//subtype
	const QStringList slist = KexiDB::typeStringsForGroup(field->typeGroup());
	const QStringList nlist = KexiDB::typeNamesForGroup(field->typeGroup());
	kdDebug() << "KexiAlterTableDialog::init(): subType strings: " << 
		slist.join("|") << "\nnames: " << nlist.join("|") << endl;
	if (slist.count()>1) {//there is more than 1 type name
		buff->add(new KexiProperty("subType", field->typeString(), slist, nlist, i18n("Subtype")));
	}
	//caption
	buff->add( new KexiProperty("caption", QVariant(field->caption()), i18n("Caption") ) );
	//desc
	prop = new KexiProperty("description", QVariant(field->helpText()));
	prop->setVisible(false);
	buff->add(prop);
	//length
	int len = field->length();//TODO
	if(len == 0)
		len = field->precision();
	buff->add(new KexiProperty("length", (int)field->length()/*200?*/, i18n("Length")));
	//pkey
	buff->add(new KexiProperty("primaryKey", QVariant(field->isPrimaryKey(), 4), i18n("Primary Key")));

	m_fields.insert(row, buff);
	return buff;
}

void
KexiAlterTableDialog::initActions()
{
/*
	plugSharedAction("edit_delete_row", m_view, SLOT(deleteCurrentRow()));
	plugSharedAction("edit_delete_row", m_view->popup());
	m_view->plugSharedAction(sharedAction("edit_delete_row")); //for proper shortcut

	plugSharedAction("edit_delete",m_view, SLOT(deleteAndStartEditCurrentCell()));
	m_view->plugSharedAction(sharedAction("edit_delete")); //for proper shortcut

	plugSharedAction("data_save_row",m_view, SLOT(acceptRowEdit()));
	m_view->plugSharedAction(sharedAction("data_save_row")); //for proper shortcut

	slotCellSelected( m_view->currentColumn(), m_view->currentRow() );
	*/
}

QWidget* KexiAlterTableDialog::mainWidget() 
{
	return m_view;
}

QSize KexiAlterTableDialog::minimumSizeHint() const
{
//	QWidget*const w= (QWidget*const)mainWidget();
	return m_view->minimumSizeHint();
//	return mainWidget() ? mainWidget()->minimumSizeHint() : KMdiChildView::minimumSizeHint();
}

QSize KexiAlterTableDialog::sizeHint() const
{
	return m_view->sizeHint();
}

// update actions --------------


void KexiAlterTableDialog::slotCellSelected(int, int row)
{
	kdDebug() << "KexiAlterTableDialog::slotCellSelected()" << endl;
	if(row == m_row)
		return;
	m_row = row;
	propertyBufferSwitched();

//	m_properties->setBuffer(m_constraints.at(row));
}

bool KexiAlterTableDialog::beforeSwitchTo(int mode)
{
	if (mode==Kexi::DesignViewMode) {
		//todo
	}
	else if (mode==Kexi::DataViewMode) {
		//todo
		KexiDB::TableSchema *nt = new KexiDB::TableSchema(m_newTable->name());
		nt->setCaption(m_newTable->caption());

		KexiTableViewData *data = m_view->data();
		int i=0;
		for(KexiTableItem *it = data->first(); it; it = data->next(), i++)
		{
			if (!(*it)[0].toString().isEmpty()) {//for nonempty names:
				KexiProperty *prop = (*m_fields.at(i))["pkey"];
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

#if 0 //js
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
	}
	return true;
}

KexiPropertyBuffer *KexiAlterTableDialog::propertyBuffer()
{
	return m_fields.at(m_view->currentRow());
//	return m_currentBufferCleared ? 0 : m_fields.at(m_view->currentRow());
}

void KexiAlterTableDialog::removeCurrentPropertyBuffer()
{
	const int r = m_view->currentRow();
	KexiPropertyBuffer *buf = m_fields.at(r);
	if (!buf)
		return;
	buf->debug();
//	m_currentBufferCleared = true;
	m_fields.remove(r);
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

bool KexiAlterTableDialog::dirty()
{
	return m_dirty;
}

void KexiAlterTableDialog::slotPropertyChanged(KexiPropertyBuffer& buf,KexiProperty& prop)
{
	m_dirty = true;
	//TODO
}

void KexiAlterTableDialog::slotBeforeCellChanged(
	KexiTableItem *item, int colnum, QVariant newValue, KexiDB::ResultInfo* result)
{
	if (colnum==0) {//'name'
		//if 'type' is not filled yet
		if (!item->at(1).toString().isEmpty() && item->at(1).isNull()) {
			//auto select 1st row of 'type' column
			m_view->data()->updateRowEditBuffer(item, 1, QVariant((int)0));

			//save row
			m_view->acceptRowEdit();
		}
	}
	else if (colnum==1) {//'type'
		//if 'name' is already filled
		if (!item->at(0).toString().isEmpty()) {
			//save row
			m_view->acceptRowEdit();
		}
	}
}

void KexiAlterTableDialog::slotRowUpdated(KexiTableItem *item)
{
	m_dirty = true;

	//TODO
	//-check if the row was empty before updating
	//if yes: we want to add a property buffer for this new row (field)
	QString fieldName = item->at(0).toString();
	const bool buffer_allowed = !fieldName.isEmpty() && !item->at(1).isNull();

	if (!buffer_allowed && propertyBuffer()) {
		//there is a buffer, but it's not allowed - remove it:
		removeCurrentPropertyBuffer();

		//clear 'type' column:
		m_view->data()->clearRowEditBuffer();
		m_view->data()->updateRowEditBuffer(m_view->selectedItem(), 1, QVariant());
		m_view->data()->saveRowChanges(*m_view->selectedItem());
	
	} else if (buffer_allowed && !propertyBuffer()) {
		//-- create a new field:

		//take the 1st type for the group
		int typegroup = item->at(1).toInt() +1 /*counting from 1*/;
		KexiDB::TypeGroupList tlst = KexiDB::typesForGroup( (KexiDB::Field::TypeGroup)typegroup );
		if (tlst.isEmpty()) {//this should not be!
			kdWarning() << "KexiAlterTableDialog::slotRowUpdated(): no types for group " 
			<< typegroup << endl;
			return;
		}
		int fieldType = tlst.first();

		QString description = item->at(2).toString();

		KexiDB::Field *field = new KexiDB::Field(
			fieldName,
			(KexiDB::Field::Type)fieldType,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			/*length*/0, 
			/*precision*/0,
			/*defaultValue*/QVariant(),
			/*caption*/QString::null,
			/*helpText*/description,
			/*width*/0);

		m_newTable->addField( field );

		kdDebug() << "KexiAlterTableDialog::slotRowUpdated(): " << field->debugString() << endl;

		//create new property buffer:
		KexiPropertyBuffer *newbuff = createPropertyBuffer( m_view->currentRow(), field );
		//add a special property indicating that this is brand new buffer, 
		//not just old-changed
		KexiProperty* prop = new KexiProperty("newrow", QVariant());
		prop->setVisible(false);
		newbuff->add( prop );

//js TODO:
//		//add this field to the list of new fields

		//refresh property editor:
		propertyBufferSwitched();
	}
}

void KexiAlterTableDialog::slotAboutToInsertRow(KexiTableItem* item, 
	KexiDB::ResultInfo* result)
{
	m_dirty = true;
	//TODO
}

void KexiAlterTableDialog::slotRowDeleted()
{
	//remove current prop. buffer
	removeCurrentPropertyBuffer();

	//let's move up all buffers that are below that deleted
	m_fields.setAutoDelete(false);//to avoid auto deleting in insert()
	const int r = m_view->currentRow();
	for (int i=r;i<int(m_fields.size()-1);i++) {
		KexiPropertyBuffer *b = m_fields[i+1];
		m_fields.insert( i , b );
	}
	m_fields.insert( m_fields.size()-1, 0 );
	m_fields.setAutoDelete(true);

	propertyBufferSwitched();
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

