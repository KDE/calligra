/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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
 
   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexitableviewdata.h"

#include <kexiutils/validator.h>

#include <kexidb/field.h>
#include <kexidb/roweditbuffer.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/error.h>
#include <kexi.h>

#include <kdebug.h>
#include <klocale.h>

#include <QApplication>

unsigned short charTable[]=
{
	#include "chartable.txt"
};

//-------------------------------

//! @internal A functor used in qSort() in order to sort by a given column
class LessThanFunctor
{
	private:
		bool m_ascendingOrder;
		QVariant m_leftTmp, m_rightTmp;
		int m_sortedColumn;

		bool (*m_lessThanFunction)(const QVariant&, const QVariant&);

#define CAST_AND_COMPARE(casting) \
	return left.casting() < right.casting()

		static bool cmpInt(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toInt);
		}

		static bool cmpUInt(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toUInt);
		}

		static bool cmpLongLong(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toLongLong);
		}

		static bool cmpULongLong(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toULongLong);
		}

		static bool cmpDouble(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toDouble);
		}

		static bool cmpDate(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toDate);
		}

		static bool cmpDateTime(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toDateTime);
		}

		static bool cmpTime(const QVariant& left, const QVariant& right)
		{
			CAST_AND_COMPARE(toDate);
		}

		static bool cmpString(const QVariant& left, const QVariant& right)
		{
			const QString &as = left.toString();
			const QString &bs = right.toString();

			const QChar *a = as.unicode();
			const QChar *b = bs.unicode();

			if ( a == b || b == 0 )
				return false;
			if ( a == 0 && b != 0 )
				return true;

			unsigned short au;
			unsigned short bu;
			int len = qMin(as.length(),bs.length());

			forever {
				au = a->unicode();
				bu = b->unicode();
				au = (au <= 0x17e ? charTable[au] : 0xffff);
				bu = (bu <= 0x17e ? charTable[bu] : 0xffff);

				if (len <= 0)
					return false;
				len--;

				if (au != bu)
					return au < bu;
				a++;
				b++;
			}
			return false;
		}

		//! Compare function for BLOB data (QByteArray). Uses size as the weight.
		static bool cmpBLOB(const QVariant& left, const QVariant& right)
		{
			return left.toByteArray().size() < right.toByteArray().size();
		}

	public:
		LessThanFunctor()
			: m_ascendingOrder(true)
			, m_lessThanFunction(0)
			, m_sortedColumn(-1)
		{
		}

		void setColumnType(const KexiDB::Field& field) {
			const KexiDB::Field::Type t = field.type();
			if (field.isTextType())
				m_lessThanFunction = &cmpString;
			if (KexiDB::Field::isFPNumericType(t))
				m_lessThanFunction = &cmpDouble;
			else if (t == KexiDB::Field::Integer && field.isUnsigned())
				m_lessThanFunction = &cmpUInt;
			else if (t == KexiDB::Field::Boolean || KexiDB::Field::isNumericType(t))
				m_lessThanFunction = &cmpInt; //other integers
			else if (t==KexiDB::Field::BigInteger) {
				if (field.isUnsigned())
					m_lessThanFunction = &cmpULongLong;
				else
					m_lessThanFunction = &cmpLongLong;
			}
			else if (t == KexiDB::Field::Date)
				m_lessThanFunction = &cmpDate;
			else if (t == KexiDB::Field::Time)
				m_lessThanFunction = &cmpTime;
			else if (t == KexiDB::Field::DateTime)
				m_lessThanFunction = &cmpDateTime;
			else if (t == KexiDB::Field::BLOB)
		//! @todo allow users to define BLOB sorting function?
				m_lessThanFunction = &cmpBLOB;
			else
				m_lessThanFunction = &cmpString; //anything else
		}

		void setAscendingOrder(bool ascending)
		{
			m_ascendingOrder = ascending;
		}

		void setSortedColumn(int column)
		{
			m_sortedColumn = column;
		}

#define _IIF(a,b) ((a) ? (b) : !(b))

		//! Main comparison operator that takes column number, type and order into account
		bool operator()(KexiDB::RecordData* record1, KexiDB::RecordData* record2)
		{
			// compare NULLs : NULL is smaller than everything
			if ((m_leftTmp = record1->at(m_sortedColumn)).isNull())
				return _IIF( m_ascendingOrder, !record2->at(m_sortedColumn).isNull() );
			if ((m_rightTmp = record2->at(m_sortedColumn)).isNull())
				return !m_ascendingOrder;

			return _IIF( m_ascendingOrder, m_lessThanFunction( m_leftTmp, m_rightTmp ) );
		}
};
#undef _IIF
#undef CAST_AND_COMPARE

