/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
 
   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexitableviewdata.h"

#include "kexivalidator.h"

#include <kexidb/field.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>

#include <kdebug.h>
#include <klocale.h>

unsigned short KexiTableViewData::charTable[]=
{
	#include "chartable.txt"
};

KexiTableViewColumn::KexiTableViewColumn(KexiDB::Field& f, bool owner)
: field(&f)
{
	isDBAware = false;
	m_fieldOwned = owner;

	m_nameOrCaption = field->captionOrName();
	m_readOnly = false;
	m_validator = 0;
}

KexiTableViewColumn::KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
	uint cconst,
	uint options,
	uint length, uint precision,
	QVariant defaultValue,
	const QString& caption, const QString& description, uint width)
{
	field = new KexiDB::Field(
		name, ctype,
		cconst,
		options,
		length, precision,
		defaultValue,
		caption, description, width);

	isDBAware = false;
	m_nameOrCaption = field->captionOrName();
	m_fieldOwned = true;
	m_readOnly = false;
	m_validator = 0;
}

KexiTableViewColumn::KexiTableViewColumn(
	const KexiDB::QuerySchema &query, KexiDB::Field& f)
: field(&f)
{
	isDBAware = true;
	m_fieldOwned = false;
	//setup column's caption:
	if (!field->caption().isEmpty()) {
		m_nameOrCaption = field->caption();
	}
	else {
		//reuse alias if available:
		m_nameOrCaption = query.alias(field);
		//last hance: use field name
		if (m_nameOrCaption.isEmpty())
			m_nameOrCaption = field->name();
	}
	//setup column's readonly flag: true if this is parent table's field
	m_readOnly = (query.parentTable()!=f.table());
	m_validator = 0;
}

KexiTableViewColumn::KexiTableViewColumn(bool)
: field(0)
{
	isDBAware = false;
	m_validator = 0;
}

KexiTableViewColumn::~KexiTableViewColumn()
{
	if (m_fieldOwned)
		delete field;
	setValidator( 0 );
}

void KexiTableViewColumn::setValidator( KexiValidator* v )
{
	if (m_validator) {//remove old one
		if (!m_validator->parent()) //destroy if has no parent
			delete m_validator;
	}
	m_validator = v;
}

bool KexiTableViewColumn::acceptsFirstChar(const QChar& ch) const
{
	if (field->isNumericType()) {
		if (ch=="-")
			 return !field->isUnsigned();
		if (ch=="+" || (ch>="0" && ch<="9"))
			return true;
		return false;
	}
	else if (field->type() == KexiDB::Field::Boolean)
		return false;
		
	return true;
}

//------------------------------------------------------

#if 0 //merged with KexiTableViewColumn
KexiDBTableViewColumn::KexiDBTableViewColumn()
 : KexiTableViewColumn(), field(0)
{
	isDBAware = true;
}

KexiDBTableViewColumn::KexiDBTableViewColumn(
	const KexiDB::QuerySchema &query, KexiDB::Field& f)
 : KexiTableViewColumn(true), field(&f)
{
	isDBAware = true;
	type = field->type();

	if (!field->caption().isEmpty()) {
		caption = field->caption();
	}
	else {
		//reuse alias if available:
		caption = query.alias(field);
		//last hance: use field name
		if (caption.isEmpty())
			caption = field->name();
	}
}

bool KexiDBTableViewColumn::acceptsFirstChar(const QChar& ch) const
{
	if (field->isNumericType()) {
		if (ch=="-")
			 return !field->isUnsigned();
		if (ch=="+" || (ch>="0" && ch<="9"))
			return true;
		return false;
	}
	else if (field->type() == KexiDB::Field::Boolean)
		return false;
		
	return true;
}
#endif

//------------------------------------------------------

KexiTableViewData::KexiTableViewData()
	: QObject()
	, KexiTableViewDataBase()
	, m_key(0)
	, m_order(1)
	, m_type(1)
	, m_pRowEditBuffer(0)
	, m_cursor(0)
	, m_readOnly(false)
	, m_insertingEnabled(true)
{
	setAutoDelete(true);
	columns.setAutoDelete(true);
}

KexiTableViewData::KexiTableViewData(KexiDB::Cursor *c)
	: QObject()
	, KexiTableViewDataBase()
	, m_key(0)
	, m_order(1)
	, m_type(1)
	, m_pRowEditBuffer(0)
	, m_cursor(c)
	, m_readOnly(false)
	, m_insertingEnabled(true)
{
	setAutoDelete(true);
	columns.setAutoDelete(true);
}

/*
KexiTableViewData::KexiTableViewData(KexiTableViewColumnList& cols) 
	: KexiTableViewDataBase()
	, columns(cols)
	, m_key(0)
	, m_order(1)
	, m_type(1)
	, m_pRowEditBuffer(0)
	, m_readOnly(false)
	, m_insertingEnabled(true)
{
	setAutoDelete(true);
	columns.setAutoDelete(true);
}*/

