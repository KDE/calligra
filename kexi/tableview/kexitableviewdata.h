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

#include "kexitableitem.h"

/*! Single column definition. */
class KexiTableViewColumn {
	public:
		KexiTableViewColumn() 
		: type(QVariant::Invalid)
		, defaultValue(QVariant())
		, width(100)
		, readOnly(false)
		{}
		~KexiTableViewColumn() 
		{}

		QString caption;
		QVariant::Type type;
		QVariant defaultValue;
		uint width;
		bool readOnly : 1;
};

/*! List of column definitions. */
typedef QValueVector<KexiTableViewColumn> KexiTableViewColumnList;

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
	KexiTableViewData(KexiTableViewColumnList& cols);
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

	void addColumn( const KexiTableViewColumn& col ) { columns.append( col ); }

	uint columnsCount() const { return columns.count(); }

	/*! Columns information */
	KexiTableViewColumnList columns;

	virtual bool isReadOnly() const { return m_readOnly; }
	virtual void setReadOnly(bool set) { m_readOnly = set; }

	virtual bool isInsertingEnabled() const { return m_insertingEnabled; }
	virtual void setInsertingEnabled(bool set) { m_insertingEnabled = set; }
	
protected:
	virtual int compareItems(Item item1, Item item2);

	int cmpStr(Item item1, Item item2);
	int cmpInt(Item item1, Item item2);

	int			m_key;
	short		m_order;
	short		m_type;
	static unsigned short charTable[];
	bool m_readOnly : 1;
	bool m_insertingEnabled : 1;
	
	int (KexiTableViewData::*cmpFunc)(void *, void *);
};

#endif