//! @internal
class KexiTableViewData::Private
{
	public:
		Private()
		: sortedColumn(0)
		, realSortedColumn(0)
		, type(1)
		, pRowEditBuffer(0)
		, visibleColumnsCount(0)
		, visibleColumnsIDs(100)
		, globalColumnsIDs(100)
		, readOnly(false)
		, insertingEnabled(true)
		, containsROWIDInfo(false)
		, ascendingOrder(false)
		, descendingOrder(false)
		, autoIncrementedColumn(-2)
		{
		}

		//! (logical) sorted column number, set by setSorting()
		//! can differ from realSortedColumn if there's lookup column used
		int sortedColumn;

		//! real sorted column number, set by setSorting(), used by cmp*() methods
		int realSortedColumn;

//		int (KexiTableViewData::*cmpFunc)(void *, void *);
//		bool (KexiTableViewData::*lessThanFunction)(KexiDB::RecordData*, KexiDB::RecordData*);
		LessThanFunctor lessThanFunctor;

		short type;

		KexiDB::RowEditBuffer *pRowEditBuffer;

		QPointer<KexiDB::Cursor> cursor;

		KexiDB::ResultInfo result;

		uint visibleColumnsCount;

		QVector<int> visibleColumnsIDs, globalColumnsIDs;

		bool readOnly : 1;

		bool insertingEnabled : 1;

		/*! Used in acceptEditor() to avoid infinite recursion, 
		 eg. when we're calling KexiTableviewData::acceptRowEdit() during cell accepting phase. */
//		bool inside_acceptEditor : 1;

		//! @see KexiTableviewData::containsROWIDInfo()
		bool containsROWIDInfo : 1;

		//! true if ascending sort order is set
		bool ascendingOrder : 1;

		//! true if descending sort order is set
		bool descendingOrder : 1;

		int autoIncrementedColumn;
};

//-------------------------------

KexiTableViewData::KexiTableViewData()
	: QObject()
	, KexiTableViewDataBase()
	, d( new Private )
{
	init();
}

// db-aware ctor
KexiTableViewData::KexiTableViewData(KexiDB::Cursor *c)
	: QObject()
	, KexiTableViewDataBase()
	, d( new Private )
{
	init();
	d->cursor = c;
	d->containsROWIDInfo = d->cursor->containsROWIDInfo();
	if (d->cursor && d->cursor->query()) {
		const KexiDB::QuerySchema::FieldsExpandedOptions fieldsExpandedOptions
			= d->containsROWIDInfo ? KexiDB::QuerySchema::WithInternalFieldsAndRowID 
			: KexiDB::QuerySchema::WithInternalFields;
		m_itemSize = d->cursor->query()->fieldsExpanded( fieldsExpandedOptions ).count();
	}
	else
		m_itemSize = m_columns.count()+(d->containsROWIDInfo?1:0);

	// Allocate KexiTableViewColumn objects for each visible query column
	const KexiDB::QueryColumnInfo::Vector fields = d->cursor->query()->fieldsExpanded();
	const uint fieldsCount = fields.count();
	for (uint i=0;i < fieldsCount;i++) {
		KexiDB::QueryColumnInfo *ci = fields[i];
		if (ci->visible) {
			KexiDB::QueryColumnInfo *visibleLookupColumnInfo = 0;
			if (ci->indexForVisibleLookupValue() != -1) {
				//Lookup field is defined
				visibleLookupColumnInfo = d->cursor->query()->expandedOrInternalField( ci->indexForVisibleLookupValue() );
				/* not needed 
				if (visibleLookupColumnInfo) {
					// 2. Create a KexiTableViewData object for each found lookup field
				}*/
			}
			KexiTableViewColumn* col = new KexiTableViewColumn(*d->cursor->query(), *ci, visibleLookupColumnInfo);
			addColumn( col );
		}
	}
}

KexiTableViewData::KexiTableViewData(
	const QList<QVariant> &keys, const QList<QVariant> &values,
	KexiDB::Field::Type keyType, KexiDB::Field::Type valueType)
	: QObject()
	, KexiTableViewDataBase()
	, d( new Private )
{
	init(keys, values, keyType, valueType);
}