KexiTableViewData::~KexiTableViewData()
{
}

void KexiTableViewData::addColumn( KexiTableViewColumn* col )
{
//	if (!col->isDBAware) {
//		if (!m_simpleColumnsByName)
//			m_simpleColumnsByName = new QDict<KexiTableViewColumn>(101);
//		m_simpleColumnsByName->insert(col->caption,col);//for faster lookup
//	}
	columns.append( col );
}

void KexiTableViewData::setSorting(int column, bool ascending)
{
	m_order = (ascending ? 1 : -1);

	if (column>=0 && column<(int)columns.count()) {
		m_key = column;
	} 
	else {
		m_key = -1;
		return;
	}

	const int t = columns.at(m_key)->field->type();
	if (t == KexiDB::Field::Boolean || KexiDB::Field::isNumericType(t))
		cmpFunc = &KexiTableViewData::cmpInt;
	else
		cmpFunc = &KexiTableViewData::cmpStr;
}

int KexiTableViewData::compareItems(Item item1, Item item2)
{
	return ((this->*cmpFunc) (item1, item2));
}

int KexiTableViewData::cmpInt(Item item1, Item item2)
{
	return m_order* (((KexiTableItem *)item1)->at(m_key).toInt() - ((KexiTableItem *)item2)->at(m_key).toInt());
}

int KexiTableViewData::cmpStr(Item item1, Item item2)
{
	const QString &as =((KexiTableItem *)item1)->at(m_key).toString();
	const QString &bs =((KexiTableItem *)item2)->at(m_key).toString();

	const QChar *a = as.unicode();
	const QChar *b = bs.unicode();

	if ( a == b )
		return 0;
	if ( a == 0 )
		return 1;
	if ( b == 0 )
		return -1;

	unsigned short au;
	unsigned short bu;

	int l=QMIN(as.length(),bs.length());

	au = a->unicode();
	bu = b->unicode();
	au = (au <= 0x17e ? charTable[au] : 0xffff);
	bu = (bu <= 0x17e ? charTable[bu] : 0xffff);

	while (l-- && au == bu)
	{
		a++,b++;
		au = a->unicode();
		bu = b->unicode();
		au = (au <= 0x17e ? charTable[au] : 0xffff);
		bu = (bu <= 0x17e ? charTable[bu] : 0xffff);
	}

	if ( l==-1 )
		return m_order*(as.length()-bs.length());

	return m_order*(au-bu);
}

void KexiTableViewData::setReadOnly(bool set)
{
	if (m_readOnly == set)
		return;
	m_readOnly = set;
	if (m_readOnly)
		setInsertingEnabled(false);
}

void KexiTableViewData::setInsertingEnabled(bool set)
{
	if (m_insertingEnabled == set)
		return;
	m_insertingEnabled = set;
	if (m_insertingEnabled)
		setReadOnly(false);
}

void KexiTableViewData::clearRowEditBuffer()
{
	//init row edit buffer
	if (!m_pRowEditBuffer)
		m_pRowEditBuffer = new KexiDB::RowEditBuffer(isDBAware());
	else
		m_pRowEditBuffer->clear();
}

bool KexiTableViewData::isDBAware()
{
	return m_cursor!=0;
/*	if (columns.count()==0)
		return false;
	else
		return columns.first()->isDBAware;*/
}

bool KexiTableViewData::updateRowEditBuffer(KexiTableItem *item, 
	int colnum, QVariant newval)
{
	m_result.clear();
	emit aboutToChangeCell(item, colnum, newval, &m_result);
	if (!m_result.success)
		return false;

	kdDebug() << "KexiTableViewData::updateRowEditBuffer() column #" << colnum << " = " << newval.toString() << endl;
	KexiTableViewColumn* col = columns.at(colnum);
	if (!col) {
		kdDebug() << "KexiTableViewData::updateRowEditBuffer(): column #" << colnum<<" not found! col==0" << endl;
		return false;
	}
	if (!(col->field)) {
		kdDebug() << "KexiTableViewData::updateRowEditBuffer(): column #" << colnum<<" not found!" << endl;
		return false;
	}
	if (m_pRowEditBuffer->isDBAware()) {
//		if (!(static_cast<KexiDBTableViewColumn*>(col)->field)) {
		m_pRowEditBuffer->insert( *col->field, newval);
		return true;
	}
	//not db-aware:
	const QString colname = col->field->name();
	if (colname.isEmpty()) {
		kdDebug() << "KexiTableViewData::updateRowEditBuffer(): column #" << colnum<<" not found!" << endl;
		return false;
	}
	m_pRowEditBuffer->insert(colname, newval);
	return true;
}

