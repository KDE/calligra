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

#include <kexidbtable.h>


KexiDBTable::KexiDBTable(const QString& name):m_tableName(name) {
}

KexiDBTable::KexiDBTable():m_tableName("") {
}


KexiDBTable::~KexiDBTable() {}

void KexiDBTable::addField(KexiDBField field) {
	field.setTable(m_tableName);
	m_fields.append(field);
}

void KexiDBTable::addPrimaryKey(const QString& key) {
	m_primaryKeys.append(key);
}

QStringList KexiDBTable::primaryKeys() const{
	return m_primaryKeys;
}

const QString& KexiDBTable::tableName() const {
	return m_tableName;
}

void KexiDBTable::setTableName(const QString& name) {
	m_tableName=name;
	int fcnt=m_fields.count();
	for (int i=0;i<fcnt;i++) {
		m_fields[i].setTable(name);
	}
}

KexiDBField KexiDBTable::field(unsigned int id) const {
	if (id<m_fields.count()) return m_fields[id];
	return KexiDBField();
}

unsigned int KexiDBTable::fieldCount() const {
	return m_fields.count();
}