KexiTableViewData::KexiTableViewData(
	KexiDB::Field::Type keyType, KexiDB::Field::Type valueType)
	: QObject()
	, KexiTableViewDataBase()
	, d( new Private )
{
	const QList<QVariant> empty;
	init(empty, empty, keyType, valueType);
}

KexiTableViewData::~KexiTableViewData()
{
	emit destroying();
	clearInternal();
	qDeleteAll(m_columns);
	delete d;
}

void KexiTableViewData::init(
	const QList<QVariant> &keys, const QList<QVariant> &values,
	KexiDB::Field::Type keyType, KexiDB::Field::Type valueType)
{
	init();
	KexiDB::Field *keyField = new KexiDB::Field("key", keyType);
	keyField->setPrimaryKey(true);
	KexiTableViewColumn *keyColumn = new KexiTableViewColumn(*keyField, true);
	keyColumn->setVisible(false);
	addColumn(keyColumn);

	KexiDB::Field *valueField = new KexiDB::Field("value", valueType);
	KexiTableViewColumn *valueColumn = new KexiTableViewColumn(*valueField, true);
	addColumn(valueColumn);

	uint cnt = qMin(keys.count(), values.count());
	QList<QVariant>::ConstIterator it_keys = keys.constBegin();
	QList<QVariant>::ConstIterator it_values = values.constBegin();
	for (;cnt>0;++it_keys, ++it_values, cnt--) {
		KexiDB::RecordData *record = new KexiDB::RecordData(2);
		(*record)[0] = (*it_keys);
		(*record)[1] = (*it_values);
		append( record );
	}
}

void KexiTableViewData::init()
{
	m_itemSize = 0;
//Qt 4	setAutoDelete(true);
//Qt 4	m_columns.setAutoDelete(true);
}

void KexiTableViewData::deleteLater()
{
	d->cursor = 0;
	QObject::deleteLater();
}

void KexiTableViewData::addColumn( KexiTableViewColumn* col )
{
//	if (!col->isDBAware()) {
//		if (!d->simpleColumnsByName)
//			d->simpleColumnsByName = new QDict<KexiTableViewColumn>(101);
//		d->simpleColumnsByName->insert(col->caption,col);//for faster lookup
//	}
	m_columns.append( col );
	col->setData( this );
	if (d->globalColumnsIDs.size() < (int)m_columns.count()) {//sanity
		d->globalColumnsIDs.resize( d->globalColumnsIDs.size()*2 );
	}
	if (col->isVisible()) {
		d->visibleColumnsCount++;
		if ((uint)d->visibleColumnsIDs.size() < d->visibleColumnsCount) {//sanity
			d->visibleColumnsIDs.resize( d->visibleColumnsIDs.size()*2 );
		}
		d->visibleColumnsIDs[ m_columns.count()-1 ] = d->visibleColumnsCount-1;
		d->globalColumnsIDs[ d->visibleColumnsCount-1 ] = m_columns.count()-1;
	}
	else {
		d->visibleColumnsIDs[ m_columns.count()-1 ] = -1;
	}
	d->autoIncrementedColumn = -2; //clear cache;
	if (!d->cursor || !d->cursor->query())
		m_itemSize = m_columns.count()+(d->containsROWIDInfo?1:0);
}

int KexiTableViewData::globalColumnID(int visibleID) const
{
	return d->globalColumnsIDs.value( visibleID, -1 );
}
int KexiTableViewData::visibleColumnID(int globalID) const
{
	return d->visibleColumnsIDs.value( globalID, -1 );
}

bool KexiTableViewData::isDBAware() const
{
	return d->cursor != 0;
}

KexiDB::Cursor* KexiTableViewData::cursor() const
{
	return d->cursor;
}

bool KexiTableViewData::isInsertingEnabled() const
{
	return d->insertingEnabled;
}

KexiDB::RowEditBuffer* KexiTableViewData::rowEditBuffer() const
{
	return d->pRowEditBuffer;
}

const KexiDB::ResultInfo& KexiTableViewData::result() const
{
	return d->result;
}

bool KexiTableViewData::containsROWIDInfo() const
{
	return d->containsROWIDInfo;
}

QString KexiTableViewData::dbTableName() const
{
	if (d->cursor && d->cursor->query() && d->cursor->query()->masterTable())
		return d->cursor->query()->masterTable()->name();
	return QString();
}

