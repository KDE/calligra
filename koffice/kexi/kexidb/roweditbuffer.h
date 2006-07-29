/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDB_ROWEDITBUFFER_H
#define KEXIDB_ROWEDITBUFFER_H

#include <qmap.h>

#include <kexidb/field.h>
#include <kexidb/queryschema.h>

namespace KexiDB {

/*! KexiDB::EditBuffer provides data for single edited row, 
	needed to perform update at the database backend.
	Its advantage over pasing e.g. KexiDB::FieldList objet is that 
	KexiDB::EditBuffer conatins only changed fields.

	Example usage (query is of type QuerySchema*):
	<code>
	EditBuffer buf;
	KexiDB::Field *f1 = query.field("name");
	KexiDB::Field *f2 = query.field("surname");
	buf[*f1]=QVariant("Joe");
	buf[*f2]=QVariant("Surname");
	//.. now use buf to add or edit record via connection ..
	</code>

	You can use QMap::clear() to clear buffer contents,
	QMap::isEmpty() to see if buffer is empty.
	For more, see QMap documentation.

	Notes: added fields should come from the same common QuerySchema object.
	However, it isn't checked at QValue& EditBuffer::operator[]( const Field& f ) level.
*/


class KEXI_DB_EXPORT RowEditBuffer {
public:
	typedef QMap<QString,QVariant> SimpleMap;
	typedef QMap<QueryColumnInfo*,QVariant> DBMap;

	RowEditBuffer(bool dbAwareBuffer);
	~RowEditBuffer();

	inline bool isDBAware() const { return m_dbBuffer!=0; }

	void clear();

	bool isEmpty() const;

	inline void insert( QueryColumnInfo& fi, QVariant &val )
		{ if (m_dbBuffer) m_dbBuffer->insert(&fi,val); }

	inline void insert( const QString& fname, QVariant &val ) 
		{ if (m_simpleBuffer) m_simpleBuffer->insert(fname,val); }

	//! useful only for db-aware buffer
	const QVariant* at( QueryColumnInfo& fi ) const;
	
	//! useful only for not-db-aware buffer
	const QVariant* at( Field& f ) const;
	const QVariant* at( const QString& fname ) const;

	inline const SimpleMap simpleBuffer() { return *m_simpleBuffer; }
	inline const DBMap dbBuffer() { return *m_dbBuffer; }

	//! for debugging purposes
	void debug();

protected:
	SimpleMap *m_simpleBuffer;
	SimpleMap::ConstIterator *m_simpleBufferIt;
	DBMap *m_dbBuffer;
	DBMap::ConstIterator *m_dbBufferIt;
};

} //namespace KexiDB

#endif
