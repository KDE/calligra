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

#ifndef KEXITABLEVIEWDATA_H
#define KEXITABLEVIEWDATA_H

#include <qptrlist.h>
#include <qvariant.h>
#include <qvaluevector.h>
#include <qstring.h>

#include "kexitableitem.h"

namespace KexiDB {
class Field;
class QuerySchema;
class RowEditBuffer;
class Cursor;
}

/*! Single column definition. */
class KEXIDATATABLE_EXPORT KexiTableViewColumn {
	public:
		KexiTableViewColumn();
		virtual ~KexiTableViewColumn();

		virtual bool acceptsFirstChar(const QChar& ch) const;

		QString caption;
		int type; //!< one of KexiDB::Field::Type
		uint width;
		bool readOnly : 1;
		bool isDBAware : 1; //!< true for KexiDBTableViewColumn object
		bool isNull() const;
		
/*		virtual QString caption() const;
		virtual void setCaption(const QString& c);
	*/	
	protected:
		//! special ctor that do not allocate d member;
		KexiTableViewColumn(bool);
		
//		class Private;
//		Private *d;
	friend class KexiTableViewData;
};

/*! KexiDBTableViewColumn reimplements KexiTableViewColumn for db-aware data. */
class KEXIDATATABLE_EXPORT KexiDBTableViewColumn : public KexiTableViewColumn {
	public:
		KexiDBTableViewColumn();
		KexiDBTableViewColumn(const KexiDB::QuerySchema &query, KexiDB::Field& f);
		virtual bool acceptsFirstChar(const QChar& ch) const;
	
		KexiDB::Field* field;
	protected:
};

/*! List of column definitions. */
typedef QPtrList<KexiTableViewColumn> KexiTableViewColumnList;
typedef QPtrListIterator<KexiTableViewColumn> KexiTableViewColumnListIterator;
//typedef QValueVector<KexiTableViewColumn> KexiTableViewColumnList;

typedef QPtrList<KexiTableItem> KexiTableViewDataBase;

/*! Reimplements QPtrList to allow configurable sorting.
	Original author: Till Busch.
	Reimplemented by Jaroslaw Staniek.

	Notes:
	- use QPtrList::inSort ( const type * item ) to insert an item if you want 
		to maintain sorting (it is very slow!)
	- An alternative, especially if you have lots of items, is to simply QPtrList::append() 
		or QPtrList::insert() them and then use single sort().

	\sa QPtrList.
*/
class KEXIDATATABLE_EXPORT KexiTableViewData : public KexiTableViewDataBase
{
public: 
	KexiTableViewData();

	KexiTableViewData(KexiDB::Cursor *c); //db-aware version

//	KexiTableViewData(KexiTableViewColumnList* cols);
	~KexiTableViewData();
//js	void setSorting(int key, bool order=true, short type=1);

	/*! Sets sorting for \a column. If \a column is -1, sorting is disabled. */
	void setSorting(int column, bool ascending=true);

	/*! \return the column number by which the data is sorted, 
	 or -1 if sorting is disabled. */
	int sortedColumn() const { return m_key; }

	/*! \return true if ascending sort order is set, or false if sorting is descending.
	 This is independant of whether data is sorted now.
	*/
	bool sortingAscending() const { return m_order == 1; }

	void addColumn( KexiTableViewColumn* col );

	virtual bool isDBAware();

	inline KexiDB::Cursor* cursor() const { return m_cursor; }

	uint columnsCount() const { return columns.count(); }

	inline KexiTableViewColumn* column(uint c) { return columns.at(c); }

	/*! Columns information */
	KexiTableViewColumnList columns;

	virtual bool isReadOnly() const { return m_readOnly; }
	virtual void setReadOnly(bool set) { m_readOnly = set; }

	virtual bool isInsertingEnabled() const { return m_insertingEnabled; }
	virtual void setInsertingEnabled(bool set) { m_insertingEnabled = set; }

	/*! Clears and initializes internal row edit buffer for incoming editing. 
	 Creates buffer using KexiDB::RowEditBuffer(false) (false means not db-aware type) id our data is not db-aware,
	 or db-aware buffer if data is db-aware (isDBAware()==true).
	 \sa KexiDB::RowEditBuffer
	*/
	void clearRowEditBuffer();

	/*! Updates internal row edit buffer: currently edited column (number \colnum) 
	 has now assigned new value of \a newval.
	 Uses column's caption to address the column in buffer 
	 if the buffer is of simple type, or db-aware buffer if (isDBAware()==true).
	 (then fields are addressed with KexiDB::Field, instead of caption strings).
	 \sa KexiDB::RowEditBuffer */
	void updateRowEditBuffer(int colnum, QVariant newval);

	inline KexiDB::RowEditBuffer* rowEditBuffer() const { return m_pRowEditBuffer; }

	bool saveRowChanges(KexiTableItem& item);

protected:
	virtual int compareItems(Item item1, Item item2);

	int cmpStr(Item item1, Item item2);
	int cmpInt(Item item1, Item item2);

	int			m_key;
	short		m_order;
	short		m_type;
	static unsigned short charTable[];
	KexiDB::RowEditBuffer *m_pRowEditBuffer;
	KexiDB::Cursor *m_cursor;

	//! used to faster lookup columns of simple type (not dbaware)
//	QDict<KexiTableViewColumn> *m_simpleColumnsByName;

	bool m_readOnly : 1;
	bool m_insertingEnabled : 1;

	int (KexiTableViewData::*cmpFunc)(void *, void *);
};

#endif