void KexiTableViewData::setSorting(int column, bool ascending)
{
	if (column>=0 && column<(int)m_columns.count()) {
		d->ascendingOrder = ascending;
		d->descendingOrder = !ascending;
	} 
	else {
		d->ascendingOrder = false;
		d->descendingOrder = false;
		d->sortedColumn = -1;
		d->realSortedColumn = -1;
		return;
	}
	// find proper column information for sorting (lookup column points to alternate column with visible data)
	const KexiTableViewColumn *tvcol = m_columns.at(column);
	KexiDB::QueryColumnInfo* visibleLookupColumnInfo = tvcol->visibleLookupColumnInfo();
	const KexiDB::Field *field = visibleLookupColumnInfo ? visibleLookupColumnInfo->field : tvcol->field();
	d->sortedColumn = column;
	d->realSortedColumn = tvcol->columnInfo()->indexForVisibleLookupValue()!=-1 
		? tvcol->columnInfo()->indexForVisibleLookupValue() : d->sortedColumn;

	// setup compare functor
	d->lessThanFunctor.setColumnType(*field);
	d->lessThanFunctor.setAscendingOrder(ascending);
	d->lessThanFunctor.setSortedColumn(column);
}

int KexiTableViewData::sortedColumn() const
{
	return d->sortedColumn;
}

int KexiTableViewData::sortingOrder() const
{
	return d->ascendingOrder ? 1 : (d->descendingOrder ? -1 : 0);
}

void KexiTableViewData::sort()
{
	if (0 != sortingOrder())
		qSort(begin(), end(), d->lessThanFunctor);
}

/*
Qt 4
int KexiTableViewData::compareItems(Item record1, Item record2)
{
	return ((this->*cmpFunc) (record1, record2));
}*/

void KexiTableViewData::setReadOnly(bool set)
{
	if (d->readOnly == set)
		return;
	d->readOnly = set;
	if (d->readOnly)
		setInsertingEnabled(false);
}

void KexiTableViewData::setInsertingEnabled(bool set)
{
	if (d->insertingEnabled == set)
		return;
	d->insertingEnabled = set;
	if (d->insertingEnabled)
		setReadOnly(false);
}

void KexiTableViewData::clearRowEditBuffer()
{
	//init row edit buffer
	if (!d->pRowEditBuffer)
		d->pRowEditBuffer = new KexiDB::RowEditBuffer(isDBAware());
	else
		d->pRowEditBuffer->clear();
}

bool KexiTableViewData::updateRowEditBufferRef(KexiDB::RecordData *record, 
	int colnum, KexiTableViewColumn* col, QVariant& newval, bool allowSignals,
	QVariant *visibleValueForLookupField)
{
	d->result.clear();
	if (allowSignals)
		emit aboutToChangeCell(record, colnum, newval, &d->result);
	if (!d->result.success)
		return false;

	kDebug() << "KexiTableViewData::updateRowEditBufferRef() column #" 
		<< colnum << " = " << newval.toString() << endl;
	if (!col) {
		kWarning() << "KexiTableViewData::updateRowEditBufferRef(): column #" 
			<< colnum << " not found! col==0" << endl;
		return false;
	}
	if (!d->pRowEditBuffer)
		d->pRowEditBuffer = new KexiDB::RowEditBuffer(isDBAware());
	if (d->pRowEditBuffer->isDBAware()) {
		if (!(col->columnInfo())) {
			kWarning() << "KexiTableViewData::updateRowEditBufferRef(): column #" 
				<< colnum << " not found!" << endl;
			return false;
		}
		d->pRowEditBuffer->insert( *col->columnInfo(), newval);

		if (col->visibleLookupColumnInfo() && visibleValueForLookupField) {
			//this is value for lookup table: update visible value as well
			d->pRowEditBuffer->insert( *col->visibleLookupColumnInfo(), *visibleValueForLookupField);
		}
		return true;
	}
	if (!(col->field())) {
		kDebug() << "KexiTableViewData::updateRowEditBufferRef(): column #" << colnum<<" not found!" << endl;
		return false;
	}
	//not db-aware:
	const QString colname = col->field()->name();
	if (colname.isEmpty()) {
		kDebug() << "KexiTableViewData::updateRowEditBufferRef(): column #" << colnum<<" not found!" << endl;
		return false;
	}
	d->pRowEditBuffer->insert(colname, newval);
	return true;
}

//get a new value (if present in the buffer), or the old one, otherwise
//(taken here for optimization)
#define GET_VALUE if (!val) { \
	val = d->cursor \
				? d->pRowEditBuffer->at( *(*it_f)->columnInfo(), true /* useDefaultValueIfPossible */ ) \
				: d->pRowEditBuffer->at( *f ); \
	if (!val) \
		val = &(*it_r); /* get old value */ \
	}