//js TODO: if there can be multiple views for this data, we need multiple buffers!
bool KexiTableViewData::saveRow(KexiTableItem& item, bool insert)
{
	if (!m_pRowEditBuffer)
		return true; //nothing to do

	const bool dbaware = isDBAware();

	//check constraints:
	//-check if every NOT NULL and NOT EMPTY field is filled
	KexiTableViewColumn::ListIterator it_f(columns);
	KexiDB::RowData::iterator it_r = item.begin();
	int col = 0;
	for (;it_f.current() && it_r!=item.end();++it_f,++it_r,col++) {
		KexiDB::Field *f = it_f.current()->field;
		//get new value (of present in the buffer), or the old one, otherwise
		QVariant *val = rowEditBuffer()->at( *f );
		if (!val)
			val = &(*it_r); //get old value
		//check it
		if (val->isNull() && f->isNotNull()) {
			//NOT NULL violated
			m_result.msg = i18n("\"%1\" column requires a value to be entered.").arg(f->captionOrName());
			m_result.desc = i18n("The column's constraint is declared as NOT NULL.");
			m_result.column = col;
			return false;
		}
		else if (f->isNotEmpty() && (val->isNull() || KexiDB::isEmptyValue( f, *val )) ) {
			//NOT EMPTY violated
			m_result.msg = i18n("\"%1\" column requires a value to be entered.").arg(f->captionOrName());
			m_result.desc = i18n("The column's constraint is declared as NOT EMPTY.");
			m_result.column = col;
			return false;
		}
	}

	if (dbaware) {
		if (insert) {
			if (!m_cursor->insertRow( static_cast<KexiDB::RowData&>(item), *rowEditBuffer() )) {
				m_result.msg = i18n("Row inserting failed.");
				KexiDB::getHTMLErrorMesage(m_cursor, &m_result);

/*			if (desc)
			*desc = 
js: TODO: use KexiMainWindowImpl::showErrorMessage(const QString &title, KexiDB::Object *obj)
	after it will be moved somewhere to kexidb (this will require moving other 
	  showErrorMessage() methods from KexiMainWindowImpl to libkexiutils....)
	then: just call: *desc = KexiDB::errorMessage(m_cursor);
*/
				return false;
			}
		}
		else {
			if (!m_cursor->updateRow( static_cast<KexiDB::RowData&>(item), *rowEditBuffer() )) {
				m_result.msg = i18n("Row changing failed.");
				KexiDB::getHTMLErrorMesage(m_cursor, m_result.desc);
				return false;
			}
		}
	}
	else {//js UNTESTED!!! - not db-aware version
		KexiDB::RowEditBuffer::SimpleMap b = m_pRowEditBuffer->simpleBuffer();
		for (KexiDB::RowEditBuffer::SimpleMap::Iterator it = b.begin();it!=b.end();++it) {
			uint i=0;
			for (KexiTableViewColumn::ListIterator it2(columns);it2.current();++it2, i++) {
				if (it2.current()->field->name()==it.key()) {
					kdDebug() << it2.current()->field->name()<< ": "<<item[i].toString()<<" -> "<<it.data().toString()<<endl;
					item[i] = it.data();
				}
			}
		}
	}
	return true;
}

bool KexiTableViewData::saveRowChanges(KexiTableItem& item)
{
	kdDebug() << "KexiTableViewData::saveRowChanges()..." << endl;
	m_result.clear();
	emit aboutToUpdateRow(&item, rowEditBuffer(), &m_result);
	if (!m_result.success)
		return false;

	if (saveRow(item, false /*update*/)) {
		emit rowUpdated(&item);
		return true;
	}
	return false;
}

bool KexiTableViewData::saveNewRow(KexiTableItem& item)
{
	kdDebug() << "KexiTableViewData::saveNewRow()..." << endl;
	m_result.clear();
	emit aboutToInsertRow(&item, &m_result);
	if (!m_result.success)
		return false;
	
	if (saveRow(item, true /*insert*/)) {
		emit rowInserted(&item);
		return true;
	}
	return false;
}

bool KexiTableViewData::deleteRow(KexiTableItem& item)
{
	m_result.clear();
	emit aboutToDeleteRow(item, &m_result);
	if (!m_result.success)
		return false;

	if (isDBAware()) {
		m_result.success = false;
		if (!m_cursor->deleteRow( static_cast<KexiDB::RowData&>(item) )) {
			m_result.msg = i18n("Row deleting failed.");
/*js: TODO: use KexiDB::errorMessage() for description (desc) as in KexiTableViewData::saveRow() */
			KexiDB::getHTMLErrorMesage(m_cursor, &m_result);
			m_result.success = false;
			return false;
		}
	}

	if (!removeRef(&item)) {
		//aah - this shouldn't be!
		kdWarning() << "KexiTableViewData::deleteRow(): !removeRef() - IMPL. ERROR?" << endl;
		m_result.success = false;
		return false;
	}
	emit rowDeleted();
	return true;
}

void KexiTableViewData::insertRow(KexiTableItem& item, uint index)
{
	if (!insert( index = QMIN(index, count()), &item ))
		return;
	emit rowInserted(&item, index);
}

#include "kexitableviewdata.moc"

