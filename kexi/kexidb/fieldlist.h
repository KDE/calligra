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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDB_FIELDLIST_H
#define KEXIDB_FIELDLIST_H

#include <qvaluelist.h>
#include <qstring.h>

#include <kexidb/field.h>

namespace KexiDB {

class Connection;

/*! Helper class that stores list of fields.
*/

class KEXI_DB_EXPORT FieldList
{
	public:
		unsigned int fieldCount() const;
		/*! Adds field at the and of field list. */
		virtual void addField(Field *field);
		/*! \return field #id or NULL if there is no such a field. */
		KexiDB::Field* field(unsigned int id);

//		Field::List::iterator fields() { return m_fields.begin(); }
//js		void addPrimaryKey(const QString& key);
//		void debug();
	protected:
		FieldList();
		~FieldList();

	//js	QStringList m_primaryKeys;
		Field::List m_fields;

//	friend class Connection;
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