//! @todo if there're multiple views for this data, we need multiple buffers!
bool KexiTableViewData::saveRow(KexiDB::RecordData& record, bool insert, bool repaint)
{
	if (!d->pRowEditBuffer)
		return true; //nothing to do

	//check constraints:
	//-check if every NOT NULL and NOT EMPTY field is filled
	KexiTableViewColumn::ListIterator it_f(m_columns.constBegin());
	KexiDB::RecordData::ConstIterator it_r = record.constBegin();
	int col = 0;
	const QVariant *val = 0;
	for (;it_f!=m_columns.constEnd() && it_r!=record.constEnd();++it_f,++it_r,col++) {
		KexiDB::Field *f = (*it_f)->field();
		if (f->isNotNull()) {
			GET_VALUE;
			//check it
			if (val->isNull() && !f->isAutoIncrement()) {
				//NOT NULL violated
				d->result.msg = i18n("\"%1\" column requires a value to be entered.",
					f->captionOrName()) + "\n\n" + Kexi::msgYouCanImproveData();
				d->result.desc = i18n("The column's constraint is declared as NOT NULL.");
				d->result.column = col;
				return false;
			}
		}
		if (f->isNotEmpty()) {
			GET_VALUE;
			if (!f->isAutoIncrement() && (val->isNull() || KexiDB::isEmptyValue( f, *val ))) {
				//NOT EMPTY violated
				d->result.msg = i18n("\"%1\" column requires a value to be entered.",
					f->captionOrName()) + "\n\n" + Kexi::msgYouCanImproveData();
				d->result.desc = i18n("The column's constraint is declared as NOT EMPTY.");
				d->result.column = col;
				return false;
			}
		}
	}

	if (d->cursor) {//db-aware
		if (insert) {
			if (!d->cursor->insertRow( record, *d->pRowEditBuffer, 
				d->containsROWIDInfo/*also retrieve ROWID*/ )) 
			{
				d->result.msg = i18n("Row inserting failed.") + "\n\n" 
					+ Kexi::msgYouCanImproveData();
				KexiDB::getHTMLErrorMesage(d->cursor, &d->result);

/*			if (desc)
			*desc = 
js: TODO: use KexiMainWindow::showErrorMessage(const QString &title, KexiDB::Object *obj)
	after it will be moved somewhere to kexidb (this will require moving other 
	  showErrorMessage() methods from KexiMainWindow to libkexiutils....)
	then: just call: *desc = KexiDB::errorMessage(d->cursor);
*/
				return false;
			}
		}
		else { // row updating
//			if (d->containsROWIDInfo)
//				ROWID = record[columns.count()].toULongLong();
			if (!d->cursor->updateRow( static_cast<KexiDB::RecordData&>(record), *d->pRowEditBuffer,
					d->containsROWIDInfo/*use ROWID*/))
			{
				d->result.msg = i18n("Row changing failed.") + "\n\n" + Kexi::msgYouCanImproveData();
//! @todo set d->result.column if possible
				KexiDB::getHTMLErrorMesage(d->cursor, d->result.desc);
				return false;
			}
		}
	}
	else {//not db-aware version
		KexiDB::RowEditBuffer::SimpleMap b = d->pRowEditBuffer->simpleBuffer();
		for (KexiDB::RowEditBuffer::SimpleMap::ConstIterator it = b.constBegin();it!=b.constEnd();++it) {
			uint i = -1;
			foreach (KexiTableViewColumn *col, m_columns) {
				i++;
				if (col->field()->name() == it.key()) {
					kDebug() << col->field()->name()<< ": "<< record.at(i).toString()
						<<" -> "<<it.value().toString()<<endl;
					record[i] = it.value();
				}
			}
		}
	}
	
	d->pRowEditBuffer->clear();

	if (repaint)
		emit rowRepaintRequested(record);
	return true;
}

bool KexiTableViewData::saveRowChanges(KexiDB::RecordData& record, bool repaint)
{
	kDebug() << "KexiTableViewData::saveRowChanges()..." << endl;
	d->result.clear();
	emit aboutToUpdateRow(&record, d->pRowEditBuffer, &d->result);
	if (!d->result.success)
		return false;

	if (saveRow(record, false /*update*/, repaint)) {
		emit rowUpdated(&record);
		return true;
	}
	return false;
}

