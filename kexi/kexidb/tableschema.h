/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_TABLE_H
#define KEXIDB_TABLE_H

#include <qvaluelist.h>
#include <qstring.h>

#include <kexidb/fieldlist.h>
#include <kexidb/schemadata.h>
#include <kexidb/indexschema.h>

namespace KexiDB {

class Connection;

/*! KexiDB::TableSchema provides information about native database table 
	that can be stored using SQL database engine. 
*/	
/*OLD:	In most cases this class is used internally, while KexiDB::TableDef 
	is visible for the users. 
	Use KexiDB::TableDef subclass to get more rich structure (meta data) 
	that is extension of KexiDB::Table.
*/

class KEXI_DB_EXPORT TableSchema : public FieldList, public SchemaData
{
	public:
		TableSchema(const QString & name);
		TableSchema();
		virtual ~TableSchema();
//		QStringList primaryKeys() const;
//		bool hasPrimaryKeys() const;
		
//		virtual KexiDB::FieldList& addField(KexiDB::Field* field);

		/*! \return list of fields that are primary key of this table.
		 This method never returns NULL value,
		 if there is no primary key, empty IndexSchema object is returned.
		 IndexSchema object is owned by the table schema. */
		IndexSchema* primaryKey() const;

//js		void addPrimaryKey(const QString& key);

		/*! Removes all fields from the list, clears name and all other properties. 
			\sa FieldList::clear() */
		virtual void clear();

		//! writes debug information to stderr
		virtual void debug() const;

		/*! if table was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection();
	protected:
		/*! Automatically retrieves table schema via connection. */
		TableSchema(Connection *conn, const QString & name = QString::null);

	//js	QStringList m_primaryKeys;
//		Field::List m_fields;
		IndexSchema::List m_indices;

//		int m_id; //! unique identifier used in kexi__tables for this table

		Connection *m_conn;
		
		IndexSchema *m_pkey;

	friend class Connection;
};

/*
class KEXI_DB_EXPORT TableDef : protected Table
{
	public:
		TableDef(const QString & name);
		TableDef();
		~TableDef();
		KexiDB::FieldDef field(unsigned int id) const;
	protected:
};*/

/*
class KexiDBTableFields: public QValueList<KexiDBField> {
public:
	KexiDBTable(const QString & name);
	~KexiDBTable();
	void addField(KexiDBField);
//	const QString& tableName() const;

private:
//	QString m_tableName;
};
*/

} //namespace KexiDB

#endif
