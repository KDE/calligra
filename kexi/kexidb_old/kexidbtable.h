/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef _KEXI_DB_TABLE
#define _KEXI_DB_TABLE
#include <qvaluelist.h>
#include "kexidbfield.h"
#include <qstring.h>


class KexiDBTable {
public:
	KexiDBTable(const QString & name);
	~KexiDBTable();
	const QString& tableName() const;
	unsigned int fieldCount() const;
	KexiDBField field(unsigned int id) const;
	QStringList primaryKeys() const;
	void addField(KexiDBField field);
	void addPrimaryKey(const QString& key);
private:
	QStringList m_primaryKeys;
	QValueList<KexiDBField> m_fields;
	QString m_tableName;

};


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

#endif
