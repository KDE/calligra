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
#include <kexidb/index.h>

namespace KexiDB {

class Connection;

/*! KexiDB::Table provides information about native database table 
	that can be stored using SQL database engine. 
	
	In most cases this class is used internally, while KexiDB::TableDef 
	is visible for the users. 
	Use KexiDB::TableDef subclass to get more rich structure (meta data) 
	that is extension of KexiDB::Table.
*/

class KEXI_DB_EXPORT Table : public FieldList
{
	public:
		Table(const QString & name);
		Table();
		~Table();
		const QString& name() const;
		void setName(const QString& name);
//		unsigned int fieldCount() const;
//		KexiDB::Field field(unsigned int id) const;
		QStringList primaryKeys() const;
		bool hasPrimaryKeys() const;
		virtual void addField(KexiDB::Field* field);

		int id() { return m_id; }
//		Field::List::iterator fields() { return m_fields.begin(); }
//js		void addPrimaryKey(const QString& key);

		//! writes debug information to stderr
		void debug();

		/*! if table was created using a connection, 
			returns this connection object, otherwise NULL. */
		Connection* connection();
	protected:
		/*! Automatically retrieves table schema via connection. */
		Table(const QString & name, Connection *conn);

	//js	QStringList m_primaryKeys;
//		Field::List m_fields;
		Index::List m_indices;
		QString m_name;

		int m_id; //! unique identifier used in kexi__tables for this table

		Connection *m_conn;

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