bool KexiTableViewData::saveNewRow(KexiDB::RecordData& record, bool repaint)
{
	kDebug() << "KexiTableViewData::saveNewRow()..." << endl;
	d->result.clear();
	emit aboutToInsertRow(&record, &d->result, repaint);
	if (!d->result.success)
		return false;
	
	if (saveRow(record, true /*insert*/, repaint)) {
		emit rowInserted(&record, repaint);
		return true;
	}
	return false;
}

bool KexiTableViewData::deleteRow(KexiDB::RecordData& record, bool repaint)
{
	d->result.clear();
	emit aboutToDeleteRow(record, &d->result, repaint);
	if (!d->result.success)
		return false;

	if (d->cursor) {//db-aware
		d->result.success = false;
		if (!d->cursor->deleteRow( static_cast<KexiDB::RecordData&>(record), d->containsROWIDInfo/*use ROWID*/ )) {
			d->result.msg = i18n("Row deleting failed.");
/*js: TODO: use KexiDB::errorMessage() for description (desc) as in KexiTableViewData::saveRow() */
			KexiDB::getHTMLErrorMesage(d->cursor, &d->result);
			d->result.success = false;
			return false;
		}
	}

	int index = indexOf(&record);
	if (index == -1) {
		//aah - this shouldn't be!
		kWarning() << "KexiTableViewData::deleteRow(): !removeRef() - IMPL. ERROR?" << endl;
		d->result.success = false;
		return false;
	}
	removeAt( index );
	emit rowDeleted();
	return true;
}

void KexiTableViewData::deleteRows( const QList<int> &rowsToDelete, bool repaint )
{
	Q_UNUSED( repaint );

	if (rowsToDelete.isEmpty())
		return;
	int last_r = 0;
	KexiTableViewData::iterator it( begin() );
	for (QList<int>::ConstIterator r_it = rowsToDelete.constBegin(); r_it!=rowsToDelete.constEnd(); ++r_it) {
		for (; last_r<(*r_it); last_r++)
			++it;
		it = erase( it ); /* this will delete *it */
		last_r++;
	}
//DON'T CLEAR BECAUSE KexiTableViewPropertyBuffer will clear BUFFERS!
//-->	emit reloadRequested(); //! \todo more effective?
	emit rowsDeleted( rowsToDelete );
}

void KexiTableViewData::insertRow(KexiDB::RecordData& record, uint index, bool repaint)
{
	insert( index = qMin(index, count()), &record );
	emit rowInserted(&record, index, repaint);
}

void KexiTableViewData::clearInternal()
{
	clearRowEditBuffer();
//	qApp->processEvents( 1 );
//TODO: this is time consuming: find better data model
//	KexiTableViewDataBase::clear();
	const uint c = count();
	for (uint i=0; i<c; i++) {
		removeLast();
#ifndef KEXI_NO_PROCESS_EVENTS
		if (i % 1000 == 0)
			qApp->processEvents( QEventLoop::AllEvents, 1 );
#endif
	}
}

bool KexiTableViewData::deleteAllRows(bool repaint)
{
	clearInternal();

	bool res = true;
	if (d->cursor) {
		//db-aware
		res = d->cursor->deleteAllRows();
	}

	if (repaint)
		emit reloadRequested();
	return res;
}

int KexiTableViewData::autoIncrementedColumn()
{
	if (d->autoIncrementedColumn==-2) {
		//find such a column
		d->autoIncrementedColumn = -1;
		foreach (KexiTableViewColumn *col, m_columns) {
			d->autoIncrementedColumn++;
			if (col->field()->isAutoIncrement())
				break;
		}
	}
	return d->autoIncrementedColumn;
}

bool KexiTableViewData::preloadAllRows()
{
	if (!d->cursor)
		return false;

	//const uint fcount = d->cursor->fieldCount() + (d->containsROWIDInfo ? 1 : 0);
	if (!d->cursor->moveFirst() && d->cursor->error())
		return false;
	for (int i=0;!d->cursor->eof();i++) {
		KexiDB::RecordData *record = d->cursor->storeCurrentRow();
		if (!record) {
			clear();
			return false;
		}
//		record->debug();
		append( record );
		if (!d->cursor->moveNext() && d->cursor->error()) {
			clear();
			return false;
		}
#ifndef KEXI_NO_PROCESS_EVENTS
		if ((i % 1000) == 0)
			qApp->processEvents( QEventLoop::AllEvents, 1 );
#endif
	}
	return true;
}

bool KexiTableViewData::isReadOnly() const
{
	return d->readOnly || (d->cursor && d->cursor->connection()->isReadOnly());
}

#include "kexitableviewdata.moc"
