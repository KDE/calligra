/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
*/

#include <kexidb/relationship.h>

#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/driver.h>

#include <kdebug.h>

using namespace KexiDB;

Relationship::Relationship()
	: m_masterIndex(0)
	, m_detailsIndex(0)
	, m_masterIndexOwned(false)
	, m_detailsIndexOwned(false)
{
	m_pairs.setAutoDelete(true);
}

Relationship::Relationship(IndexSchema* masterIndex, IndexSchema* detailsIndex)
	: m_masterIndex(0)
	, m_detailsIndex(0)
	, m_masterIndexOwned(false)
	, m_detailsIndexOwned(false)
{
	m_pairs.setAutoDelete(true);
	setIndices(masterIndex, detailsIndex);
}

Relationship::Relationship( QuerySchema *query, Field *field1, Field *field2 )
	: m_masterIndex(0)
	, m_detailsIndex(0)
	, m_masterIndexOwned(false)
	, m_detailsIndexOwned(false)
{
	m_pairs.setAutoDelete(true);
	createIndices( query, field1, field2 );
}

Relationship::~Relationship()
{
	if (m_masterIndexOwned)
		delete m_masterIndex;
	if (m_detailsIndexOwned)
		delete m_detailsIndex;
}

void Relationship::createIndices( QuerySchema *query, Field *field1, Field *field2 )
{
	if (!field1 || !field2 || !query) {
		KexiDBWarn << "Relationship::addRelationship(): !masterField || !detailsField || !query" << endl;
		return;
	}
	if (field1->isQueryAsterisk() || field2->isQueryAsterisk()) {
		KexiDBWarn << "Relationship::addRelationship(): relationship's fields cannot be asterisks" << endl;
		return;
	}
	if (field1->table() == field2->table()) {
		KexiDBWarn << "Relationship::addRelationship(): fields cannot belong to the same table" << endl;
		return;
	}
//	if (!query->hasField(field1) && !query->hasField(field2)) {
	if (!query->contains(field1->table()) || !query->contains(field2->table())) {
		KexiDBWarn << "Relationship::addRelationship(): fields do not belong to this query" << endl;
		return;
	}
//@todo: check more things: -types
//@todo: find existing global db relationships

	Field *masterField = 0, *detailsField = 0;
	bool p1 = field1->isPrimaryKey(), p2 = field2->isPrimaryKey();
	if (p1 && p2) {
		//2 primary keys
		masterField = field1;
		m_masterIndex = masterField->table()->primaryKey();
		detailsField = field2;
		m_detailsIndex = detailsField->table()->primaryKey();
	}
	else if (!p1 && p2) {
		//foreign + primary: swap
		Field *tmp = field1;
		field1 = field2;
		field2 = tmp;
		p1 = !p1;
		p2 = !p2;
	}

	if (p1 && !p2) {
		//primary + foreign
		masterField = field1;
		m_masterIndex = masterField->table()->primaryKey();
		detailsField = field2;
		//create foreign key
//@todo: check if it already exists
		m_detailsIndex = new IndexSchema(detailsField->table());
		m_detailsIndexOwned = true;
		m_detailsIndex->addField(detailsField);
		m_detailsIndex->setForeignKey(true);
	}
	else if (!p1 && !p2) {
		masterField = field1;
		m_masterIndex = new IndexSchema(masterField->table());
		m_masterIndexOwned = true;
		m_masterIndex->addField(masterField);
		m_masterIndex->setForeignKey(true);
		
		detailsField = field2;
		m_detailsIndex = new IndexSchema(detailsField->table());
		m_detailsIndexOwned = true;
		m_detailsIndex->addField(detailsField);
		m_detailsIndex->setForeignKey(true);
	}

	if (!m_masterIndex || !m_detailsIndex)
		return; //failed

	setIndices(m_masterIndex, m_detailsIndex, false);
}

TableSchema* Relationship::masterTable() const
{
	return m_masterIndex ? m_masterIndex->table() : 0;
}

TableSchema* Relationship::detailsTable() const
{
	return m_detailsIndex ? m_detailsIndex->table() : 0;
}

void Relationship::setIndices(IndexSchema* masterIndex, IndexSchema* detailsIndex)
{
	setIndices(masterIndex, detailsIndex, true);
}

void Relationship::setIndices(IndexSchema* masterIndex, IndexSchema* detailsIndex, bool ownedByMaster)
{
	m_masterIndex = 0;
	m_detailsIndex = 0;
	m_pairs.clear();
	if (!masterIndex || !detailsIndex || !masterIndex->table() || !detailsIndex->table() 
	|| masterIndex->table()==detailsIndex->table() || masterIndex->fieldCount()!=detailsIndex->fieldCount())
		return;
	Field::ListIterator it1(*masterIndex->fields());
	Field::ListIterator it2(*detailsIndex->fields());
	for (;it1.current() && it2.current(); ++it1, ++it2) {
		Field *f1 = it1.current(); //masterIndex->fields()->first();
		Field *f2 = it2.current(); //detailsIndex->fields()->first();
	//	while (f1 && f2) {
		if (f1->type()!=f2->type() && f1->isIntegerType()!=f2->isIntegerType() && f1->isTextType()!=f2->isTextType()) {
			KexiDBWarn << "Relationship::setIndices(INDEX on '"<<masterIndex->table()->name()
			<<"',INDEX on "<<detailsIndex->table()->name()<<"): !equal field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
		}
#if 0 //too STRICT!
		if ((f1->isUnsigned() && !f2->isUnsigned()) || (!f1->isUnsigned() && f2->isUnsigned())) {
			KexiDBWarn << "Relationship::setIndices(INDEX on '"<<masterIndex->table()->name()
			<<"',INDEX on "<<detailsIndex->table()->name()<<"): !equal signedness of field types: "
			<<Driver::defaultSQLTypeName(f1->type())<<" "<<f1->name()<<", "
			<<Driver::defaultSQLTypeName(f2->type())<<" "<<f2->name() <<endl;
			m_pairs.clear();
			return;
		}
#endif
		m_pairs.append( new Field::Pair(f1,f2) );
	}
	//ok: update information
	if (m_masterIndex) {//detach yourself
		m_masterIndex->detachRelationship(this);
	}
	if (m_detailsIndex) {//detach yourself
		m_detailsIndex->detachRelationship(this);
	}
	m_masterIndex = masterIndex;
	m_detailsIndex = detailsIndex;
	m_masterIndex->attachRelationship(this, ownedByMaster);
	m_detailsIndex->attachRelationship(this, ownedByMaster);
}

